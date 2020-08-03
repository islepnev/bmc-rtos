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
#ifndef DEV_SFPIIC_PRINT_H
#define DEV_SFPIIC_PRINT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sfpPrintStatus(void);
void dev_sfpiic_print(void);

const char *dev_sfpiic_transceiver_str(uint8_t value);
const char *dev_sfpiic_connector_str(uint8_t value);
struct sfpiic_ch_status_t;
const char *ethernet_compliance_str(const struct sfpiic_ch_status_t *status);

#ifdef __cplusplus
}
#endif

#endif // DEV_SFPIIC_PRINT_H
