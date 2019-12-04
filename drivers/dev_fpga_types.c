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

#include "dev_fpga_types.h"

SensorStatus get_fpga_sensor_status(const Dev_fpga *d)
{
   if (!d->present)
      return SENSOR_UNKNOWN;
   if (!d->initb)
      return SENSOR_CRITICAL;
   if (!d->done)
      return SENSOR_CRITICAL;
   if (d->id == 0 || d->id == 0xFFFFu)
      return SENSOR_WARNING;
   return SENSOR_NORMAL;
}
