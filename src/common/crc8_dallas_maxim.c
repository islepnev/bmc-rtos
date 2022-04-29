
#include "crc8_dallas_maxim.h"

unsigned char crc8_dallas_maxim(unsigned char val, unsigned char crc)
{
  unsigned char i = 8;

  crc ^= val;
  do {
    if (crc & 0x01) {
      crc >>= 1;
      crc ^= 0x8C;
    } else {
      crc >>= 1;
    }
  } while (--i);

  return crc;
}

