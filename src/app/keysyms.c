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

#include "keysyms.h"

const char *ESC_CODE_LEFT = "[D";
const char *ESC_CODE_RIGHT = "[C";
const char *ESC_CODE_UP = "[A";
const char *ESC_CODE_DOWN = "[B";

const char *ESC_CODE_F1 = "OP";
const char *ESC_CODE_F2 = "OQ";
const char *ESC_CODE_F3 = "OR";
const char *ESC_CODE_F4 = "OS";
const char *ESC_CODE_F5 = "[15~";
const char *ESC_CODE_F6 = "[17~";
const char *ESC_CODE_F7 = "[18~";
const char *ESC_CODE_F8 = "[19~";
const char *ESC_CODE_F9 = "[20~";
const char *ESC_CODE_F10 = "[21~";
const char *ESC_CODE_F11 = "[23~";
const char *ESC_CODE_F12 = "[24~";

const char ASCII_LF = '\x0A';
const char ASCII_CR = '\x0D';
const char ASCII_ESC = '\x1B';
const char ASCII_DEL = '\x7F';
