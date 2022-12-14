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
#ifndef HTTP_SERVE_PAGE_H
#define HTTP_SERVE_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

struct http_server_t;
struct http_request_t;

int http_serve_index(struct http_server_t *server);
#ifdef BOARD_TTVXS
int http_serve_slot(struct http_server_t *server, const struct http_request_t *r);
int http_serve_boards(struct http_server_t *server);
int http_serve_sensors(struct http_server_t *server);
#endif
int http_serve_task_list(struct http_server_t *server);

#ifdef __cplusplus
}
#endif

#endif // HTTP_SERVE_PAGE_H
