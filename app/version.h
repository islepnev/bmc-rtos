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

#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
 extern "C" {
#endif

#define APP_NAME_STR "TDC72VXS4 BMC"
#define VERSION_MAJOR_NUM 1
#define VERSION_MINOR_NUM 2
#define xstr(s) str(s)
#define str(s) #s

//#define VERSION_STR "1.2"

#define VERSION_STR xstr(VERSION_MAJOR_NUM)"."xstr(VERSION_MINOR_NUM)

#ifdef __cplusplus
}
#endif

#endif // VERSION_H
