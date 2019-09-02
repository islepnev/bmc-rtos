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

#include "httpserver-netconn.h"
#include <string.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "cmsis_os.h"
#include "version.h"
#include "logentry.h"
#include "app_shared_data.h"
#include "system_status.h"
#include "devices_types.h"
//#include "rtc_util.h"

#define WEBSERVER_THREAD_PRIO    ((TCPIP_THREAD_PRIO) - 1)

static const unsigned char SERVER_HEADERS_200[] =
        "HTTP/1.0 200 OK\n"
        "Server: embedded/1.0 (RTOS)\n"
        "Content-Type: text/html; charset=utf-8\n"
        "\n";

static const unsigned char SERVER_HEADERS_404[] =
        "HTTP/1.0 404 Not found\n"
        "Server: embedded/1.0 (RTOS)\n"
        "Content-Type: text/html; charset=utf-8\n"
        "\n";

static const char task_list_header[] =
        "";
static const char task_list_footer[] =
        "";

static err_t http_serve_headers_200(struct netconn *conn)
{
    return netconn_write(conn, SERVER_HEADERS_200, strlen((char*)SERVER_HEADERS_200), NETCONN_COPY);
}

static err_t http_serve_headers_404(struct netconn *conn)
{
    return netconn_write(conn, SERVER_HEADERS_404, strlen((char*)SERVER_HEADERS_404), NETCONN_COPY);
}

static err_t http_serve_page_404(struct netconn *conn)
{
    static const char buf1[] =
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
    netconn_write(conn, buf1,  strlen(buf1), NETCONN_COPY);
    return ERR_OK;
}

static err_t http_serve_page_header(struct netconn *conn, const char *title)
{
    static const char buf1[] =
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
            "<html>\n"
            "<head>\n"
            "<title>";
    static const char buf2[] =
            "</title>\n"
            "<style>\n"
            "#bgyellow { background-color: yellow; }\n"
            "#bgred { background-color: red; }\n"
            "#bggreen { background-color: green; }\n"
            "#bggray { background-color: gray; }\n"
            ".yellow { color: yellow; }\n"
            ".red { color: red; }\n"
            ".purple { color: purple; }\n"
            ".blue { color: blue; }\n"
            ".green { color: green; }\n"
            "</style>\n"
            "<meta http-equiv=\"refresh\" content=\"3\">\n"
            "</head>\n"
            "<body>\n"
            "<h4>";
    static const char buf3[] =
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
    netconn_write(conn, buf1,  strlen(buf1), NETCONN_COPY);
    netconn_write(conn, title, strlen(title), NETCONN_COPY);
    netconn_write(conn, buf2,  strlen(buf2), NETCONN_COPY);
    netconn_write(conn, title, strlen(title), NETCONN_COPY);
    netconn_write(conn, buf3,  strlen(buf3), NETCONN_COPY);
    return ERR_OK;
}

static err_t http_serve_page_footer(struct netconn *conn)
{
    static const char buf[] =
            "<hr>\n"
            "<small>"APP_NAME_STR " version " VERSION_STR"</small>\n"
            "</body>\n"
            "</html>\n";
    return netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
}

static const char *sensorStatusStr(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return "unknown";
    case SENSOR_NORMAL:   return "normal";
    case SENSOR_WARNING:  return "<span class=\"yellow\">warning</span>";
    case SENSOR_CRITICAL: return "<span class=\"red\">critical</span>";
    default: return "invalid";
    }
}

static const char *pmStateStr(PmState state)
{
    switch(state) {
    case PM_STATE_INIT:    return "INIT";
    case PM_STATE_STANDBY: return "STANDBY";
    case PM_STATE_RAMP:    return "<span class=\"yellow\">RAMP</span>";
    case PM_STATE_RUN:     return "<span class=\"green\">RUN</span>";
    case PM_STATE_PWRFAIL: return "<span class=\"red\">POWER SUPPLY FAILURE</span>";
    case PM_STATE_ERROR:   return "<span class=\"red\">ERROR</span>";
    default: return "?";
    }
}

static void sprintf_uptime(char *str)
{
    uint32_t ss = osKernelSysTick() / osKernelSysTickFrequency;
    uint16_t dd = ss / 86400;
    ss -= dd*86400;
    uint16_t hh = ss / 3600;
    ss -= hh*3600;
    uint16_t mm = ss / 60;
    ss -= mm*60;
    if (dd > 1)
        sprintf(str, "%u days ", dd);
    if (dd == 1)
        sprintf(str, "%u day ", dd);
    sprintf(str, "%2u:%02u:%02lu", hh, mm, ss);
}

