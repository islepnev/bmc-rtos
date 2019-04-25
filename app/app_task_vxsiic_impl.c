/*
**    Copyright 2019 Ilja Slepnev
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

#include "app_task_vxsiic_impl.h"
#include <string.h>
#include "i2c.h"
#include "led_gpio_hal.h"
#include "stm32f7xx_ll_i2c.h"
#include "debug_helpers.h"
#include "cmsis_os.h"
#include "app_shared_data.h"
#include "version.h"
#include "system_status.h"
#include "ipmi_sensors.h"

#define SLAVE_OWN_ADDRESS 0x33
void Error_Callback(void);
void Slave_Complete_Callback(void);
void Slave_Ready_To_Transmit_Callback(void);

static const uint32_t BMC_MAGIC = 0x424D4320;
enum { MAX_MSG_SIZE = 40 };
static uint8_t rx_buf[MAX_MSG_SIZE];
static uint8_t rx_buf_len = 0;
static uint16_t mem_write = 0;
static uint16_t mem_addr = 0;
static uint32_t mem_rxdata = 0;
enum {MEM_SIZE = 4};
static uint32_t mem[MEM_SIZE] = {0};
static uint16_t byte_count = 0;
typedef struct iic_op {
    uint16_t write;
    uint16_t addr;
    uint32_t data;
//    uint8_t data[MAX_MSG_SIZE];
} iic_op;
osMailQDef(mailq_iic_rx, 1, iic_op);
osMailQId (mailq_iic_rx);

void task_vxsiic_init(void)
{
    led_all_set_state(LED_OFF);
    mailq_iic_rx = osMailCreate(osMailQ(mailq_iic_rx), NULL);

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

void task_vxsiic_run(void)
{
    if (!mailq_iic_rx)
        return;
    osEvent event = osMailGet(mailq_iic_rx, 1);
    if (osOK == event.status || osEventTimeout == event.status) {
        return;
    }
    if (osEventMail != event.status) {
        for(;;) {
            debug_print("error\n");
        }
    }
    iic_op *op = event.value.p;
//    debug_printf("%s %04X %08X\n", op->write ? "WRITE" : "READ ", op->addr, op->data);
    osMailFree(mailq_iic_rx, op);
}

static uint32_t mem_txdata = 0;

void Slave_Ready_To_Transmit_Callback(void)
{
    if (byte_count == 2) {
        if (mem_addr >= IIC_SENSORS_MAP_START && mem_addr < IIC_SENSORS_MAP_START + IIC_SENSORS_MAP_SIZE_BYTES / 4) {
            uint32_t offset = mem_addr - IIC_SENSORS_MAP_START;
            uint32_t *ptr = (uint32_t *)&ipmi_sensors + offset;
            memcpy(&mem_txdata, ptr, sizeof(mem_txdata));
        } else
        switch (mem_addr) {
        case 0:
            mem_txdata = BMC_MAGIC;
            break;
        case 1:
            mem_txdata = ((uint32_t)(VERSION_MAJOR_NUM) << 16) | (uint16_t)(VERSION_MINOR_NUM);
            break;
        case 2:
            mem_txdata = dev.fpga.id;
            break;
        case 3: {
            const SensorStatus systemStatus = getSystemStatus(&dev);
            mem_txdata = systemStatus;
            break;
        }
        case 8:
        case 9:
//            mem_txdata = mem[mem_addr];
            break;
        default:
            mem_txdata = 0;
            break;
        }
    }

    uint32_t tx_data = mem_txdata >> ((byte_count-2)*8);
    LL_I2C_TransmitData8(I2C1, tx_data);
}

void Error_Callback(void)
{
    led_toggle(LED_RED);
}

void Slave_Complete_Callback(void)
{
//    debug_printf("%s %d %04X %08X\n", mem_write ? "WRITE" : "READ ", byte_count, mem_addr, mem_write ? mem_rxdata : mem_txdata);
    if (mem_write) {
        if (mem_addr < MEM_SIZE) {
            mem[mem_addr] = mem_rxdata;
        }
    }

//    uint32_t data = byte_count; //rx_data;
    iic_op *op = osMailAlloc(mailq_iic_rx, osWaitForever);
    op->addr = mem_addr;
    op->write = mem_write;
    op->data = mem_write ? mem_rxdata : mem_txdata;
    osStatus status = osMailPut(mailq_iic_rx, op);
    if (osOK != status) {
        Error_Callback();
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
            Error_Callback();
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
        /* Call Error function */
        Error_Callback();
    }
}

void i2c_error_interrupt_handler(void)
{
    debug_printf("%lu, I2C ERROR\n", DWT->CYCCNT);
    Error_Callback();
}
