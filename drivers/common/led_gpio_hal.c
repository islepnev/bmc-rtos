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

#include "bsp_pin_defs.h"
#include "gpio.h"

void led_all_set_state(bool state)
{
    led_set_state(LED_RED,       state);
    led_set_state(LED_YELLOW,    state);
    led_set_state(LED_GREEN,     state);
    led_set_state(LED_INT_RED,   state);
    led_set_state(LED_INT_GREEN, state);
}

void led_set_state(DeviceLeds led, bool state)
{
    switch(led) {
    case LED_RED :
        write_gpio_pin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    !state);
        break;
    case LED_YELLOW :
        write_gpio_pin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, !state);
        break;
    case LED_GREEN :
        write_gpio_pin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  !state);
        break;
#ifdef LED_ERROR_B_Pin
    case LED_INT_RED :
        write_gpio_pin(LED_ERROR_B_GPIO_Port, LED_ERROR_B_Pin, !state);
        break;
#endif
#ifdef LED_HEARTBEAT_B_Pin
    case LED_INT_GREEN :
        write_gpio_pin(LED_HEARTBEAT_B_GPIO_Port, LED_HEARTBEAT_B_Pin, !state);
        break;
#endif
    default:
        break;
    }
}

void led_toggle(DeviceLeds led)
{
    switch(led) {
    case LED_RED :
        HAL_GPIO_TogglePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin);
        break;
    case LED_YELLOW :
        HAL_GPIO_TogglePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin);
        break;
    case LED_GREEN :
        HAL_GPIO_TogglePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin);
        break;
#ifdef LED_ERROR_B_Pin
    case LED_INT_RED :
        HAL_GPIO_TogglePin(LED_ERROR_B_GPIO_Port, LED_ERROR_B_Pin);
        break;
#endif
#ifdef LED_HEARTBEAT_B_Pin
    case LED_INT_GREEN :
        HAL_GPIO_TogglePin(LED_HEARTBEAT_B_GPIO_Port, LED_HEARTBEAT_B_Pin);
        break;
#endif
    default:
        break;
    }
}
