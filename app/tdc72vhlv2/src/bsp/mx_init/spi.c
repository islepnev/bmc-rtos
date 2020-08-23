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

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h"
#include "error_handler.h"
#include "stm32_hal.h"
#include "stm32_init_periph.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi4;
SPI_HandleTypeDef hspi5;

//static SPI_TypeDef *ad9548_spi_instance  = SPI1;
//static SPI_TypeDef *fpga_spi_instance    = SPI2;
//static SPI_TypeDef *adt7301_spi_instance = SPI3;

static void init_fpga_spi(int index)
{
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.DataSize = SPI_DATASIZE_16BIT;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
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

static void init_adt7301_spi(int index)
{
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi->Init.DataSize = SPI_DATASIZE_16BIT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    if (HAL_SPI_Init(hspi) != HAL_OK) {
        Error_Handler();
    }
}

static void init_ad9548_spi(int index)
{
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(hspi) != HAL_OK) {
        Error_Handler();
    }
}

void init_spi_peripherals(void)
{
    spi_enable_clock();
    init_ad9548_spi(SPI_BUS_INDEX_AD9548);
    init_fpga_spi(SPI_BUS_INDEX_FPGA);
    init_adt7301_spi(SPI_BUS_INDEX_ADT7301);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (spiHandle->Instance==SPI1)
    {
        __HAL_RCC_SPI1_CLK_ENABLE();

        GPIO_InitStruct.Pin = PLL_CS_Pin|PLL_SCLK_Pin|PLL_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
    }
    else if (spiHandle->Instance==SPI2)
    {
        __HAL_RCC_SPI2_CLK_ENABLE();

        GPIO_InitStruct.Pin = FPGA_SCLK_Pin|FPGA_MISO_Pin|FPGA_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
    }
    else if (spiHandle->Instance==SPI3)
    {
        __HAL_RCC_SPI3_CLK_ENABLE();

        GPIO_InitStruct.Pin = ADT_SCLK_Pin|ADT_DIN_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ADT_DOUT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(ADT_DOUT_GPIO_Port, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI3_IRQn);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

    if (spiHandle->Instance==SPI1)
    {
        __HAL_RCC_SPI1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, PLL_CS_Pin|PLL_SCLK_Pin|PLL_MOSI_Pin);
        HAL_NVIC_DisableIRQ(SPI1_IRQn);
    }
    else if (spiHandle->Instance==SPI2)
    {
        __HAL_RCC_SPI2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, FPGA_SCLK_Pin|FPGA_MISO_Pin|FPGA_MOSI_Pin);
        HAL_NVIC_DisableIRQ(SPI2_IRQn);
    }
    else if (spiHandle->Instance==SPI3)
    {
        __HAL_RCC_SPI3_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, ADT_SCLK_Pin|ADT_DOUT_Pin|ADT_DIN_Pin);
        HAL_NVIC_DisableIRQ(SPI3_IRQn);
    }
}
