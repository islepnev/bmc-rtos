/*
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

#ifndef AD9548_REGS_REFIN_H
#define AD9548_REGS_REFIN_H

#include <stdint.h>

enum { PLL_REFIN_SIZE = 8};

#pragma pack(push, 1)

typedef union {
    struct {
        uint8_t a:1;
        uint8_t aa:1;
        uint8_t b:1;
        uint8_t bb:1;
        uint8_t c:1;
        uint8_t cc:1;
        uint8_t d:1;
        uint8_t dd:1;
    } b;
    uint8_t raw;
} AD9548_RefIn_Powerdown_REG8_Type;

typedef union {
    struct {
        uint8_t a:2;
        uint8_t aa:2;
        uint8_t b:2;
        uint8_t bb:2;
        uint8_t c:2;
        uint8_t cc:2;
        uint8_t d:2;
        uint8_t dd:2;
    } b;
    uint16_t raw;
} AD9548_RefIn_Logic_Family_REG16_Type;

typedef union {
    struct {
        uint8_t a:3;
        uint8_t en_a:1;
        uint8_t aa:3;
        uint8_t en_aa:1;
        uint8_t b:3;
        uint8_t en_b:1;
        uint8_t bb:3;
        uint8_t en_bb:1;
        uint8_t c:3;
        uint8_t en_c:1;
        uint8_t cc:3;
        uint8_t en_cc:1;
        uint8_t d:3;
        uint8_t en_d:1;
        uint8_t dd:3;
        uint8_t en_dd:1;
    } b;
    uint32_t raw;
} AD9548_RefIn_Manual_Select_REG32_Type;


typedef union {
    struct {
        uint8_t priority:3; // Phase master threshold priority
        uint8_t unused:5;
    } b;
    uint8_t raw;
} AD9548_RefIn_Phase_Buildout_Switching_REG8_Type;

typedef union {
    struct {
        AD9548_RefIn_Powerdown_REG8_Type powerdown;
        AD9548_RefIn_Logic_Family_REG16_Type logic_family;
        AD9548_RefIn_Manual_Select_REG32_Type manual_select;
        AD9548_RefIn_Phase_Buildout_Switching_REG8_Type phase_buildout_switching;
    } b;
    uint8_t v[PLL_REFIN_SIZE];
} AD9548_RefIn_TypeDef;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern const AD9548_RefIn_TypeDef AD9548_RefIn_Default;

#ifdef __cplusplus
}
#endif

#endif // AD9548_REGS_REFIN_H
