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
#include <stdio.h>

#include "cmsis_os.h"
#include "crc16.h"
#include "fpga_spi_hal.h"
#include "fpga_spi_iostat.h"
#include "log/log.h"
#include "wswap.h"

static bool debug_spi_transactions = false;
static bool enable_length_check = false; // FIXME
static const uint32_t addr_op_stat = 0x57A714F0;
static const uint32_t addr_op_null = 0xDEADCA5E;


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
static bool v3_rx_crc_error = 0;
static bool v3_rx_addr_error = 0;

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

static uint16_t fpga_spi_v3_crc(const fpga_spi_v3_txn_t *txn)
{
    const int size = sizeof(fpga_spi_v3_txn_t) / 2;
    const uint16_t *buf = &txn->raw[0];
    return crc16_be16(buf, size-1);
}

static int error_log_count = 0;
static int error_log_limit = 10;

bool error_log_inc()
{
    error_log_count++;
    if (error_log_count == error_log_limit) {
        log_printf(LOG_INFO, "FPGA SPI: logging suppressed");
    }
    return error_log_count < error_log_limit;
}

enum {LOGBUF_SIZE = 200};
static char logbuf[LOGBUF_SIZE];

static void log_tx_txn(const BusInterface *bus, const uint16_t *txBuf, uint16_t wordcount)
{
    if (error_log_count >= error_log_limit)
        return;
    if (wordcount < SPI_FRAME_LEN) {
        log_printf(LOG_ERR, "FPGA SPI: invalid TX frame length %d", wordcount);
        return;
    }
    const fpga_spi_v3_txn_t *tx_struct = (const fpga_spi_v3_txn_t *)txBuf;
    snprintf(logbuf, sizeof(logbuf),
             "FPGA SPI [%ld] << %04X  %04X %04X  %04X %04X %04X %04X  %04X, "
             "%s len %d, addr %lX, data %llX",
             (ulong)iostat.tx_count,
             txBuf[0], txBuf[1], txBuf[2], txBuf[3],
             txBuf[4], txBuf[5], txBuf[6], txBuf[7],
             opcode_str(tx_struct->b.op.b.opcode),
             tx_struct->b.op.b.length,
             (ulong)wswap_32(tx_struct->b.addr),
             wswap_64(tx_struct->b.data));
    log_put(LOG_DEBUG, logbuf);
}

static void log_rx_txn(const BusInterface *bus, const uint16_t *rxBuf, uint16_t wordcount)
{
    if (error_log_count >= error_log_limit)
        return;
    if (wordcount < SPI_FRAME_LEN) {
        log_printf(LOG_ERR, "FPGA SPI: invalid frame length %d", wordcount);
        return;
    }
    const fpga_spi_v3_txn_t *rx_struct = (const fpga_spi_v3_txn_t *)rxBuf;
    uint16_t calc_crc = crc16_be16(rxBuf, wordcount-1);
    bool crc_ok = (calc_crc == rxBuf[wordcount-1]);
    int prio = (crc_ok) ? LOG_DEBUG : LOG_WARNING;
    snprintf(logbuf, sizeof(logbuf), "FPGA SPI [%ld] >> %04X  %04X %04X  %04X %04X %04X %04X  %04X, "
                                     "%s len %d, addr %lX, data %llX (crc %04X%s%04X %s)",
             (ulong)iostat.tx_count, // not rx_count
             rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3],
             rxBuf[4], rxBuf[5], rxBuf[6], rxBuf[7],
             opcode_str(rx_struct->b.op.b.opcode),
             rx_struct->b.op.b.length,
             (ulong)wswap_32(rx_struct->b.addr),
             wswap_64(rx_struct->b.data),
             rxBuf[wordcount-1],
             crc_ok ? "==" : "!=",
             calc_crc,
             crc_ok ? "ok" : "error");
    log_put(prio, logbuf);
}

