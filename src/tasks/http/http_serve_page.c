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
#include "http_serve_page.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

//#include "lwip/err.h"
//#include "lwip/api.h"
#include "cmsis_os.h"

#include "http_request.h"
#include "http_server.h"

#include "http_app.h"
#include "http_app_log.h"

#include "devicelist.h"
#include "ipmi_sensor_types.h"
#include "powermon/dev_powermon_types.h"
#include "system_status.h"
#include "version.h"
#include "vxsiicm/dev_vxsiicm_types.h"

#define RETURN_IF(x) { int ret = x; if (ret) return ret; }

static const char *sensor_status_html(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return "unknown";
    case SENSOR_NORMAL:   return "normal";
    case SENSOR_WARNING:  return "<span class=\"yellow\">warning</span>";
    case SENSOR_CRITICAL: return "<span class=\"red\">critical</span>";
    default: return "invalid";
    }
}

static const char *pm_state_html(PmState state)
{
    switch(state) {
    case PM_STATE_INIT:    return "INIT";
    case PM_STATE_WAITINPUT: return "WAIT-INPUT";
    case PM_STATE_STANDBY: return "STANDBY";
    case PM_STATE_RAMP:    return "<span class=\"yellow\">RAMP</span>";
    case PM_STATE_RUN:     return "<span class=\"green\">RUN</span>";
    case PM_STATE_PWRFAIL: return "<span class=\"red\">POWER SUPPLY FAILURE</span>";
    case PM_STATE_FAILWAIT: return "<span class=\"red\">POWER SUPPLY FAILURE</span>";
    case PM_STATE_ERROR:   return "<span class=\"red\">ERROR</span>";
    case PM_STATE_OFF: return "SWITCH-OFF";
    case PM_STATE_OVERHEAT: return "<span class=\"red\">OVERHEAT</span>";
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

int http_serve_index(struct http_server_t *server)
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
    sprintf(str, "System status: %s<br>\n", sensor_status_html(getSystemStatus()));
    strcat(buf, str);
    sprintf(str, "Powermon state: %s<br>\n", pm_state_html(get_powermon_state()));
    strcat(buf, str);
    strcat(buf, "<p><a href=\"https://afi-project.jinr.ru/projects/ttvxs/wiki\">TTVXS project Wiki</a>\n");
    return http_server_write(server, buf);
}

static const char *task_list_header =
        "";
static const char *task_list_footer =
        "";

int http_serve_task_list(struct http_server_t *server)
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
    int ret = http_server_write(server, task_list_header);
    if (ret != 0)
        return ret;
    ret = http_server_write(server, PAGE_BODY);
    if (ret != 0)
        return ret;

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
    ret = http_server_write(server, PAGE_BODY);
    if (ret != 0)
        return ret;

    //

    ret = http_server_write(server, task_list_footer);
    return ret;
}

