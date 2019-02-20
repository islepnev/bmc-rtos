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

#include "led_gpio_hal.h"
#include "stm32f7xx_hal_gpio.h"
#include "main.h"

void led_set_state(DeviceLeds led, LedState state)
{
    switch(led) {
    case LED_RED :
        HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    case LED_YELLOW :
        HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    case LED_GREEN :
        HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    }
}
