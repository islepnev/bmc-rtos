/*
**    VXS IIC Slave
**
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

#include "dev_vxsiics.h"

#include <string.h>

#include "app_shared_data.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "dev_vxsiics_stats.h"
#include "devices_types.h"
#include "fpga/dev_fpga_types.h"
#include "i2c.h"
#include "ipmi_sensors.h"
#include "led_gpio_hal.h"
#include "logbuffer.h"
#include "stm32f7xx_ll_i2c.h"
#include "system_status.h"
#include "version.h"

#define SLAVE_OWN_ADDRESS 0x33

static const uint32_t BMC_MAGIC = 0x424D4320;
enum { MAX_MSG_SIZE = 40 };
static uint8_t rx_buf[MAX_MSG_SIZE];
static uint8_t rx_buf_len = 0;
static uint16_t mem_write = 0;
static uint16_t mem_addr = 0;
static uint32_t mem_rxdata = 0;
enum {MEM_START_ADDR = 8};
enum {MEM_SIZE = 4};
static uint32_t mem[MEM_SIZE] = {0};
static uint16_t byte_count = 0;

void dev_vxsiics_init(void)
{
    vxsiic_i2c_stats_t zz = {0};
    vxsiic_i2c_stats = zz;

    NVIC_DisableIRQ(I2C1_EV_IRQn);
    //    LL_I2C_Disable(I2C1);
    //    LL_I2C_Enable(I2C1);
    LL_I2C_EnableIT_ADDR(I2C1);
    LL_I2C_EnableIT_NACK(I2C1);
    LL_I2C_EnableIT_ERR(I2C1);
    LL_I2C_EnableIT_STOP(I2C1);
//    NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
//    NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(I2C1_EV_IRQn);
}

static uint32_t mem_txdata = 0;

static void iic_write_callback(uint16_t addr, uint32_t data)
{
    // writable scratch pad
    if (addr >= MEM_START_ADDR && addr < MEM_START_ADDR + MEM_SIZE) {
        mem[addr - MEM_START_ADDR] = data;
    }
}

static void iic_read_callback(uint16_t addr, uint32_t *data)
{
    if (!data)
        return;
    if (addr >= IIC_SENSORS_MAP_START && addr < IIC_SENSORS_MAP_START + IIC_SENSORS_MAP_SIZE_BYTES / 4) {
        uint32_t offset = addr - IIC_SENSORS_MAP_START;
        uint32_t *ptr = (uint32_t *)&ipmi_sensors + offset;
        memcpy(data, ptr, sizeof(uint32_t));
    } else if (addr >= MEM_START_ADDR && addr < MEM_START_ADDR + MEM_SIZE) {
        *data = mem[addr - MEM_START_ADDR];
    } else {
        switch (addr) {
        case 0:
            *data = BMC_MAGIC;
            break;
        case 1:
            *data = ((uint32_t)(VERSION_MAJOR_NUM) << 16) | (uint16_t)(VERSION_MINOR_NUM);
            break;
        case 2:
            *data = get_fpga_id();
            break;
        case 3: {
            *data = encode_system_status().w;
            break;
        }
        case 4:
            *data = vxsiic_i2c_stats.ops;
            break;
        case 5:
            *data = vxsiic_i2c_stats.errors;
            break;
        case 6:
            *data = osKernelSysTick() / osKernelSysTickFrequency;
            break;
        default:
            *data = 0;
            break;
        }
    }
}

static void Slave_Ready_To_Transmit_Callback(void)
{
    if (byte_count == 2) {
        iic_read_callback(mem_addr, &mem_txdata);
    }

    uint32_t tx_data = mem_txdata >> ((byte_count-2)*8);
    LL_I2C_TransmitData8(I2C1, tx_data);
}

static void Slave_Complete_Callback(void)
{
//    debug_printf("%s %d %04X %08X\n", mem_write ? "WRITE" : "READ ", byte_count, mem_addr, mem_write ? mem_rxdata : mem_txdata);
    if (mem_write) {
        iic_write_callback(mem_addr, mem_rxdata);
    }
    mem_addr = 0;
    mem_rxdata = 0;
}

void i2c_event_interrupt_handler(void)
{
    if (LL_I2C_IsActiveFlag_ADDR(I2C1))
    {
        if (SLAVE_OWN_ADDRESS == (LL_I2C_GetAddressMatchCode(I2C1) >> 1))
        {
            if (LL_I2C_DIRECTION_READ == LL_I2C_GetTransferDirection(I2C1))
            {
//                debug_printf("%d ADDR-READ\n", byte_count);
                LL_I2C_ClearFlag_ADDR(I2C1);
                mem_write = 0;
                LL_I2C_EnableIT_TX(I2C1);
            }
            else
            {
//                debug_printf("%d ADDR-WRITE\n", byte_count);
                LL_I2C_ClearFlag_ADDR(I2C1);
                mem_write = 1;
                LL_I2C_EnableIT_RX(I2C1);
            }
        }
        else
        {
            LL_I2C_ClearFlag_ADDR(I2C1);
            vxsiic_i2c_stats.errors++;
            log_put(LOG_ERR, "vxsiic: I2C event interrupt address error");
        }
    }
    else if (LL_I2C_IsActiveFlag_NACK(I2C1))
    {
//        debug_printf("%d NACK\n", byte_count);
        LL_I2C_ClearFlag_NACK(I2C1);
    }
    else if (LL_I2C_IsActiveFlag_STOP(I2C1))
    {
//        debug_printf("%d STOP\n", byte_count);
        LL_I2C_ClearFlag_STOP(I2C1);
        if (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
            LL_I2C_ClearFlag_TXE(I2C1);
        }
        Slave_Complete_Callback();
        vxsiic_i2c_stats.ops++;
        byte_count = 0;
        rx_buf_len = 0;
    }
    else if (LL_I2C_IsActiveFlag_TXIS(I2C1))
    {
//        debug_printf("%d TXIS\n", byte_count);
        Slave_Ready_To_Transmit_Callback();
        byte_count++;
    }
    else if (LL_I2C_IsActiveFlag_RXNE(I2C1)) {
//        debug_printf("%d RXNE\n", byte_count);
        uint8_t rx = LL_I2C_ReceiveData8(I2C1);
        if (byte_count == 0) {
            mem_addr = 0;
            mem_rxdata = 0;
        }
        if (byte_count >= 2) {
            if (rx_buf_len < MAX_MSG_SIZE) {
                rx_buf[rx_buf_len++] = rx;
            }
        }
        if (byte_count < 2) {
            mem_addr |= (uint16_t)rx << ((1-byte_count)*8);
        } else if (byte_count < 6) {
            mem_rxdata |= (uint32_t)rx << ((byte_count-2)*8);
        }
        byte_count++;
    }
    else if (!LL_I2C_IsActiveFlag_TXE(I2C1))
    {
        /* Do nothing */
        /* This Flag will be set by hardware when the TXDR register is empty */
        /* If needed, use LL_I2C_ClearFlag_TXE() interface to flush the TXDR register  */
    }
    else
    {
        vxsiic_i2c_stats.errors++;
        log_put(LOG_ERR, "vxsiic: unexpected I2C event interrupt");
    }
}

