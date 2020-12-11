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

#include <stdint.h>

#include "bsp_pin_defs.h"
#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h" // FIXME: use index, not handle
#include "bus/spi_driver.h"
#include "gpio.h"
#include "log/log.h"

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
        log_printf(LOG_ERR, "fpga_spi_hal_write_reg: SPI error\n");
        return ret;
    }
    return ret;
}

enum {
    FPGA_SPI_V3_OP_NULL = 0,
    FPGA_SPI_V3_OP_WR   = 1,
    FPGA_SPI_V3_OP_RD   = 2,
    FPGA_SPI_V3_OP_RSVD = 3,
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

/**
 * @brief Read FPGA register
 * @param addr 15-bit address
 * @param data pointer to 16-bit destination
 * @return @arg true on success
 */
bool fpga_spi_v3_hal_read_reg(BusInterface *bus, uint32_t addr, uint64_t *data)
{
    log_printf(LOG_DEBUG, "fpga_spi_v3_hal_read_reg: %08X\n", addr);
    {
        fpga_spi_v3_txn_t txBuf = {0};
        txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_RD;
        txBuf.b.op.b.length = 5;
        txBuf.b.addr = (addr << 16);
        txBuf.b.data = 0xfedcba0987654321;
        txBuf.b.crc = 0x5555;
        fpga_spi_v3_txn_t rxBuf;

        log_printf(LOG_DEBUG, "[0] FPGA SPI TXBUF: %04X   %04X %04X   %04X %04X %04X %04X   %04X, %16llX\n",
                   txBuf.raw[0], txBuf.raw[1], txBuf.raw[2], txBuf.raw[3],
                   txBuf.raw[4], txBuf.raw[5], txBuf.raw[6], txBuf.raw[7], txBuf.b.data);
        fpga_spi_hal_spi_nss_b(NSS_ASSERT);
        bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), (uint8_t *)txBuf.raw, (uint8_t *)rxBuf.raw, sizeof(txBuf) / sizeof(uint16_t), SPI_TIMEOUT_MS);
        fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
        if (! ret) {
            log_printf(LOG_ERR, "fpga_spi_v3_hal_read_reg: SPI error\n");
            return false;
        }

        log_printf(LOG_DEBUG, "[0] FPGA SPI RXBUF: %04X   %04X %04X   %04X %04X %04X %04X   %04X\n",
                   rxBuf.raw[0], rxBuf.raw[1], rxBuf.raw[2], rxBuf.raw[3],
                   rxBuf.raw[4], rxBuf.raw[5], rxBuf.raw[6], rxBuf.raw[7]);
    }
    fpga_spi_v3_txn_t txBuf = {0};
    txBuf.b.op.b.opcode = FPGA_SPI_V3_OP_NULL;

    fpga_spi_v3_txn_t rxBuf;
    const int max_retry = 2;
    bool complete = false;
    for (int i=0; i<max_retry; i++) {
        log_printf(LOG_DEBUG, "[%d] FPGA SPI TXBUF: %04X   %04X %04X   %04X %04X %04X %04X   %04X\n", i+1,
                   txBuf.raw[0], txBuf.raw[1], txBuf.raw[2], txBuf.raw[3],
                   txBuf.raw[4], txBuf.raw[5], txBuf.raw[6], txBuf.raw[7]);
        fpga_spi_hal_spi_nss_b(NSS_ASSERT);
        bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), (uint8_t *)txBuf.raw, (uint8_t *)rxBuf.raw, sizeof(txBuf) / sizeof(uint16_t), SPI_TIMEOUT_MS);
        fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
        if (! ret) {
            log_printf(LOG_ERR, "fpga_spi_v3_hal_read_reg: SPI error\n");
            return false;
        }
        log_printf(LOG_DEBUG, "[%d] FPGA SPI RXBUF: %04X   %04X %04X   %04X %04X %04X %04X   %04X\n", i+1,
                   rxBuf.raw[0], rxBuf.raw[1], rxBuf.raw[2], rxBuf.raw[3],
                   rxBuf.raw[4], rxBuf.raw[5], rxBuf.raw[6], rxBuf.raw[7]);
        if (rxBuf.b.op.b.opcode == FPGA_SPI_V3_OP_RD) {
            log_printf(LOG_DEBUG, "fpga_spi_v3_hal_read_reg: read complete: len %d, addr %08X, data %08X, crc %04X\n",
                       rxBuf.b.op.b.length, rxBuf.b.addr, rxBuf.b.data, rxBuf.b.crc);
            complete = true;
            break;
        }
    }
    if (! complete) {
        log_printf(LOG_ERR, "fpga_spi_v3_hal_read_reg: retry limit exceeded\n");
        return false;
    }

    if (data) {
        *data = rxBuf.b.data;
    }
    return true;
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
    txBuf.b.op.b.length = 5;
    txBuf.b.addr = addr;
    txBuf.b.data = data;
    txBuf.b.crc = 0x5555;
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx(hspi_handle(bus->bus_number), (uint8_t *)txBuf.raw, sizeof(txBuf) / sizeof(uint16_t), SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (! ret) {
        log_printf(LOG_ERR, "fpga_spi_v3_hal_write_reg: SPI error\n");
        return false;
    }
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
