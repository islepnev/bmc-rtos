/*
**    Copyright 2019-2020 Ilja Slepnev
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

#ifndef DISPLAY_BRIEF_H
#define DISPLAY_BRIEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_AUXPLL_DETAIL_H 3

void print_pm_pots(void);
void print_footer(void);
void print_system_status(int y);
void display_pll_detail(int y);
void display_auxpll_detail(int y);
void print_powermon(int y);
void print_pll(int y);
void print_auxpll(int y);
void print_thset(int y);
void print_fpga(int y);
void print_ttvxs_clkmux(int y);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_BRIEF_H
