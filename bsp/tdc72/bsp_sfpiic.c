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

#include "bsp_sfpiic.h"
#include "stm32f7xx_hal_gpio.h"
#include "bsp_pin_defs.h"

void bsp_sfpiic_reset(void)
{
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin,  GPIO_PIN_SET);
}
