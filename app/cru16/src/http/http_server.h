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
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

struct http_server_t;
struct http_request_t;

struct http_server_t *http_server_alloc(void *priv, const char *client_data);
void http_server_free(struct http_server_t *);
void *http_server_get_priv(struct http_server_t *server);
const char *http_server_get_client_data(const struct http_server_t *server);
const struct http_request_t *http_server_get_http_request(const struct http_server_t *server);
int http_server_handle_client_data(struct http_server_t *server);
int http_server_write(struct http_server_t *server, const char *str);

#ifdef __cplusplus
}
#endif

#endif // HTTP_SERVER_H