typedef union {
  struct {
      uint32_t txe:1;
      uint32_t txis:1;
      uint32_t rxne:1;
      uint32_t addr:1;
      uint32_t nackf:1;
      uint32_t stopf:1;
      uint32_t tc:1;
      uint32_t tcr:1;
      uint32_t berr:1;
      uint32_t arlo:1;
      uint32_t ovr:1;
      uint32_t pecerr:1;
      uint32_t timeout:1;
      uint32_t alert:1;
      uint32_t busy:1;
      uint32_t dir:1;
      uint32_t addcode:15;
  } b;
  uint32_t raw;
} stm32f7xx_i2c_isr_t;

void i2c_error_interrupt_handler(void)
{
    stm32f7xx_i2c_isr_t isr;
    isr.raw = I2C1->ISR;
    log_printf(LOG_ERR, "vxsiic: I2C ERROR (addr 0x%02X): ISR=%08lX %s%s%s\n",
                 LL_I2C_GetAddressMatchCode(I2C1) / 2,
                 isr.raw,
                 LL_I2C_IsActiveFlag_ARLO(I2C1) ? " ARLO" : "",
                 LL_I2C_IsActiveFlag_BERR(I2C1) ? " BERR" : "",
                 LL_I2C_IsActiveFlag_OVR(I2C1)  ? " OVR" : ""
                                                  );
    if (LL_I2C_IsActiveFlag_ARLO(I2C1)) {
        LL_I2C_ClearFlag_ARLO(I2C1);
    }
    if (LL_I2C_IsActiveFlag_BERR(I2C1)) {
        LL_I2C_ClearFlag_BERR(I2C1);
    }
    if (LL_I2C_IsActiveFlag_OVR(I2C1)) {
        LL_I2C_ClearFlag_OVR(I2C1);
    }
    vxsiic_i2c_stats.errors++;
}
