/*
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
#ifndef DEV_LEDS_TYPES_H
#define DEV_LEDS_TYPES_H

typedef enum {
    LED_RED = 1,
    LED_YELLOW = 2,
    LED_GREEN = 3,
    LED_INT_RED = 4,
    LED_INT_GREEN = 5
} DeviceLeds;

typedef enum {
    LED_OFF = 0,
    LED_ON = 1
} LedState;

#endif // DEV_LEDS_TYPES_H
