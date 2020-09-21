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

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h"
#include "error_handler.h"
#include "stm32_hal.h"
#include "stm32_init_periph.h"

SPI_HandleTypeDef hspi1 = {0};
SPI_HandleTypeDef hspi2 = {0};
SPI_HandleTypeDef hspi3 = {0};
SPI_HandleTypeDef hspi4 = {0};
SPI_HandleTypeDef hspi5 = {0};

static void init_ad9516_spi(int index)
{
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#ifdef TTVXS_1_0
    // ADT7301
    hspi->Init.DataSize = SPI_DATASIZE_16BIT;
    hspi->Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
#else
    // AD9516-4
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
#endif
    if (HAL_SPI_Init(hspi) != HAL_OK) {
        Error_Handler();
    }
}

static void init_fpga_spi(int index)
{
    // FPGA
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.DataSize = SPI_DATASIZE_16BIT;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(hspi) != HAL_OK) {
        Error_Handler();
    }
}

static void SPI2_synchronize(void)
{
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

void init_spi_peripherals(void)
{
    init_ad9516_spi(SPI_BUS_INDEX_AD9516);
    init_fpga_spi(SPI_BUS_INDEX_FPGA);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (spiHandle->Instance==SPI2) {
        SPI2_synchronize();
        __HAL_RCC_SPI2_CLK_ENABLE();

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
