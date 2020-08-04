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

#ifndef MCP23017_I2C_HAL_H
#define MCP23017_I2C_HAL_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MCP23017_IODIRA = 0,
    MCP23017_IODIRB = 1,
    MCP23017_IPOLA = 2,
    MCP23017_IPOLB = 3,
    MCP23017_GPINTENA = 4,
    MCP23017_GPINTENB = 5,
    MCP23017_GPPUA = 0xC,
    MCP23017_GPPUB = 0xD,
    MCP23017_GPIOA = 0x12,
    MCP23017_GPIOB = 0x13,
    MCP23017_OLATA = 0x14,
    MCP23017_OLATB = 0x15
} mcp23017_regs_bank_0;

bool mcp23017_detect(void);
bool mcp23017_read(uint8_t reg, uint8_t *data);
bool mcp23017_write(uint8_t reg, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // MCP23017_I2C_HAL_H
