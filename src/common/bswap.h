/*
**    Copyright 2020 Ilia Slepnev
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

#ifndef BSWAP_H
#define BSWAP_H

//#include <endian.h>
//#include <machine/endian.h>
#include <sys/cdefs.h>

#if (defined __BYTE_ORDER__) && (_BYTE_ORDER == _LITTLE_ENDIAN)
#else
#error
#endif

#ifdef __GNUC__
#define	__bswap16(_x)	__builtin_bswap16(_x)
#define	__bswap32(_x)	__builtin_bswap32(_x)
#define	__bswap64(_x)	__builtin_bswap64(_x)
#else /* __GNUC__ */
#error
#endif

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define ntohs(x) __builtin_bswap16(x)
#define htons(x) __builtin_bswap16(x)
#define ntohl(x) __builtin_bswap32(x)
#define htonl(x) __builtin_bswap32(x)
#define ntohll(x) __builtin_bswap64(x)
#define htonll(x) __builtin_bswap64(x)
//#define ntohs(x) __bswap_16(x)
//#define htons(x) __bswap_16(x)
//#define ntohl(x) __bswap_32(x)
//#define htonl(x) __bswap_32(x)
//#define ntohll(x) __bswap_64(x)
//#define htonll(x) __bswap_64(x)
#else
#error
#endif

#endif // BSWAP_H
