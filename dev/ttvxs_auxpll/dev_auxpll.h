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

#ifndef DEV_AUXPLL_H
#define DEV_AUXPLL_H

#include <stdbool.h>

#include "dev_common_types.h"
#include "dev_auxpll_types.h"
#include "ad9516/ad9516_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

DeviceStatus auxpllDetect(Dev_auxpll *d);
OpStatusTypeDef auxpllSetup(Dev_auxpll *d);

//void reset_SPI_auxpll(void);
bool auxpllSoftwareReset(void);
//OpStatusTypeDef auxpllSetupSysclk(Dev_auxpll *d);
//OpStatusTypeDef auxpllCalibrateSysclk(Dev_auxpll *d);
OpStatusTypeDef auxpllReadStatus(Dev_auxpll *d);
//OpStatusTypeDef auxpllReadSysclkStatus(Dev_auxpll *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_AUXPLL_H