//static void print_rtc_str(void)
//{
//    struct tm tm;
//    get_rtc_tm(&tm);
//    char buf[32];
//    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &tm);
//    printf("%s", buf);
//}

static err_t http_serve_index(struct netconn *conn)
{
    static char buf[1024];
    buf[0] = '\0';
    static char str[100];
    sprintf(str, "System uptime: ");
    strcat(buf, str);
    sprintf_uptime(str);
    strcat(buf, str);
    sprintf(str, "<br>\n");
    strcat(buf, str);
    sprintf(str, "System status: %s<br>\n", sensorStatusStr(getSystemStatus(getDevicesConst())));
    strcat(buf, str);
    sprintf(str, "Powermon state: %s<br>\n", pmStateStr(getDevicesConst()->pm.pmState));
    strcat(buf, str);
    strcat(buf, "<p><a href=\"https://afi-project.jinr.ru/projects/ttvxs/wiki\">TTVXS project Wiki</a>\n");
    return netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
}

static void http_serve_log_entry(struct netconn *conn, uint32_t index)
{
    struct LogEntry ent;
    log_get(index, &ent);
    const char *css_class = "default";
    switch (ent.priority) {
    case LOG_EMERG: css_class = "purple"; break;
    case LOG_ALERT: css_class = "purple"; break;
    case LOG_CRIT: css_class = "purple"; break;
    case LOG_ERR: css_class = "red"; break;
    case LOG_WARNING: css_class = "yellow"; break;
    case LOG_NOTICE: css_class = "blue"; break;
    case LOG_INFO: css_class = "green"; break;
    case LOG_DEBUG: break;
    default: css_class = "purple"; break;
    }
    enum {size=30};
    static char buf[size+1];
    snprintf(buf, size, "<span class=\"%s\">%lu.%03lu ", css_class,
           ent.tick/1000, ent.tick%1000);
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
    netconn_write(conn, ent.str, strlen(ent.str), NETCONN_COPY);
    strcpy(buf, "</span></br>\n");
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
}

static void http_serve_log_lines(struct netconn *conn, uint32_t count)
{
    uint32_t max_count = count;
    if (max_count > LOG_BUF_SIZE)
        max_count = LOG_BUF_SIZE;
    volatile const uint32_t log_count = log_get_count();
    volatile const uint32_t log_wptr = log_get_wptr();
    volatile const uint32_t log_start = (log_count > max_count) ? (log_wptr + LOG_BUF_SIZE - max_count) % LOG_BUF_SIZE : 0;
    if (log_start <= log_wptr) {
        for (uint32_t i=log_start; i<log_wptr; i++)
            http_serve_log_entry(conn, i);
    } else {
        for (uint32_t i=log_start; i<LOG_BUF_SIZE; i++)
            http_serve_log_entry(conn, i);
        for (uint32_t i=0; i<log_wptr; i++)
            http_serve_log_entry(conn, i);
    }
}

static err_t http_serve_log(struct netconn *conn)
{
    http_serve_log_lines(conn, LOG_BUF_SIZE);
    return ERR_OK;
}

static err_t http_serve_task_list(struct netconn *conn)
{
    static portCHAR PAGE_BODY[1024];

    PAGE_BODY[0] = '\0';
    strcat((char *)PAGE_BODY, "<pre>Name          State  Priority  Stack   Num\n" );
    strcat((char *)PAGE_BODY, "---------------------------------------------\n");

    /* The list of tasks and their status */
    osThreadList((unsigned char *)(PAGE_BODY + strlen(PAGE_BODY)));
    strcat((char *)PAGE_BODY, "---------------------------------------------\n");
    strcat((char *)PAGE_BODY, "B : Blocked, R : Ready, D : Deleted, S : Suspended</pre>\n");

    /* Send the dynamically generated page */
    netconn_write(conn, task_list_header, strlen(task_list_header), NETCONN_COPY);
    netconn_write(conn, PAGE_BODY, strlen(PAGE_BODY), NETCONN_COPY);

    //
    PAGE_BODY[0] = '\0';
    char *buf = PAGE_BODY;
    strcpy(buf, "<pre>\nTask");
    buf += strlen(buf);
    for(int i = strlen("Task"); i < ( configMAX_TASK_NAME_LEN - 3 ); i++) {
        *buf = ' ';
        buf++;
        *buf = '\0';
    }
    const char *hdr = "  Abs Time      % Time\r\n****************************************\n";
    strcpy(buf, hdr);
    buf += strlen(hdr);
    vTaskGetRunTimeStats(buf);
    strcat(buf, "</pre>\n");
    netconn_write(conn, PAGE_BODY, strlen(PAGE_BODY), NETCONN_COPY);

    //

    netconn_write(conn, task_list_footer, strlen(task_list_footer), NETCONN_COPY);
    return ERR_OK;
}

