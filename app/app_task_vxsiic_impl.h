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

#ifndef APP_TASK_VXSIIC_IMPL_H
#define APP_TASK_VXSIIC_IMPL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vxsiic_i2c_stats_t {
    uint32_t ops;
    uint32_t errors;
} vxsiic_i2c_stats_t;

const struct vxsiic_i2c_stats_t *get_vxsiic_i2c_stats_ptr(void);

void task_vxsiic_init(void);
void task_vxsiic_run(void);
void i2c_event_interrupt_handler(void);
void i2c_error_interrupt_handler(void);

#ifdef __cplusplus
}
#endif

#endif // APP_TASK_VXSIIC_IMPL_H
