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

#ifndef AD9516_STATUS_H
#define AD9516_STATUS_H

#include <stdint.h>

typedef union
{
  struct
  {
      uint8_t dlock:1;
      uint8_t ref1_over_thr:1;
      uint8_t ref2_over_thr:1;
      uint8_t vco_over_thr:1;
      uint8_t ref2_selected:1;
      uint8_t holdover:1;
      uint8_t vco_cal_finished:1;
      uint8_t reserved:1;
  } b;
  uint8_t raw;
} AUXPLL_Readback_REG_Type;

enum { AUXREF_INPUT_COUNT = 2};

typedef struct {
    AUXPLL_Readback_REG_Type pll_readback;
} AD9516_Status;

#endif // AD9516_STATUS_H
