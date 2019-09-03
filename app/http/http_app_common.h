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
#ifndef HTTP_APP_COMMON_H
#define HTTP_APP_COMMON_H

struct http_server_t;

int http_serve_headers_200(struct http_server_t *server);
int http_serve_headers_404(struct http_server_t *server);
int http_serve_headers_405(struct http_server_t *server);
int http_serve_page_404(struct http_server_t *server);
int http_serve_page_405(struct http_server_t *server);
int http_serve_page_header(struct http_server_t *server, const char *title);
int http_serve_page_footer(struct http_server_t *server);

#endif // HTTP_APP_COMMON_H
