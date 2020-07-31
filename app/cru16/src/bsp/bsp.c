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

// CRU16 board specific functions

#include "bsp.h"

#include <stdbool.h>

#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_ll_gpio.h"
#include "gpio.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "spi.h"
#include "usart.h"
#include "error_handler.h"

uint32_t detect_pcb_version(void)
{
    bool a0 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A0_GPIO_Port, PCB_VER_A0_Pin));
    bool a1 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A1_GPIO_Port, PCB_VER_A1_Pin));
    return a1 * 2 + a0;
}

void update_board_version(int powermon_count)
{
    // do nothing
}

/*
void fpga_enable_interface(bool enable)
{
    GPIO_PinState low = GPIO_PIN_RESET;
    GPIO_PinState high = enable ? GPIO_PIN_SET : GPIO_PIN_RESET;

    HAL_GPIO_WritePin(FPGA_RX_GPIO_Port, FPGA_RX_Pin, high);
    HAL_GPIO_WritePin(FPGA_TX_GPIO_Port, FPGA_TX_Pin, high);

    if (enable) {
        __HAL_UART_ENABLE(&huart6);
    } else {
        __HAL_UART_DISABLE(&huart6);
//        FPGA_RX_GPIO_Port->ODR &= ~FPGA_RX_Pin;
//        FPGA_TX_GPIO_Port->ODR &= ~FPGA_TX_Pin;
    }
}
*/

bool fpga_done_pin_present(void)
{
    return true;
}

void sfpiic_switch_enable(bool enable)
{
    write_gpio_pin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  !enable);
}
