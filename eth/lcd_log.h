
#ifndef  __LCD_LOG_H__
#define  __LCD_LOG_H__

#include "logbuffer.h"

typedef struct _LCD_LOG_line
{
  uint8_t  line[128];
  uint32_t color;

} LCD_LOG_line;

#define  LCD_ErrLog(...)    do { \
    log_printf(LOG_ERR, __VA_ARGS__);\
    }while (0)

#define  LCD_UsrLog(...)    do { \
    log_printf(LOG_NOTICE, __VA_ARGS__);\
    } while (0)


#define  LCD_DbgLog(...)    do { \
    log_printf(LOG_DEBUG, __VA_ARGS__);\
    }while (0)

#endif /* __LCD_LOG_H__ */
