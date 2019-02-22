//
//    Copyright 2017-2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "adt7301_spi_hal.h"
#include "main.h"
#include "spi.h"

static const int SPI_TIMEOUT_MS = 10;

/**
  * @brief  Converts raw temperature value
  * @param  raw: 14-bit raw value from ADT7301 temperature sensor
  * @retval temperature in degrees Celsius multiplied by 32
  */
int16_t adt7301_convert_temp_adt7301_scale32(uint16_t raw)
{
    return (int16_t)(raw << 2) >> 2;
}

/**
  * @brief Read the temperature data from the specified sensor
  * @param source: sensor index [0..3]
  */
uint16_t adt7301_read_temp(int source)
{
    uint8_t SPI_transmit_buffer[2] = {0, 0};
    uint8_t SPI_receive_buffer[2] = {0, 0};

    GPIO_TypeDef * port;
    uint16_t cs_pin;
//	int16_t * result;

    switch (source)
    {
    case 0:
        port = ADT_CS_B0_GPIO_Port;
        cs_pin = ADT_CS_B0_Pin;
//		result = &Temp.temp_b0;
        break;
    case 1:
        port = ADT_CS_B1_GPIO_Port;
        cs_pin = ADT_CS_B1_Pin;
//		result = &Temp.temp_b1;
        break;
    case 2:
        port = ADT_CS_B2_GPIO_Port;
        cs_pin = ADT_CS_B2_Pin;
//		result = &Temp.temp_b2;
        break;
    case 3:
        port = ADT_CS_B3_GPIO_Port;
        cs_pin = ADT_CS_B3_Pin;
//		result = &Temp.temp_b3;
        break;
    default:
        return TEMP_RAW_ERROR;
    }

    HAL_GPIO_WritePin(port, cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef spi_ret = HAL_SPI_TransmitReceive(therm_spi, SPI_transmit_buffer, SPI_receive_buffer, 1, SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(port, cs_pin, GPIO_PIN_SET);
    if (spi_ret != HAL_OK) {
//        *result = 0;
        return TEMP_RAW_ERROR;
    }
    uint16_t result = ((uint16_t)SPI_receive_buffer[1] << 8) | SPI_receive_buffer[0];
    return result;
}

