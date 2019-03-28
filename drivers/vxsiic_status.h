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
#ifndef VXSIIC_STATUS_H
#define VXSIIC_STATUS_H

enum {VXSIIC_SLOTS = 18};

struct vxsiic_slot_status_t {
   int present;
};

typedef struct vxsiic_slot_status_t vxsiic_slot_status_t;

struct vxsiic_status_t {
   vxsiic_slot_status_t slot[VXSIIC_SLOTS];
};

typedef struct vxsiic_status_t vxsiic_status_t;

#endif // VXSIIC_STATUS_H
