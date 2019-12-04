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

#ifndef DEV_LEDS_H
#define DEV_LEDS_H

#include <stdbool.h>

#include "dev_leds_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void dev_led_set(DeviceLeds led, bool state);
void dev_leds_toggle(DeviceLeds led);

#ifdef __cplusplus
}
#endif

#endif // DEV_LEDS_H