bool fpga_spi_v3_tx_rx(BusInterface *bus, uint16_t *txBuf, uint16_t *rxBuf, uint16_t wordcount)
{
    assert(wordcount == SPI_FRAME_LEN);
    if (wordcount != SPI_FRAME_LEN)
        return false;

    txBuf[wordcount-1] = crc16_be16(txBuf, wordcount-1);

    bool ret = fpga_spi_hal_tx_rx(bus, (uint8_t *)txBuf, (uint8_t *)rxBuf, wordcount);
    if (debug_spi_transactions && error_log_count < error_log_limit) {
        log_tx_txn(bus, txBuf, wordcount);
        log_rx_txn(bus, rxBuf, wordcount);
    }

    if (! ret) {
        if (error_log_inc())
            log_printf(LOG_ERR, "%s: spi_driver_tx_rx error", __func__);
        iostat.hal_errors++;
        return false;
    }
    iostat.tx_count++;
    uint32_t sum = 0;
    for (int i=0; i<wordcount; i++)
        sum += rxBuf[i];
    if (sum == 0 || sum == (uint32_t)0xFFFFu * wordcount) {
        // if (error_log_inc())
        //     log_printf(LOG_WARNING, "FPGA SPI: no data received");
        iostat.no_response_errors++;
        return false;
    }
    iostat.rx_count++;
    uint16_t rx_crc = crc16_be16(rxBuf, wordcount-1);
    bool rx_crc_error = rx_crc != rxBuf[wordcount-1];
    if (rx_crc_error) {
        iostat.rx_crc_errors++;
        if (error_log_inc()) {
            log_printf(LOG_WARNING,
                       "FPGA SPI: RX CRC error: %04X != calculated %04X",
                       rxBuf[wordcount-1], rx_crc);
            log_rx_txn(bus, rxBuf, wordcount);
        }
    }
    return !rx_crc_error;
}

static bool fpga_spi_v3_txn(BusInterface *bus, fpga_spi_v3_txn_t *txBuf, fpga_spi_v3_txn_t *rxBuf)
{
    txBuf->b.op.b.length = SPI_FRAME_LEN-3;
    txBuf->b.addr = wswap_32(txBuf->b.addr);
    txBuf->b.data = wswap_64(txBuf->b.data);

    if (!fpga_spi_v3_tx_rx(bus, txBuf->raw, rxBuf->raw, SPI_FRAME_LEN))
        return false;

    uint16_t crc = fpga_spi_v3_crc(rxBuf);
    v3_rx_crc_error = (crc != rxBuf->b.crc);
    if (v3_rx_crc_error) {
        iostat.rx_crc_errors++;
        if (error_log_inc()) {
            log_printf(LOG_ERR, "FPGA SPI: RX CRC error");
            log_tx_txn(bus, (const uint16_t *)&v3_txBuf, SPI_FRAME_LEN);
            log_rx_txn(bus, (const uint16_t *)&v3_rxBuf, SPI_FRAME_LEN);
        }
    }

    rxBuf->b.addr = wswap_32(rxBuf->b.addr);
    rxBuf->b.data = wswap_64(rxBuf->b.data);
    v3_rx_addr_error = 0;
    switch (rxBuf->b.op.b.opcode) {
    case FPGA_SPI_V3_OP_NULL:
        v3_rx_addr_error = (rxBuf->b.addr != addr_op_null);
        break;
    case FPGA_SPI_V3_OP_ST:
        v3_rx_addr_error = (rxBuf->b.addr != addr_op_stat);
        break;
    default:;
    }
    if (v3_rx_addr_error)
        iostat.rx_addr_errors++;
    return true;
}

