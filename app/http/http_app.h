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
#ifndef HTTP_APP_H
#define HTTP_APP_H

typedef enum {
    URI_PATH_INDEX,
    URI_PATH_SLOT,
    URI_PATH_BOARDS,
    URI_PATH_SENSORS,
    URI_PATH_LOG,
    URI_PATH_TASKS,
    URI_PATH_ERROR,
} uri_path_t;

struct http_server_t;
struct http_request_t;

void http_app_handle_request(struct http_server_t *server, const struct http_request_t *r);
const char *page_title(uri_path_t p);

#endif // HTTP_APP_H
