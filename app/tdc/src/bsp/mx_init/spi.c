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

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi4;
SPI_HandleTypeDef hspi5;

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

#ifdef ENABLE_AD9516
static void init_ad9516_spi(int index)
{
    SPI_HandleTypeDef *hspi = hspi_handle(index);
    hspi->Instance = spi_instance(index);
    hspi->Init.Mode = SPI_MODE_MASTER;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi->Init.NSS = SPI_NSS_SOFT; // SPI_NSS_HARD_OUTPUT
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    if (HAL_SPI_Init(hspi) != HAL_OK) {
        Error_Handler();
    }
}
#endif

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
    init_fpga_spi(SPI_BUS_INDEX_FPGA);
#if defined (BOARD_TDC72) || defined (BOARD_TDC72VHLV3)
    init_adt7301_spi(SPI_BUS_INDEX_ADT7301);
#endif
#ifdef BOARD_TDC64
    init_ad9516_spi(SPI_BUS_INDEX_AD9516);
#endif
#ifdef BOARD_TDC72VHLV3
    init_ad9548_spi(SPI_BUS_INDEX_AD9548);
#endif
}

#if 0
static void SPI4_synchronize(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // force NSS high
    GPIO_InitStruct.Pin = SPI4_NSS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_NSS_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(SPI4_GPIO_Port, &GPIO_InitStruct);

    // toggle SCLK to reset SPI interface on device
    GPIO_InitStruct.Pin = SPI4_SCLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_SCLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(SPI4_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_NSS_Pin, GPIO_PIN_RESET); // CS# assert
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_SCLK_Pin, GPIO_PIN_SET);   // SCLK up
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_SCLK_Pin, GPIO_PIN_RESET); // SCLK down
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_SCLK_Pin, GPIO_PIN_SET);   // SCLK up
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_SCLK_Pin, GPIO_PIN_RESET); // SCLK down
    HAL_GPIO_WritePin(SPI4_GPIO_Port, SPI4_NSS_Pin, GPIO_PIN_SET);   // CS# deassert
}
#endif

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

//#ifdef BOARD_TDC72VHLV3
//        GPIO_InitStruct.Pin = ADT_SCLK_Pin|ADT_DOUT_Pin|ADT_DIN_Pin;
//#else
        GPIO_InitStruct.Pin = SPI4_DIN_Pin|SPI4_SCLK_Pin|SPI4_DOUT_Pin;
//#endif
//#ifdef BOARD_TDC64
//        GPIO_InitStruct.Pin |= AD9516_CS_Pin;
//#endif
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
#if ENABLE_AD9548
        __HAL_RCC_SPI5_CLK_ENABLE();
        GPIO_InitStruct.Pin = AD9548_SPI_NSS_Pin|AD9548_SPI_SCLK_Pin|AD9548_SPI_MOSI_Pin|AD9548_SPI_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(SPI5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI5_IRQn);
#endif
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
//#ifdef BOARD_TDC64
////        HAL_GPIO_DeInit(GPIOE, AD9516_CS_Pin);
//        GPIO_InitTypeDef GPIO_InitStruct = {0};
//        GPIO_InitStruct.Pin = AD9516_CS_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//        GPIO_InitStruct.Pull = GPIO_PULLUP;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//#endif
//#ifdef BOARD_TDC72VHLV3
//        HAL_GPIO_DeInit(GPIOE, ADT_SCLK_Pin|ADT_DOUT_Pin|ADT_DIN_Pin);
//#else
        HAL_GPIO_DeInit(GPIOE, SPI4_DIN_Pin|SPI4_SCLK_Pin|SPI4_DOUT_Pin);
//#endif
        HAL_NVIC_DisableIRQ(SPI4_IRQn);
    }
    else if(spiHandle->Instance==SPI5) {
#if ENABLE_AD9548
        __HAL_RCC_SPI5_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOF, AD9548_SPI_NSS_Pin|AD9548_SPI_SCLK_Pin|AD9548_SPI_MOSI_Pin|AD9548_SPI_MISO_Pin);
#endif
    }
}
