/*
**    Copyright 2022 Ilia Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "fpga_spi_v3.h"

#include <assert.h>
#include <string.h>

#include "cmsis_os.h"
#include "crc16.h"
#include "fpga_spi_hal.h"
#include "log/log.h"
#include "wswap.h"

enum {
    FPGA_SPI_V3_OP_NULL = 0,
    FPGA_SPI_V3_OP_WR   = 1,
    FPGA_SPI_V3_OP_RD   = 2,
    FPGA_SPI_V3_OP_ST   = 3
};

#pragma pack(push, 1)
typedef union {
    struct {
        uint32_t length:8;
        uint32_t reserved:6;
        uint32_t opcode:2;
    } b;
    uint16_t raw;
} fpga_spi_v3_op_t;

typedef union {
    struct {
        fpga_spi_v3_op_t op;
        uint32_t addr;
        uint64_t data;
        uint16_t crc;
    } b;
    uint16_t raw[8];
} fpga_spi_v3_txn_t;

#pragma pack(pop)

enum {SPI_FRAME_LEN = sizeof(fpga_spi_v3_txn_t) / sizeof(uint16_t)};

// global buffers (reduce stack allocations)
static fpga_spi_v3_txn_t v3_txBuf = {0};
static fpga_spi_v3_txn_t v3_rxBuf = {0};

void clear_v3_buf()
{
    memset(&v3_txBuf.raw[0], 0, sizeof(v3_txBuf.raw));
    memset(&v3_rxBuf.raw[0], 0, sizeof(v3_rxBuf.raw));
}

static const char *opcode_str(int op)
{
    switch (op) {
    case FPGA_SPI_V3_OP_NULL: return "NULL";
    case FPGA_SPI_V3_OP_RD: return "  RD";
    case FPGA_SPI_V3_OP_WR: return "  WR";
    case FPGA_SPI_V3_OP_ST: return "  ST";
    }
    return "?";
}

bool fpga_spi_v3_tx_rx(BusInterface *bus, uint16_t *txBuf, uint16_t *rxBuf, uint16_t wordcount)
{
    assert(wordcount == SPI_FRAME_LEN);
    if (wordcount != SPI_FRAME_LEN)
        return false;

    txBuf[wordcount-1] = crc16_be16(txBuf, wordcount-1);

    bool ret = fpga_spi_hal_tx_rx(bus, (uint8_t *)txBuf, (uint8_t *)rxBuf, wordcount);
    if (0) {
        const fpga_spi_v3_txn_t *tx_struct = (const fpga_spi_v3_txn_t *)txBuf;
        int prio = ret ? LOG_DEBUG : LOG_WARNING;
        log_printf(prio,
                   "FPGA SPI << %04X  %04X %04X  %04X %04X %04X %04X  %04X, %s addr %X, data %llX",
                   txBuf[0], txBuf[1], txBuf[2], txBuf[3],
                   txBuf[4], txBuf[5], txBuf[6], txBuf[7],
                   opcode_str(tx_struct->b.op.b.opcode),
                   wswap_32(tx_struct->b.addr),
                   wswap_64(tx_struct->b.data));
    }
    if (! ret) {
        log_printf(LOG_ERR, "%s: spi_driver_tx_rx error", __func__);
        return false;
    }
    if (0) {
        const fpga_spi_v3_txn_t *rx_struct = (const fpga_spi_v3_txn_t *)rxBuf;
        uint16_t calc_crc = crc16_be16(rxBuf, wordcount-1);
        bool crc_ok = (calc_crc == rxBuf[wordcount-1]);
        int prio = (crc_ok) ? LOG_DEBUG : LOG_WARNING;
        log_printf(prio,
                   "FPGA SPI >> %04X  %04X %04X  %04X %04X %04X %04X  %04X, %s addr %X, data %llX (crc %s)",
                   rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3],
                   rxBuf[4], rxBuf[5], rxBuf[6], rxBuf[7],
                   opcode_str(rx_struct->b.op.b.opcode),
                   wswap_32(rx_struct->b.addr),
                   wswap_64(rx_struct->b.data),
                   crc_ok ? "ok" : "error");
    }
    return true;
}

static bool fpga_spi_v3_txn(BusInterface *bus, fpga_spi_v3_txn_t *txBuf, fpga_spi_v3_txn_t *rxBuf)
{
    txBuf->b.op.b.opcode = txBuf->b.op.b.opcode;
    txBuf->b.op.b.length = SPI_FRAME_LEN-3;
    txBuf->b.addr = wswap_32(txBuf->b.addr);
    txBuf->b.data = wswap_64(txBuf->b.data);

    if (!fpga_spi_v3_tx_rx(bus, txBuf->raw, rxBuf->raw, SPI_FRAME_LEN))
        return false;

    rxBuf->b.addr = wswap_32(rxBuf->b.addr);
    rxBuf->b.data = wswap_64(rxBuf->b.data);

    return true;
}

bool fpga_spi_v3_hal_read_status(BusInterface *bus)
{
    clear_v3_buf();
    // STATUS opcode
    v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_ST;
    if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
        return false;
    if (v3_rxBuf.b.op.b.opcode != FPGA_SPI_V3_OP_ST) {
        log_printf(LOG_ERR, "%s: unexpected opcode %d, should be %d",
                   __func__, v3_rxBuf.b.op.b.opcode, FPGA_SPI_V3_OP_ST);
        return false;
    }
    uint16_t regio_timeouts = (v3_rxBuf.b.data >> 32) & 0xFFFF;
    uint16_t spi_crc_errors = (v3_rxBuf.b.data >> 16) & 0xFFFF;
    uint16_t regio_errors   = (v3_rxBuf.b.data) & 0xFFFF;
    log_printf(LOG_DEBUG, "%s: RegIO timeouts: %d, RegIO errors: %d, SPI CRC errors: %d",
               __func__, regio_timeouts, regio_errors, spi_crc_errors);
    return true;
}

/**
 * @brief Read FPGA register
 * @param addr 15-bit address
 * @param data pointer to 16-bit destination
 * @return @arg true on success
 */
