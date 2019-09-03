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
#include "http_server.h"
#include <string.h>
#include <stdlib.h>
#include "http_app.h"
#include "http_request.h"
#include "http_server_impl.h"

typedef struct http_server_t {
    void *_priv;
    const char *_client_data;
    http_request_t r;
} http_server_t;

struct http_server_t *http_server_alloc(void *priv, const char *client_data)
{
    http_server_t *server = malloc(sizeof(struct http_server_t));
    if (server) {
        server->_priv = priv;
        server->_client_data = client_data;
    }
    return server;
}

void http_server_free(struct http_server_t *server)
{
    free(server);
}

void *http_server_get_priv(struct http_server_t *server)
{
    return server->_priv;
}

const char *http_server_get_client_data(const struct http_server_t *server)
{
    return server->_client_data;
}

const struct http_request_t *http_server_get_http_request(const struct http_server_t *server)
{
    return &server->r;
}

int http_server_handle_client_data(struct http_server_t *server)
{
    http_request_t *r = &server->r;
    tokenize_http_request_alloc(r, http_server_get_client_data(server));
    http_app_handle_request(server, r);
    tokenize_http_request_free(r);
    return 0;
}

int http_server_write(struct http_server_t *server, const char *str)
{
    return http_server_write_impl(server, str);
}
