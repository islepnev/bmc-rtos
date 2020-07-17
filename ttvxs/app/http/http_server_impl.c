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

#include "http_server_impl.h"
#include <string.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "cmsis_os.h"
#include "http/http_server.h"
#include "http/http_request.h"

#define WEBSERVER_THREAD_PRIO    ((TCPIP_THREAD_PRIO) - 1)

static void http_server_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    err_t recv_err;
    char* buf;
    u16_t buflen;

    /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
    recv_err = netconn_recv(conn, &inbuf);

    if (recv_err == ERR_OK)
    {
        if (netconn_err(conn) == ERR_OK)
        {
            netbuf_data(inbuf, (void**)&buf, &buflen);
            if (buflen > MAX_HTTP_REQ_LEN) // limit heap usage
                buflen = MAX_HTTP_REQ_LEN;
            char *str = strndup(buf, buflen); // make null-terminated
            struct http_server_t *server = http_server_alloc(conn, str);
            http_server_handle_client_data(server);
            http_server_free(server);
            free(str);
        }
    }
    netconn_close(conn);
    netbuf_delete(inbuf);
}

static void http_server_netconn_thread(void *arg)
{
    (void)arg;
    struct netconn *conn = netconn_new(NETCONN_TCP);

    if (conn != NULL) {
        err_t err = netconn_bind(conn, NULL, 80);

        if (err == ERR_OK) {
            netconn_listen(conn);
            while (1) {
                struct netconn *newconn;
                err_t accept_err = netconn_accept(conn, &newconn);
                if (accept_err == ERR_OK) {
                    http_server_serve(newconn);
                    netconn_delete(newconn);
                }
            }
        }
    }
}

void http_server_init(void)
{
    sys_thread_new("http", http_server_netconn_thread, NULL, 2*DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
}

int http_server_write_impl(struct http_server_t *server, const char *str)
{
    if (!server)
        return -1;

    struct netconn *conn = (struct netconn *) http_server_get_priv(server);
    return netconn_write(conn, str, strlen(str), NETCONN_COPY);
}
