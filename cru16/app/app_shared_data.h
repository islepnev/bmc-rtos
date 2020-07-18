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

#ifndef APP_SHARED_DATA_H
#define APP_SHARED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISPLAY_SUMMARY,
    DISPLAY_LOG,
    DISPLAY_BOARDS,
    DISPLAY_PLL_DETAIL,
    DISPLAY_SFP_DETAIL,
    DISPLAY_TASKS,
    DISPLAY_NONE,
} display_mode_t;

//extern Devices dev;
extern int enable_pll_run;
extern display_mode_t display_mode;
extern int enable_power;
extern int enable_stats_display;

struct Devices;
struct Dev_powermon;
struct Dev_fpga;
struct Dev_clkmux;
struct Dev_pll;
struct Dev_auxpll;
struct Dev_thset;
struct Dev_vxsiic;
struct Dev_sfpiic;
struct Dev_at24c;

int get_mcb_pcb_ver(void);
struct Devices* getDevices(void);
const struct Devices* getDevicesConst(void);

struct Dev_powermon* get_dev_powermon(void);
const struct Dev_powermon* get_dev_powermon_const(void);
struct Dev_fpga *get_dev_fpga(void);
struct Dev_clkmux *get_dev_clkmux(void);
struct Dev_pll *get_dev_pll(void);
struct Dev_auxpll *get_dev_auxpll(void);
struct Dev_thset *get_dev_thset(void);
struct Dev_vxsiic *get_dev_vxsiic(void);
struct Dev_sfpiic *get_dev_sfpiic(void);
struct Dev_at24c *get_dev_eeprom_config(void);

#ifdef __cplusplus
}
#endif

#endif // APP_SHARED_DATA_H
