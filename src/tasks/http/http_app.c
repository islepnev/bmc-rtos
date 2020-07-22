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
#include "http_app.h"

#include <string.h>
#include "logbuffer.h"
#include "http_request.h"
#include "http_serve_page.h"
#include "http_app_common.h"
#include "http_app_log.h"

const char *page_title(uri_path_t p)
{
    switch (p) {
    case URI_PATH_INDEX:
        return "Index";
    case URI_PATH_BOARDS:
        return "Boards";
    case URI_PATH_SENSORS:
        return "Sensors";
    case URI_PATH_LOG:
        return "Message Log";
    case URI_PATH_TASKS:
        return "RTOS Tasks";
    case URI_PATH_ERROR:
        return "Error";
    default:
        return "";
    }
}

int str_startswith(const char *str, const char *match)
{
    const size_t match_len = strlen(match);
    return strncmp(str, match, match_len);
}

static uri_path_t match_uri_path(const char *str)
{
    if (strcmp(str, "/tasks") == 0)
        return URI_PATH_TASKS;
    if (strcmp(str, "/boards") == 0)
        return URI_PATH_BOARDS;
    if (strcmp(str, "/sensors") == 0)
        return URI_PATH_SENSORS;
    if (strcmp(str, "/log") == 0)
        return URI_PATH_LOG;
    if (strcmp(str, "/") == 0)
        return URI_PATH_INDEX;
    if (str_startswith(str, "/slot/") == 0)
        return URI_PATH_SLOT;
    return URI_PATH_ERROR;
}

static void http_app_handle_get(struct http_server_t *server, const struct http_request_t *r)
{
    uri_path_t p = match_uri_path(r->path);
    if (p == URI_PATH_ERROR) {
        http_serve_headers_404(server);
        http_serve_page_404(server);
        http_serve_page_footer(server);
        return;
    }
    http_serve_headers_200(server);
    http_serve_page_header(server, page_title(p));
    switch (p) {
    case URI_PATH_INDEX:
        http_serve_index(server);
        break;
#ifdef BOARD_TTVXS
    case URI_PATH_SLOT:
        http_serve_slot(server, r);
        break;
    case URI_PATH_BOARDS:
        http_serve_boards(server);
        break;
    case URI_PATH_SENSORS:
        http_serve_sensors(server);
        break;
#endif
    case URI_PATH_LOG:
        http_serve_log(server);
        break;
    case URI_PATH_TASKS:
        http_serve_task_list(server);
        break;
    case URI_PATH_ERROR:
        break;
    default:
        break;
    }
    http_serve_page_footer(server);
}

void http_app_handle_request(struct http_server_t *server, const struct http_request_t *r)
{
    // log_printf(LOG_INFO, "httpd: '%s', '%s', '%s', '%s'", r->method, r->version, r->path, r->args);

    if (0 == strcmp(r->method, "GET")) {
        http_app_handle_get(server, r);
    } else {
        http_serve_headers_405(server);
        http_serve_page_405(server);
        http_serve_page_footer(server);
    }
}
