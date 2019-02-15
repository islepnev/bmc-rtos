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

#include "dev_leds.h"
#include "stm32f7xx_hal.h"
#include "gpio.h"

void struct_dev_leds_init(Dev_Leds *d)
{
    d->led_red = LED_OFF;
    d->led_yellow = LED_OFF;
    d->led_green = LED_OFF;
}

void dev_led_set(Dev_Leds *d, DeviceLeds led, LedState state)
{
    switch(led) {
    case LED_RED :
        d->led_red = state;
        HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    case LED_YELLOW :
        d->led_yellow = state;
        HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    case LED_GREEN :
        d->led_green = state;
        HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    }
}

void dev_leds_toggle(Dev_Leds *d, DeviceLeds led)
{
    switch(led) {
    case LED_RED :
        d->led_red = ! d->led_red;
        dev_led_set(d, led, d->led_red);
        break;
    case LED_YELLOW :
        d->led_yellow = ! d->led_yellow;
        dev_led_set(d, led, d->led_yellow);
        break;
    case LED_GREEN :
        d->led_green = ! d->led_green;
        dev_led_set(d, led, d->led_green);
        break;
    }
}
