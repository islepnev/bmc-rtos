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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

enum {MAX_HTTP_REQ_LEN = 100};

typedef struct http_request_t {
    char *_source;
    const char *method;
    const char *version;
    const char *path;
    const char *args;
} http_request_t;

void tokenize_http_request_alloc(struct http_request_t *r, const char *str);
void tokenize_http_request_free(struct http_request_t *r);

#endif // HTTP_REQUEST_H
