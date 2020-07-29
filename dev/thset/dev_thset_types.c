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

#include "dev_thset_types.h"

#include "devicelist.h"

const Dev_thset_priv *get_thset_priv_const(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_THSET);
    if (!d || !d->priv)
        return 0;
    return (const Dev_thset_priv *)device_priv_const(d);
}
