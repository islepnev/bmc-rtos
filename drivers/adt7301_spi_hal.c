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
#include "bsp.h"

static const int SPI_TIMEOUT_MS = 10;

/**
  * @brief  Converts raw temperature value
  * @param  raw: 14-bit raw value from ADT7301 temperature sensor
  * @retval temperature in degrees Celsius multiplied by 32
  */
int16_t adt7301_convert_temp_adt7301_scale32(int16_t raw)
{
    return (int16_t)(raw << 2) >> 2;
}

/**
  * @brief Read the temperature data from the specified sensor
  * @param source: sensor index [0..3]
  */
HAL_StatusTypeDef adt7301_read_temp(int source, int16_t *data)
{
    uint16_t SPI_transmit_buffer = {0};
    uint16_t SPI_receive_buffer = {0};

    GPIO_TypeDef * port;
    uint16_t cs_pin;

    switch (source)
    {
    default:
        port = ADT_CS_B_GPIO_Port;
        cs_pin = ADT_CS_B_Pin;
        break;
    }

    HAL_GPIO_WritePin(port, cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(therm_spi, (uint8_t *)&SPI_transmit_buffer, (uint8_t *)&SPI_receive_buffer, 1, SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(port, cs_pin, GPIO_PIN_SET);
    if (data) {
        if (ret == HAL_OK) {
            uint16_t result = SPI_receive_buffer;
            *data = result;
        } else {
            *data = TEMP_RAW_ERROR;
        }
    }
    return ret;
}

