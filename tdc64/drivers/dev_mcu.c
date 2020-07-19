//
//    Copyright 2019 Ilja Slepnev
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

#include "dev_mcu.h"
#include "stm32f7xx_hal.h"

uint32_t getTickFreqHz(void)
{
    switch (HAL_GetTickFreq()) {
    case HAL_TICK_FREQ_10HZ : return 10;
    case HAL_TICK_FREQ_100HZ: return 100;
    case HAL_TICK_FREQ_1KHZ: return 1000;
    };
    return 1000;
}
