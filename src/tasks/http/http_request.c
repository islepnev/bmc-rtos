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

#include "http_request.h"

#include <stdlib.h>
#include <string.h>

void tokenize_http_request_alloc(struct http_request_t *r, const char *str)
{
    r->_source = strndup(str, MAX_HTTP_REQ_LEN);
    char *buf = r->_source;
    char *line = strsep(&buf, "\r\n");
    r->method = strsep(&line, " "); // 'GET'
    char *uri = strsep(&line, " "); // '/index?arg=val&arg2'
    r->version = strsep(&line, " "); // 'HTTP/1.0'
    r->path = strsep(&uri, "?"); // '/index'
    r->args = uri; // 'arg=val&arg2'
}

void tokenize_http_request_free(struct http_request_t *r)
{
    free(r->_source);
}
