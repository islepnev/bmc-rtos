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
#include "http_app_log.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "logentry.h"
//#include "logbuffer.h"
#include "http_server.h"

#define RETURN_IF(x) { int ret = x; if (ret) return ret; }

static int8_t http_serve_log_entry(struct http_server_t *server, uint32_t index)
{
    struct LogEntry ent;
    log_get(index, &ent);
    const char *css_class = "default";
    switch (ent.priority) {
    case LOG_EMERG: css_class = "purple"; break;
    case LOG_ALERT: css_class = "purple"; break;
    case LOG_CRIT: css_class = "purple"; break;
    case LOG_ERR: css_class = "red"; break;
    case LOG_WARNING: css_class = "yellow"; break;
    case LOG_NOTICE: css_class = "blue"; break;
    case LOG_INFO: css_class = "green"; break;
    case LOG_DEBUG: break;
    default: css_class = "purple"; break;
    }
    enum {size=30};
    static char buf[size+1];
    snprintf(buf, size, "<span class=\"%s\">%lu.%03lu ", css_class,
           ent.tick/1000, ent.tick%1000);
    int ret = http_server_write(server, buf);
    if (ret != 0)
        return ret;
    RETURN_IF(http_server_write(server, ent.str));
    if (ret != 0)
        return ret;
    strcpy(buf, "</span></br>\n");
    ret = http_server_write(server, buf);
    return ret;
}

static int http_serve_log_lines(struct http_server_t *server, uint32_t count)
{
    uint32_t max_count = count;
    if (max_count > LOG_BUF_SIZE)
        max_count = LOG_BUF_SIZE;
    volatile const uint32_t log_count = log_get_count();
    volatile const uint32_t log_wptr = log_get_wptr();
    volatile const uint32_t log_start = (log_count > max_count) ? (log_wptr + LOG_BUF_SIZE - max_count) % LOG_BUF_SIZE : 0;
    if (log_start <= log_wptr) {
        for (uint32_t i=log_start; i<log_wptr; i++) {
            int ret = http_serve_log_entry(server, i);
            if (ret != 0)
                return ret;
        }
    } else {
        for (uint32_t i=log_start; i<LOG_BUF_SIZE; i++) {
            int ret = http_serve_log_entry(server, i);
            if (ret != 0)
                return ret;
        }
        for (uint32_t i=0; i<log_wptr; i++) {
            int ret = http_serve_log_entry(server, i);
            if (ret != 0)
                return ret;
        }
    }
    return 0;
}

int http_serve_log(struct http_server_t *server)
{
    return http_serve_log_lines(server, LOG_BUF_SIZE);
}
