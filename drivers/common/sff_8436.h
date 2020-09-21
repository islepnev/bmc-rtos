/*
**    SFF-8436 definitions (partial)
**
**    Copyright 2020 Ilja Slepnev
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

#ifndef SFF_8436_H
#define SFF_8436_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SFF_8436_MON_TEMP_REG2 = 22, // module temperature, 2 bytes
    SFF_8436_MON_VOLT_REG2 = 26, // module supply voltage, 2 bytes
    SFF_8436_CH_MON_BASE_REG = 34,
    SFF_8436_CH_1_RX_POW_REG2 = 34,
    SFF_8436_CH_1_TX_POW_REG2 = 42,

    // Control Bytes (86-99)
    SFF_8436_TX_DISABLE_REG = 86,

    // Serial ID: Data Fields (128-190)
    SFF_8436_VENDOR_NAME_REG16 = 148, // 148-163, QSFP+ vendor name(ASCII)
    SFF_8436_CC_BASE = 191, // Check code for base ID fields (addresses 128-190)

    // extended ID fields (192-222)
    SFF_8436_VENDOR_SN_REG16 = 196, // 196-211, Serial number provided by vendor (ASCII)

    // Vendor Specific ID Fields (224-255)
    SFF_8436_VENDOR_SPECIFIC_EEPROM_REG32 = 224 // Vendor Specific EEPROM, 32 bytes
};

#ifdef __cplusplus
}
#endif

#endif // SFF_8436_H
