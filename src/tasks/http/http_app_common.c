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
#include "http_app_common.h"

#include "app_name.h"
#include "http_server.h"
#include "version.h"

static const char *SERVER_HEADERS_200 =
        "HTTP/1.0 200 OK\n"
        "Server: embedded/1.0 (RTOS)\n"
        "Content-Type: text/html; charset=utf-8\n"
        "\n";

static const char *SERVER_HEADERS_404 =
        "HTTP/1.0 404 Not found\n"
        "Server: embedded/1.0 (RTOS)\n"
        "Content-Type: text/html; charset=utf-8\n"
        "\n";

static const char *SERVER_HEADERS_405 =
        "HTTP/1.0 405 Method Not Allowed\n"
        "Server: embedded/1.0 (RTOS)\n"
        "Content-Type: text/html; charset=utf-8\n"
        "\n";

int http_serve_headers_200(struct http_server_t *server)
{
    return http_server_write(server, SERVER_HEADERS_200);
}

int http_serve_headers_404(struct http_server_t *server)
{
    return http_server_write(server, SERVER_HEADERS_404);
}

int http_serve_headers_405(struct http_server_t *server)
{
    return http_server_write(server, SERVER_HEADERS_405);
}

int http_serve_page_404(struct http_server_t *server)
{
    static const char *buf1 =
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
            "<html>\n"
            "<head>\n"
            "<title>Page not found</title>\n"
            "</head>\n"
            "<body>\n"
            "<h4>Page not found</h4>\n"
            "<hr>\n"
            "<table>\n"
            "<tbody>\n"
            "<tr>\n"
            "<td><a href=\"/\">Home</a></td>\n"
            "</tr>\n"
            "</tbody>\n"
            "</table>\n"
            "<br>\n";
    return http_server_write(server, buf1);
}

int http_serve_page_405(struct http_server_t *server)
{
    static const char *buf1 =
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
            "<html>\n"
            "<head>\n"
            "<title>Method Not Allowed</title>\n"
            "</head>\n"
            "<body>\n"
            "<h4>Method Not Allowed</h4>\n"
            "<hr>\n"
            "<table>\n"
            "<tbody>\n"
            "<tr>\n"
            "<td><a href=\"/\">Home</a></td>\n"
            "</tr>\n"
            "</tbody>\n"
            "</table>\n"
            "<br>\n";
    return http_server_write(server, buf1);
}

int http_serve_page_header(struct http_server_t *server, const char *title)
{
    static const char *buf1 =
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
            "<html>\n"
            "<head>\n"
            "<title>";
    static const char *buf2 =
            "</title>\n"
            "<style>\n"
            "#bgyellow { background-color: yellow; }\n"
            "#bgred    { background-color: red; }\n"
            "#bggreen  { background-color: green; }\n"
            "#bggray   { background-color: gray; }\n"
            ".log  { min-width: 60pt; display: inline-block; text-align: center;}\n"
            ".log-emerg  { color: #fff; background-color: #ff3547; }\n"
            ".log-alert  { color: #fff; background-color: #ff3547; }\n"
            ".log-crit   { color: #fff; background-color: #ff3547; }\n"
            ".log-err    { color: #fff; background-color: #a6c;    }\n"
            ".log-warn   { color: #000; background-color: #fb3;    }\n"
            ".log-notice { color: #fff; background-color: #33b5e5; }\n"
            ".log-info   { color: #fff; background-color: #4285f4; }\n"
            ".log-debug  { color: #000; background-color: #e0e0e0; }\n"
            "</style>\n"
            "<meta http-equiv=\"refresh\" content=\"3\">\n"
            "</head>\n"
            "<body>\n"
            "<h4>";
    static const char *buf3 =
            "</h4>\n"
            "<hr>\n"
            "<table>\n"
            "<tbody>\n"
            "<tr>\n"
            "<td><a href=\"/\">Home</a></td>\n"
            "<td><a href=\"/boards\">Boards</a></td>\n"
            "<td><a href=\"/sensors\">Sensors</a></td>\n"
            "<td><a href=\"/tasks\">Tasks</a></td>\n"
            "<td><a href=\"/log\">Log</a></td>\n"
            "</tr>\n"
            "</tbody>\n"
            "</table>\n"
            "<br>\n";
    http_server_write(server, buf1);
    http_server_write(server, title);
    http_server_write(server, buf2);
    http_server_write(server, title);
    http_server_write(server, buf3);
    return 0;
}

int http_serve_page_footer(struct http_server_t *server)
{
    static const char *buf =
            "<hr>\n"
            "<small>" APP_NAME_STR_BMC " version " VERSION_STR"</small>\n"
            "</body>\n"
            "</html>\n";
    return http_server_write(server, buf);
}
