/*
**    Interrupt and Exception Handlers
**
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

#include "bsp.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "main.h"
#include "stm32f7xx_it.h"
#ifndef BOARD_TTVXS
#include "vxsiics/dev_vxsiics.h"
#endif
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_usart.h"

extern TIM_HandleTypeDef htim1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;
extern I2C_HandleTypeDef hi2c4;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi5;
extern ETH_HandleTypeDef EthHandle;

extern void serial_console_interrupt_handler(USART_TypeDef *usart);

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/


void TIM1_UP_TIM10_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

void I2C1_EV_IRQHandler(void)
{
#ifndef BOARD_TTVXS
    i2c_event_interrupt_handler();
#else
    HAL_I2C_EV_IRQHandler(&hi2c1);
#endif
}

void I2C1_ER_IRQHandler(void)
{
#ifndef BOARD_TTVXS
    i2c_error_interrupt_handler();
#else
    HAL_I2C_ER_IRQHandler(&hi2c1);
#endif
}

void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c2);
}

void I2C3_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c3);
}

void I2C3_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c3);
}

void I2C4_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C4_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c4);
}

void USART1_IRQHandler(void)
{
    serial_console_interrupt_handler(USART1);
}

void USART2_IRQHandler(void)
{
    serial_console_interrupt_handler(USART2);
}

void USART3_IRQHandler(void)
{
    serial_console_interrupt_handler(USART3);
}

void ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&EthHandle);
}

void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

void SPI2_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi2);
}

void SPI3_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi3);
}

void SPI4_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi4);
}

void SPI5_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi5);
}
