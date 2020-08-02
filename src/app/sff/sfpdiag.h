#ifndef SFPDIAG_H

#define SFPDIAG_H

#include "sff-common.h"

void sff8472_parse_eeprom(const __u8 *id, struct sff_diags *sd);

#endif // SFPDIAG_H
