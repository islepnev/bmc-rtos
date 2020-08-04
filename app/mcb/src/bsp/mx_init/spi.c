/*
**    SPI Init/Deinit
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
#include "stm32f7xx_hal_def.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_spi.h"

SPI_HandleTypeDef hspi1 = {0};
SPI_HandleTypeDef hspi2 = {0};
SPI_HandleTypeDef hspi3 = {0};
SPI_HandleTypeDef hspi4 = {0};
SPI_HandleTypeDef hspi5 = {0};

static void MX_SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 7;
    hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#ifdef TTVXS_1_0
    // ADT7301
    hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
#else
    // AD9516-4
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
#endif
    if (HAL_SPI_Init(&hspi2) != HAL_OK) {
        Error_Handler();
    }

}

// PI3     ------> SPI2_MOSI
// PI2     ------> SPI2_MISO
// PI1     ------> SPI2_SCK
// PI0     ------> SPI2_NSS

// PF7     ------> SPI5_SCK
// PF9     ------> SPI5_MOSI
// PF8     ------> SPI5_MISO

static void MX_SPI5_Init(void)
{
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi5.Init.NSS = SPI_NSS_SOFT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 7;
    hspi5.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(&hspi5) != HAL_OK) {
        Error_Handler();
    }
}

static void SPI2_synchronize(void)
{
    __HAL_RCC_GPIOI_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // force NSS high
    GPIO_InitStruct.Pin = SPI2_NSS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(SPI2_GPIO_Port, &GPIO_InitStruct);

    // toggle SCLK to reset SPI interface on device
    GPIO_InitStruct.Pin = SPI2_SCLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_SCLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(SPI2_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET); // CS# assert
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_SCLK_Pin, GPIO_PIN_SET);   // SCLK up
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_SCLK_Pin, GPIO_PIN_RESET); // SCLK down
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_SCLK_Pin, GPIO_PIN_SET);   // SCLK up
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_SCLK_Pin, GPIO_PIN_RESET); // SCLK down
    HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);   // CS# deassert
}

void MX_SPI_Init(void)
{
    MX_SPI2_Init();
    MX_SPI5_Init();
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (spiHandle->Instance==SPI2) {
        SPI2_synchronize();
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_GPIOI_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
    }
    else if (spiHandle->Instance==SPI5) {
        GPIO_InitStruct.Pin = FPGA_NSS_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(FPGA_NSS_GPIO_Port, &GPIO_InitStruct);

        __HAL_RCC_SPI5_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();
        GPIO_InitStruct.Pin = FPGA_SCLK_Pin|FPGA_MOSI_Pin|FPGA_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI5_IRQn);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
    if (spiHandle->Instance==SPI2) {
        __HAL_RCC_SPI2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOI, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
        HAL_NVIC_DisableIRQ(SPI2_IRQn);
    }
    else if (spiHandle->Instance==SPI5) {
        __HAL_RCC_SPI5_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOF, FPGA_NSS_Pin|FPGA_SCLK_Pin|FPGA_MOSI_Pin|FPGA_MISO_Pin);
        HAL_NVIC_DisableIRQ(SPI5_IRQn);
    }
}