static const char *sensorUnitsStr(IpmiSensorType t)
{
    switch (t) {
    case IPMI_SENSOR_DISCRETE:
        return "";
    case IPMI_SENSOR_TEMPERATURE:
        return "°C";
    case IPMI_SENSOR_VOLTAGE:
        return "V";
    case IPMI_SENSOR_CURRENT:
        return "A";
    default:
        return "";
    }
}

static const char *sensorStatusCssStyle(SensorStatus s)
{
    switch (s) {
    case SENSOR_UNKNOWN:
        return "bggray";
    case SENSOR_NORMAL:
        return "";
    case SENSOR_WARNING:
        return "bgyellow";
    case SENSOR_CRITICAL:
        return "bgred";
    default:
        return "";
    }
}

static err_t http_serve_sensors(struct netconn *conn)
{
    netconn_write(conn, task_list_header, strlen(task_list_header), NETCONN_COPY);

    static char buf[2000];
    strcpy(buf, "<table>\n");
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);

    const Dev_vxsiic *d = &getDevicesConst()->vxsiic;
    for (uint32_t pp=0; pp<VXSIIC_SLOTS; pp++) {
        const vxsiic_slot_status_t *status = &d->status.slot[pp];
        if (!status->present)
            continue;
        uint16_t sensor_count = status->sensor_count;
        if (sensor_count > MAX_SENSOR_COUNT)
            sensor_count = MAX_SENSOR_COUNT;
        buf[0] = '\0';
        {
            static char str[100];
            sprintf(str, "<tr>\n<td>Slot %s:</td>\n", vxsiic_map_slot_to_label[pp]);
            strcat(buf, str);
        }
        for (uint16_t i=0; i<sensor_count; i++) {
            static char str[200];
            static char name[SENSOR_NAME_SIZE];
            strncpy(name, status->sensors[i].name, SENSOR_NAME_SIZE-1);
            name[SENSOR_NAME_SIZE-1] = '\0';
            SensorStatus s = status->sensors[i].hdr.b.state;
            sprintf(str, "<td id=\"%s\">%s<br>", sensorStatusCssStyle(s), name);
            strcat(buf, str);
            switch (status->sensors[i].hdr.b.type) {
            case IPMI_SENSOR_DISCRETE:
                sprintf(str, "%d", (int)status->sensors[i].value);
                strcat(buf, str);
                break;
            case IPMI_SENSOR_CURRENT:
            case IPMI_SENSOR_VOLTAGE:
                sprintf(str, "%dm", (int)(status->sensors[i].value*1000));
                strcat(buf, str);
                break;
            case IPMI_SENSOR_TEMPERATURE:
                sprintf(str, "%d", (int)(status->sensors[i].value));
                strcat(buf, str);
                break;
            default:
                break;
            }
//            double f = status->sensors[i].value;
//            sprintf(str, "%.3f", f); // FIXME: causes HardFault
//            strcat(buf, str);
            sprintf(str, "%s</td>\n", sensorUnitsStr(status->sensors[i].hdr.b.type));
            strcat(buf, str);
        }
        strcat(buf, "</tr>\n");
        netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
    }
    strcpy(buf, "</table>\n");
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
    return ERR_OK;
}