static const char *sensorUnitsStr(IpmiSensorType t)
{
    switch (t) {
    case IPMI_SENSOR_DISCRETE:
        return "";
    case IPMI_SENSOR_TEMPERATURE:
        return "??C";
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
#ifdef BOARD_TTVXS
int http_serve_sensors(struct http_server_t *server)
{
    int ret = http_server_write(server, task_list_header);
    if (ret != 0)
        return ret;

    static char buf[2000];
    strcpy(buf, "<table>\n");
    ret = http_server_write(server, buf);
    if (ret != 0)
        return ret;

    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICM);
    const Dev_vxsiicm_priv *vxsiicm = (const Dev_vxsiicm_priv *)device_priv_const(d);

    for (uint32_t pp=0; vxsiicm && pp<VXSIIC_SLOTS; pp++) {
        const vxsiic_slot_status_t *status = &vxsiicm->status.slot[pp];
        if (!status->present)
            continue;
        uint16_t sensor_count = status->mcu_sensors.count;
        if (sensor_count > MAX_SENSOR_COUNT)
            sensor_count = MAX_SENSOR_COUNT;
        buf[0] = '\0';
        {
            static char str[100];
            sprintf(str, "<tr>\n<td>Slot %s:</td>\n", vxsiic_map_slot_to_label[pp]);
            strcat(buf, str);
        }
        for (uint16_t i=0; i<sensor_count; i++) {
            const GenericSensor *sensor = &status->mcu_sensors.sensors[i];
            static char str[200];
            static char name[SENSOR_NAME_SIZE];
            strncpy(name, sensor->name, SENSOR_NAME_SIZE-1);
            name[SENSOR_NAME_SIZE-1] = '\0';
            SensorStatus s = (SensorStatus)sensor->hdr.b.state;
            sprintf(str, "<td id=\"%s\">%s<br>", sensorStatusCssStyle(s), name);
            strcat(buf, str);
            switch (sensor->hdr.b.type) {
            case IPMI_SENSOR_DISCRETE:
                sprintf(str, "%d", (int)sensor->value);
                strcat(buf, str);
                break;
            case IPMI_SENSOR_CURRENT:
            case IPMI_SENSOR_VOLTAGE:
                sprintf(str, "%dm", (int)(sensor->value*1000));
                strcat(buf, str);
                break;
            case IPMI_SENSOR_TEMPERATURE:
                sprintf(str, "%d", (int)(sensor->value));
                strcat(buf, str);
                break;
            default:
                break;
            }
//            double f = status->sensors[i].value;
//            sprintf(str, "%.3f", f); // FIXME: causes HardFault
//            strcat(buf, str);
            sprintf(str, "%s</td>\n", sensorUnitsStr((IpmiSensorType)sensor->hdr.b.type));
            strcat(buf, str);
        }
        strcat(buf, "</tr>\n");
        ret = http_server_write(server, buf);
        if (ret != 0)
            return ret;
    }
    strcpy(buf, "</table>\n");
    ret = http_server_write(server, buf);
    return ret;
}

int http_serve_boards(struct http_server_t *server)
{
    RETURN_IF(http_server_write(server, task_list_header));

    static char buf[2000];
    strcpy(buf, "<table>\n");
    RETURN_IF(http_server_write(server, buf));

    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICM);
    const Dev_vxsiicm_priv *vxsiicm = (const Dev_vxsiicm_priv *)device_priv_const(d);

    for (uint32_t pp=0; vxsiicm && pp<VXSIIC_SLOTS; pp++) {
        const vxsiic_slot_status_t *status = &vxsiicm->status.slot[pp];
        if (!status->present)
            continue;
        uint16_t sensor_count = status->mcu_sensors.count;
        if (sensor_count > MAX_SENSOR_COUNT)
            sensor_count = MAX_SENSOR_COUNT;
        buf[0] = '\0';
        {
            static char str[100];
            const char *label = vxsiic_map_slot_to_label[pp];
            sprintf(str, "<tr>\n<td><a href=\"?s=%s\">Slot %s:</a></td>\n", label, label);
            strcat(buf, str);
        }
        for (uint16_t i=0; i<sensor_count; i++) {
            const GenericSensor *sensor = &status->mcu_sensors.sensors[i];
            static char str[200];
            static char name[SENSOR_NAME_SIZE];
            strncpy(name, sensor->name, SENSOR_NAME_SIZE-1);
            name[SENSOR_NAME_SIZE-1] = '\0';
            SensorStatus s = (SensorStatus)sensor->hdr.b.state;
            sprintf(str, "<td id=\"%s\">%s<br>", sensorStatusCssStyle(s), name);
            strcat(buf, str);
            switch (sensor->hdr.b.type) {
            case IPMI_SENSOR_DISCRETE:
                sprintf(str, "%d", (int)sensor->value);
                strcat(buf, str);
                break;
            case IPMI_SENSOR_CURRENT:
            case IPMI_SENSOR_VOLTAGE:
                sprintf(str, "%dm", (int)(sensor->value*1000));
                strcat(buf, str);
                break;
            case IPMI_SENSOR_TEMPERATURE:
                sprintf(str, "%d", (int)(sensor->value));
                strcat(buf, str);
                break;
            default:
                break;
            }
//            double f = status->sensors[i].value;
//            sprintf(str, "%.3f", f); // FIXME: causes HardFault
//            strcat(buf, str);
            sprintf(str, "%s</td>\n", sensorUnitsStr((IpmiSensorType)sensor->hdr.b.type));
            strcat(buf, str);
        }
        strcat(buf, "</tr>\n");
        RETURN_IF(http_server_write(server, buf));
    }
    strcpy(buf, "</table>\n");
    RETURN_IF(http_server_write(server, buf));
    return 0;
}

int http_serve_slot(struct http_server_t *server, const struct http_request_t *r)
{
    return 0;
}

#endif