bool fpga_spi_v3_hal_read_reg(BusInterface *bus, uint32_t addr, uint64_t *data)
{
    // READ opcode
    {
        clear_v3_buf();
        v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_RD;
        v3_txBuf.b.addr = addr;
        if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
            return false;
    }

    // NULL opcode
    const int max_retry = 2;
    for (int i=0; i<max_retry; i++) {
        clear_v3_buf();
        v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_NULL;
        if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
            return false;
        if (v3_rxBuf.b.op.b.opcode == FPGA_SPI_V3_OP_RD) {
            //            if (rxBuf.b.op.b.length != 1) {
            //                log_printf(LOG_ERR, "%s: unexpected length %d, should be 1",
            //                           __func__, addr, rxBuf.b.op.b.length);
            //                return false;
            //            }
            if (v3_rxBuf.b.addr != addr) {
                log_printf(LOG_ERR, "%s: address mismatch: request %08X != reply %08X",
                           __func__, addr, v3_rxBuf.b.addr);
                return false;
            }
            if (data) {
                *data = v3_rxBuf.b.data;
            }
            return true;
        }
        osDelay(1);
    }
    log_printf(LOG_ERR, "%s(%08X): retry limit exceeded", __func__, addr);
    return false;
}

/**
 * @brief Write FPGA register
 * @param addr 32-bit address
 * @param data 32-bit data to write
 * @return @arg true on success
 */
bool fpga_spi_v3_hal_write_reg(BusInterface *bus, uint32_t addr, uint64_t data)
{
    clear_v3_buf();
    v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_WR;
    v3_txBuf.b.op.b.length = SPI_FRAME_LEN-3;
    v3_txBuf.b.addr = wswap_32(addr);
    v3_txBuf.b.data = wswap_64(data);
    v3_txBuf.b.crc = crc16_be16((uint16_t *)(&v3_txBuf), SPI_FRAME_LEN-1);

    if (!fpga_spi_v3_tx_rx(bus, v3_txBuf.raw, v3_rxBuf.raw, SPI_FRAME_LEN))
        return false;

    return true;
}
