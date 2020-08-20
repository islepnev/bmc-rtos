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
    // hspi4.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
#endif
#if defined (BOARD_TDC72) || defined (BOARD_TDC72VHLV3)
    // ADT7301
    hspi4.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
#endif
    if (HAL_SPI_Init(&hspi4) != HAL_OK) {
        Error_Handler();
    }
}

// SPI5: PLL AD9548
void MX_SPI5_Init(void)
{

    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi5.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 7;
    hspi5.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi5.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi5) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
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
    else if (spiHandle->Instance==SPI4) {
        // ADT7301
        // SPI4_synchronize();
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        GPIO_InitStruct.Pin = ADT_SCLK_Pin|ADT_DOUT_Pin|ADT_DIN_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI4_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI4_IRQn);
    }
    else if (spiHandle->Instance==SPI5) {
        // PLL AD9548
        __HAL_RCC_SPI5_CLK_ENABLE();
        GPIO_InitStruct.Pin = PLL_SPI_SCLK_Pin|PLL_SPI_NSS_Pin|PLL_SPI_MOSI_Pin|PLL_SPI_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

    if (spiHandle->Instance==SPI1) {
        __HAL_RCC_SPI1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, FPGA_MISO_Pin|FPGA_SCK_Pin|FPGA_MOSI_Pin);
    }
    else if(spiHandle->Instance==SPI4) {
        __HAL_RCC_SPI4_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOE, ADT_SCLK_Pin|ADT_DOUT_Pin|ADT_DIN_Pin);
    }
    else if(spiHandle->Instance==SPI5) {
        __HAL_RCC_SPI5_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOF, PLL_SPI_SCLK_Pin|PLL_SPI_NSS_Pin|PLL_SPI_MOSI_Pin|PLL_SPI_MISO_Pin);
    }
} 

//SPI_HandleTypeDef * const pll_spi = &hspi5;
//SPI_HandleTypeDef * const fpga_spi = &hspi1;
//SPI_HandleTypeDef * const therm_spi = &hspi4;
