/*
**    TDC72VHL SPI Init/Deinit
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

#include "spi.h"

#include "bsp_pin_defs.h"
#include "error_handler.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_spi.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi4;
SPI_HandleTypeDef hspi5;

// SPI1: FPGA
void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
}

// SPI4: ADT7301
void MX_SPI4_Init(void)
{
    hspi4.Instance = SPI4;
    hspi4.Init.Mode = SPI_MODE_MASTER;
    hspi4.Init.Direction = SPI_DIRECTION_2LINES;
    hspi4.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi4.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi4.Init.NSS = SPI_NSS_SOFT;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 7;
    hspi4.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi4.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#ifdef BOARD_TDC64
    // AD9516-4
    hspi4.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
#endif
#ifdef BOARD_TDC72
    // ADT7301
    hspi4.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
#endif
    if (HAL_SPI_Init(&hspi4) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (spiHandle->Instance==SPI1) {
        // FPGA
        // software NSS
        GPIO_InitStruct.Pin = FPGA_NSS_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, GPIO_PIN_RESET);
        HAL_GPIO_Init(FPGA_NSS_GPIO_Port, &GPIO_InitStruct);

        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = FPGA_MISO_Pin|FPGA_SCK_Pin|FPGA_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
    }
    else if(spiHandle->Instance==SPI4) {
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        GPIO_InitStruct.Pin = SPI4_DIN_Pin|SPI4_SCLK_Pin|SPI4_DOUT_Pin;
#ifdef BOARD_TDC64
        GPIO_InitStruct.Pin |= AD9516_CS_Pin;
#endif
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI4_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI4_IRQn);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
    if (spiHandle->Instance==SPI1) {
        __HAL_RCC_SPI1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, FPGA_NSS_Pin|FPGA_MISO_Pin|FPGA_SCK_Pin|FPGA_MOSI_Pin);
        HAL_NVIC_DisableIRQ(SPI1_IRQn);
    }
    else if (spiHandle->Instance==SPI4) {
        __HAL_RCC_SPI4_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOE, SPI4_DIN_Pin|SPI4_SCLK_Pin|SPI4_DOUT_Pin);
        HAL_NVIC_DisableIRQ(SPI4_IRQn);
    }
}