static void add_spi_stat(BusInterface *bus, const fpga_spi_v3_txn_t *txn)
{
    static uint16_t prev_regio_timeouts = 0;
    static uint16_t prev_spi_crc_errors = 0;
    static uint16_t prev_regio_errors = 0;
    if (!txn)
        return;
    uint16_t regio_timeouts = (txn->b.data >> 48) & 0xFFFF;
    uint16_t spi_crc_errors = (txn->b.data >> 32) & 0xFFFF;
    uint16_t regio_errors   = (txn->b.data >> 16) & 0xFFFF;

    uint16_t inc_regio_timeouts = regio_timeouts - prev_regio_timeouts;
    if (inc_regio_timeouts > 0x8000u)
        inc_regio_timeouts = regio_timeouts;
    prev_regio_timeouts = regio_timeouts;

    uint16_t inc_spi_crc_errors = spi_crc_errors - prev_spi_crc_errors;
    if (inc_spi_crc_errors > 0x8000u)
        inc_spi_crc_errors = spi_crc_errors;
    prev_spi_crc_errors = spi_crc_errors;

    uint16_t inc_regio_errors = regio_errors - prev_regio_errors;
    if (inc_regio_errors > 0x8000u)
        inc_regio_errors = regio_errors;
    prev_regio_errors = regio_errors;

    iostat.bus_timeouts += inc_regio_timeouts;
    iostat.tx_crc_errors += inc_spi_crc_errors;
    iostat.bus_errors += inc_regio_errors;
}

const int max_retry = 2;

bool fpga_spi_v3_hal_read_status(BusInterface *bus)
{
    // STATUS opcode
    {
        clear_v3_buf();
        v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_ST;
        if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
            return false;
    }
    // NULL opcode
    for (int i=0; i<max_retry; i++) {
        clear_v3_buf();
        v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_NULL;
        if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
            return false;
        if (v3_rx_crc_error || v3_rx_addr_error)
            continue;
        switch (v3_rxBuf.b.op.b.opcode) {
        case FPGA_SPI_V3_OP_NULL: break;
        case FPGA_SPI_V3_OP_ST: {
            add_spi_stat(bus, &v3_rxBuf);
            return true;
        }
        default: {
            iostat.rx_opcode_errors++;
            if (error_log_inc())
                log_printf(LOG_ERR, "FPGA SPI: unexpected opcode %d, should be %d",
                           v3_rxBuf.b.op.b.opcode, FPGA_SPI_V3_OP_ST);
            break;
        }
        }
        osDelay(1);
    }
    iostat.rx_timeouts++;
    if (error_log_inc())
        log_printf(LOG_ERR, "%s: retry limit exceeded", __func__);

    return false;
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
    for (int i=0; i<max_retry; i++) {
        clear_v3_buf();
        v3_txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_NULL;
        if (!fpga_spi_v3_txn(bus, &v3_txBuf, &v3_rxBuf))
            return false;
        if (v3_rx_crc_error || v3_rx_addr_error)
            continue;
        switch (v3_rxBuf.b.op.b.opcode) {
        case FPGA_SPI_V3_OP_NULL: break;
        case  FPGA_SPI_V3_OP_RD: {
            if (v3_rxBuf.b.op.b.length != 1) {
                iostat.rx_len_errors++;
                if (enable_length_check) {
                    if (error_log_inc())
                        log_printf(LOG_ERR, "%s: unexpected length %d, should be 1",
                                   __func__, v3_rxBuf.b.op.b.length);
                    continue;
                }
            }
            if (v3_rxBuf.b.addr != addr) {
                iostat.rx_addr_errors++;
                if (error_log_inc())
                    log_printf(LOG_ERR, "%s: address mismatch: request %08X != reply %08X, data=%08llX, crc=%04X",
                               __func__, addr, v3_rxBuf.b.addr, v3_rxBuf.b.data, v3_rxBuf.b.crc);
                //return false;
                continue;
            }
            if (data) {
                *data = v3_rxBuf.b.data;
            }
            return true;
        }
        default: {
            iostat.rx_opcode_errors++;
            if (error_log_inc())
                log_printf(LOG_ERR, "FPGA SPI: unexpected opcode %d, should be %d",
                           v3_rxBuf.b.op.b.opcode, FPGA_SPI_V3_OP_RD);
            break;
        }
        }
        osDelay(1);
    }
    iostat.rx_timeouts++;
    if (error_log_inc())
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
