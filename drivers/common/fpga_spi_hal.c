/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "fpga_spi_hal.h"

#include <assert.h>
#include <stdint.h>

#include "bsp_pin_defs.h"
#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h" // FIXME: use index, not handle
#include "bus/spi_driver.h"
#include "gpio.h"
#include "log/log.h"
#include "crc16.h"

static const int SPI_TIMEOUT_MS = 100; // osWaitForever;

typedef enum {
    NSS_ASSERT = 0,
    NSS_DEASSERT = 1,
} NssState;

/**
 * @brief toggle NSS pin by software
 * @param state state of NSS pin (active low)
 */
static void fpga_spi_hal_spi_nss_b(NssState state)
{
    write_gpio_pin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, (state == NSS_DEASSERT) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief Read FPGA register
 * @param addr 15-bit address
 * @param data pointer to 16-bit destination
 * @return @arg true on success
 */
bool fpga_spi_hal_read_reg(BusInterface *bus, uint16_t addr, uint16_t *data)
{
    enum {Size = 2}; // number of 16-bit words
    uint16_t txBuf[2];
    uint16_t rxBuf[2] = {0};
    txBuf[0] = (0x8000 | (addr & 0x7FFF));
    txBuf[1] = 0;
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), (uint8_t *)txBuf, (uint8_t *)rxBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (ret && data) {
        uint16_t result = rxBuf[1];
        *data = result;
    }
    return ret;
}

/**
 * @brief Write FPGA register
 * @param addr 15-bit address
 * @param data 16-bit data to write
 * @return @arg true on success
 */
bool fpga_spi_hal_write_reg(BusInterface *bus, uint16_t addr, uint16_t data)
{
    enum {Size = 2};
    uint16_t txBuf[2];
    txBuf[0] = (0x0000 | (addr & 0x7FFF));
    txBuf[1] = data;
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx(hspi_handle(bus->bus_number), (uint8_t *)txBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (! ret) {
        log_printf(LOG_ERR, "fpga_spi_hal_write_reg: SPI error");
        return ret;
    }
    return ret;
}

enum {
    FPGA_SPI_V3_OP_NULL = 0,
    FPGA_SPI_V3_OP_WR   = 1,
    FPGA_SPI_V3_OP_RD   = 2,
    FPGA_SPI_V3_OP_ST   = 3,
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

uint32_t wswap_32(uint32_t data)
{
    return ((data & 0xFFFF) << 16) | ((data >> 16) & 0xFFFF);
}

uint64_t wswap_64(uint64_t data)
{
    return
            ((data >> 48) & 0xFFFF) |
            (((data >> 32) & 0xFFFF) << 16) |
            (((data >> 16) & 0xFFFF) << 32) |
            ((data & 0xFFFF) << 48);
}

static int op_count = 0;
enum {SPI_FRAME_LEN = sizeof(fpga_spi_v3_txn_t) / sizeof(uint16_t)};

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

    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), (uint8_t *)txBuf, (uint8_t *)rxBuf, wordcount, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    op_count++;
    if (0) {
        const fpga_spi_v3_txn_t *tx_struct = (const fpga_spi_v3_txn_t *)txBuf;
        int prio = ret ? LOG_DEBUG : LOG_WARNING;
        log_printf(prio,
                   "[%d] FPGA SPI << %04X  %04X %04X  %04X %04X %04X %04X  %04X, %s addr %X, data %llX",
                   op_count,
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
                   "[%d] FPGA SPI >> %04X  %04X %04X  %04X %04X %04X %04X  %04X, %s addr %X, data %llX (crc %s)",
                   op_count,
                   rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3],
                rxBuf[4], rxBuf[5], rxBuf[6], rxBuf[7],
                opcode_str(rx_struct->b.op.b.opcode),
                wswap_32(rx_struct->b.addr),
                wswap_64(rx_struct->b.data),
                crc_ok ? "ok" : "error");
    }
    return true;
}

bool fpga_spi_v3_txn(BusInterface *bus, fpga_spi_v3_txn_t *txBuf, fpga_spi_v3_txn_t *rxBuf)
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
    // STATUS opcode
    fpga_spi_v3_txn_t txBuf = {0};
    txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_ST;
    fpga_spi_v3_txn_t rxBuf = {0};
    if (!fpga_spi_v3_txn(bus, &txBuf, &rxBuf))
        return false;
    if (rxBuf.b.op.b.opcode != FPGA_SPI_V3_OP_ST) {
        log_printf(LOG_ERR, "%s: unexpected opcode %d, should be %d",
                   __func__, rxBuf.b.op.b.opcode, FPGA_SPI_V3_OP_ST);
        return false;
    }
    uint16_t regio_timeouts = (rxBuf.b.data >> 32) & 0xFFFF;
    uint16_t spi_crc_errors = (rxBuf.b.data >> 16) & 0xFFFF;
    uint16_t regio_errors   = (rxBuf.b.data) & 0xFFFF;
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
        fpga_spi_v3_txn_t txBuf = {0};
        txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_RD;
        txBuf.b.addr = addr;
        fpga_spi_v3_txn_t rxBuf = {0};
        if (!fpga_spi_v3_txn(bus, &txBuf, &rxBuf))
            return false;
    }

    // NULL opcode
    const int max_retry = 2;
    for (int i=0; i<max_retry; i++) {
        fpga_spi_v3_txn_t txBuf = {0};
        txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_NULL;
        fpga_spi_v3_txn_t rxBuf = {0};
        if (!fpga_spi_v3_txn(bus, &txBuf, &rxBuf))
            return false;
        if (rxBuf.b.op.b.opcode == FPGA_SPI_V3_OP_RD) {
            if (rxBuf.b.op.b.length != 1) {
                log_printf(LOG_ERR, "%s: unexpected length %d, should be 1",
                           __func__, addr, rxBuf.b.op.b.length);
                return false;
            }
            if (rxBuf.b.addr != addr) {
                log_printf(LOG_ERR, "%s: address mismatch: request %08X != reply %08X",
                           __func__, addr, rxBuf.b.addr);
                return false;
            }
            if (data) {
                *data = rxBuf.b.data;
            }
            return true;
        }
        osDelay(1);
    }
    log_printf(LOG_ERR, "%s: retry limit exceeded", __func__);
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
    fpga_spi_v3_txn_t txBuf = {0};
    txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_WR;
    txBuf.b.op.b.length = SPI_FRAME_LEN-3;
    txBuf.b.addr = wswap_32(addr);
    txBuf.b.data = wswap_64(data);
    txBuf.b.crc = crc16_be16((uint16_t *)(&txBuf), SPI_FRAME_LEN-1);

    fpga_spi_v3_txn_t rxBuf = {0};
    if (!fpga_spi_v3_tx_rx(bus, txBuf.raw, rxBuf.raw, SPI_FRAME_LEN))
        return false;

    return true;
}

void fpga_enable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, true);
}

void fpga_disable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, false);
}