static err_t http_serve_boards(struct netconn *conn)
{
    netconn_write(conn, task_list_header, strlen(task_list_header), NETCONN_COPY);

    static char buf[2000];
    strcpy(buf, "<table>\n");
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);

    const Dev_vxsiic *d = &getDevicesConst()->vxsiic;
    for (uint32_t pp=0; pp<VXSIIC_SLOTS; pp++) {
        const vxsiic_slot_status_t *status = &d->status.slot[pp];
        if (!status->present)
            continue;
        uint16_t sensor_count = status->sensor_count;
        if (sensor_count > MAX_SENSOR_COUNT)
            sensor_count = MAX_SENSOR_COUNT;
        buf[0] = '\0';
        {
            static char str[100];
            sprintf(str, "<tr>\n<td>Slot %s:</td>\n", vxsiic_map_slot_to_label[pp]);
            strcat(buf, str);
        }
        for (uint16_t i=0; i<sensor_count; i++) {
            static char str[200];
            static char name[SENSOR_NAME_SIZE];
            strncpy(name, status->sensors[i].name, SENSOR_NAME_SIZE-1);
            name[SENSOR_NAME_SIZE-1] = '\0';
            SensorStatus s = status->sensors[i].hdr.b.state;
            sprintf(str, "<td id=\"%s\">%s<br>", sensorStatusCssStyle(s), name);
            strcat(buf, str);
            switch (status->sensors[i].hdr.b.type) {
            case IPMI_SENSOR_DISCRETE:
                sprintf(str, "%d", (int)status->sensors[i].value);
                strcat(buf, str);
                break;
            case IPMI_SENSOR_CURRENT:
            case IPMI_SENSOR_VOLTAGE:
                sprintf(str, "%dm", (int)(status->sensors[i].value*1000));
                strcat(buf, str);
                break;
            case IPMI_SENSOR_TEMPERATURE:
                sprintf(str, "%d", (int)(status->sensors[i].value));
                strcat(buf, str);
                break;
            default:
                break;
            }
//            double f = status->sensors[i].value;
//            sprintf(str, "%.3f", f); // FIXME: causes HardFault
//            strcat(buf, str);
            sprintf(str, "%s</td>\n", sensorUnitsStr(status->sensors[i].hdr.b.type));
            strcat(buf, str);
        }
        strcat(buf, "</tr>\n");
        netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
    }
    strcpy(buf, "</table>\n");
    netconn_write(conn, buf, strlen(buf), NETCONN_COPY);
    return ERR_OK;
}

typedef enum {
    URI_PATH_INDEX,
    URI_PATH_BOARDS,
    URI_PATH_SENSORS,
    URI_PATH_LOG,
    URI_PATH_TASKS,
    URI_PATH_ERROR,
} uri_path_t;

static const char *page_title(uri_path_t p)
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
        return "TTVXS tasks";
    case URI_PATH_ERROR:
        return "Error";
    default:
        return "";
    }
}

// match against string + trailing space
int match_request(const char *str, const char *match)
{
    const size_t str_len = strlen(str);
    const size_t match_len = strlen(match);
    if (str_len < match_len + 1)
        return -1;
    if (str[match_len] != ' ')
        return -1;
    return strncmp(str, match, match_len);
}

static uri_path_t match_uri_path(const char *str)
{
    if (match_request(str, "tasks") == 0)
        return URI_PATH_TASKS;
    if (match_request(str, "boards") == 0)
        return URI_PATH_BOARDS;
    if (match_request(str, "sensors") == 0)
        return URI_PATH_SENSORS;
    if (match_request(str, "log") == 0)
        return URI_PATH_LOG;
    if (match_request(str, "") == 0)
        return URI_PATH_INDEX;
    return URI_PATH_ERROR;
}

static void http_server_serve_page(struct netconn *conn, uri_path_t p)
{
    if (p == URI_PATH_ERROR) {
        http_serve_headers_404(conn);
        http_serve_page_404(conn);
        http_serve_page_footer(conn);
        return;
    }
    http_serve_headers_200(conn);
    http_serve_page_header(conn, page_title(p));
    switch (p) {
    case URI_PATH_INDEX:
        http_serve_index(conn);
        break;
    case URI_PATH_BOARDS:
        http_serve_boards(conn);
        break;
    case URI_PATH_SENSORS:
        http_serve_sensors(conn);
        break;
    case URI_PATH_LOG:
        http_serve_log(conn);
        break;
    case URI_PATH_TASKS:
        http_serve_task_list(conn);
        break;
    case URI_PATH_ERROR:
        break;
    default:
        break;
    }
    http_serve_page_footer(conn);
}

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

            if ((buflen >= 5) && (strncmp(buf, "GET /", 5) == 0))
            {
                const char *path = &buf[5];
                uri_path_t p = match_uri_path(path);
                http_server_serve_page(conn, p);
            }
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

void http_server_netconn_init()
{
    sys_thread_new("http", http_server_netconn_thread, NULL, 2*DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
}
