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

#ifndef I2C_HAL_HANDLES_H
#define I2C_HAL_HANDLES_H

struct __I2C_HandleTypeDef;

extern struct __I2C_HandleTypeDef hi2c1;
extern struct __I2C_HandleTypeDef hi2c2;
extern struct __I2C_HandleTypeDef hi2c3;
extern struct __I2C_HandleTypeDef hi2c4;

#endif // I2C_HAL_HANDLES_H
