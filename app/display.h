#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "ansi_escape_codes.h"
#include "devices.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define ANSI_COL_RESULTS ANSI_COL40

#define STR_ON (ANSI_COL_RESULTS ANSI_GREEN "ON" ANSI_CLEAR)
#define STR_OFF (ANSI_COL_RESULTS ANSI_RED "OFF" ANSI_CLEAR)

#define STR_NORMAL (ANSI_COL_RESULTS ANSI_GREEN "NORMAL" ANSI_CLEAR)
#define STR_FAIL (ANSI_COL_RESULTS ANSI_RED "FAIL" ANSI_CLEAR)

void print_adt7301_value(uint16_t rawTemp);
void devPrintStatus(const Devices d);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
