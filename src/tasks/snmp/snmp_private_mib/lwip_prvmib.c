/**
 * @file
 * lwip Private MIB
 *
 * @todo create MIB file for this example
 * @note the lwip enterprise tree root (26381) is owned by the lwIP project.
 * It is NOT allowed to allocate new objects under this ID (26381) without our,
 * the lwip developers, permission!
 *
 * Please apply for your own ID with IANA: http://www.iana.org/numbers.html
 *
 * lwip        OBJECT IDENTIFIER ::= { enterprises 26381 }
 * example     OBJECT IDENTIFIER ::= { lwip 1 }
 */

/*
 * Copyright (c) 2006 Axon Digital Design B.V., The Netherlands.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Christiaan Simons <christiaan.simons@axon.tv>
 */

#include "private_mib.h"

#if LWIP_SNMP

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "lwip/apps/snmp_table.h"
#include "lwip/apps/snmp_scalar.h"

#include "app_name.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "bsp_sensors_config.h"
#include "devices.h"
#include "devices_types.h"
#include "fpga/dev_fpga_types.h"
#include "powermon/dev_pm_sensors_types.h"
#include "powermon/dev_powermon_types.h"
#include "rtc_util.h"
#include "snmp_boards_table.h"
#include "snmp_ipmi_sensors_table.h"
#include "snmp_powermon_table.h"
#include "snmp_sfp_table.h"
#include "system_status.h"
#include "system_status_common.h"
#include "thset/dev_thset_types.h"
#include "version.h"

static s16_t sensor_count_get_value(struct snmp_node_instance* instance, void* value)
{
    //  size_t count = 0;
    u32_t *uint_ptr = (u32_t*)value;

    LWIP_UNUSED_ARG(instance);

    *uint_ptr = (u32_t)POWERMON_SENSORS;
    return sizeof(*uint_ptr);
}

static s16_t systemFirmwareName_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    const char *name = APP_NAME_STR;
    size_t len = strlen(name);
    MEMCPY(value, name, len);
    return (s16_t)len;
}

static s16_t systemFirmwareVersion_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    const char *name = VERSION_STR;
    size_t len = strlen(name);
    MEMCPY(value, name, len);
    return (s16_t)len;
}

static s16_t systemStatus_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)getSystemStatus();
    return sizeof(*uint_ptr);
}

static s16_t systemDateTime_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    struct tm tm = {0};
    get_rtc_tm(&tm);
    char buf[12];
    u_short tmp_year = htons(tm.tm_year + 1900);
    memcpy(buf, &tmp_year, sizeof(tmp_year));
    buf[2] = tm.tm_mon;
    buf[3] = tm.tm_mday+1;
    buf[4] = tm.tm_hour;
    buf[5] = tm.tm_min;
    buf[6] = tm.tm_sec;
    buf[7] = 0;
    buf[8] = '+'; //(utc_offset_direction < 0) ? '-' : '+';
    buf[9] = 0; // utc_offset_hours;
    buf[10] = 0; // utc_offset_minutes;
    buf[11] = 0;
    size_t len = 11;
    MEMCPY(value, buf, len);
    return (s16_t)len;
}

static s16_t systemPowerSwitch_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)(enable_power ? 1 : 2);
    return sizeof(*uint_ptr);
}

static s16_t systemTemperature_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    const Dev_thset_priv *p = get_thset_priv_const();
    if (!p || !p->count)
        return 0;
    s32_t t = (s32_t)p->sensors[0].value * 100;
    *uint_ptr = (s32_t)(t);
    return sizeof(*uint_ptr);
}

static s16_t systemVXSPowerEnable_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    return snmp_encode_truthvalue((s32_t *)value, getDevicesConst()->pen_b == 0);
}

static s16_t systemSDCardPresent_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    return snmp_encode_truthvalue((s32_t *)value, getDevicesConst()->sd.detect_b == 0);
}

static const struct snmp_scalar_node systemStatus = SNMP_SCALAR_CREATE_NODE_READONLY(
        1, SNMP_ASN1_TYPE_INTEGER, systemStatus_get_value);

static const struct snmp_scalar_node systemFirmwareName = SNMP_SCALAR_CREATE_NODE_READONLY(
        2, SNMP_ASN1_TYPE_OCTET_STRING, systemFirmwareName_get_value);

static const struct snmp_scalar_node systemFirmwareVersion = SNMP_SCALAR_CREATE_NODE_READONLY(
        3, SNMP_ASN1_TYPE_OCTET_STRING, systemFirmwareVersion_get_value);

static const struct snmp_scalar_node systemDateTime = SNMP_SCALAR_CREATE_NODE_READONLY(
        4, SNMP_ASN1_TYPE_OCTET_STRING, systemDateTime_get_value);

static const struct snmp_scalar_node systemPowerSwitch = SNMP_SCALAR_CREATE_NODE_READONLY(
        5, SNMP_ASN1_TYPE_INTEGER, systemPowerSwitch_get_value);

static const struct snmp_scalar_node systemTemperature = SNMP_SCALAR_CREATE_NODE_READONLY(
        6, SNMP_ASN1_TYPE_INTEGER, systemTemperature_get_value);

static const struct snmp_scalar_node systemVXSPowerEnable = SNMP_SCALAR_CREATE_NODE_READONLY(
        7, SNMP_ASN1_TYPE_INTEGER, systemVXSPowerEnable_get_value);

static const struct snmp_scalar_node systemSDCardPresent = SNMP_SCALAR_CREATE_NODE_READONLY(
        8, SNMP_ASN1_TYPE_INTEGER, systemSDCardPresent_get_value);

static const struct snmp_node* const system_nodes[] = {
    &systemStatus.node.node,
    &systemFirmwareName.node.node,
    &systemFirmwareVersion.node.node,
    &systemDateTime.node.node,
    &systemPowerSwitch.node.node,
    &systemTemperature.node.node,
    &systemVXSPowerEnable.node.node,
    &systemSDCardPresent.node.node
};

// ---

static s16_t sysmonStatus_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)getPowermonStatus();
    return sizeof(*uint_ptr);
}

static const struct snmp_scalar_node supplyStatus = SNMP_SCALAR_CREATE_NODE_READONLY(
        1, SNMP_ASN1_TYPE_INTEGER, sysmonStatus_get_value);

static const struct snmp_scalar_node supplySensorCount = SNMP_SCALAR_CREATE_NODE_READONLY(
        2, SNMP_ASN1_TYPE_INTEGER, sensor_count_get_value);

static const struct snmp_node* const sensors_nodes[] = {
    &sensor_table.node.node
};

static const struct snmp_tree_node supplySensors_node = SNMP_CREATE_TREE_NODE(3, sensors_nodes);

// --- FPGA

static s16_t fpgaStatus_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)get_fpga_sensor_status();
    return sizeof(*uint_ptr);
}

static const struct snmp_scalar_node fpgaStatus = SNMP_SCALAR_CREATE_NODE_READONLY(
        1, SNMP_ASN1_TYPE_INTEGER, fpgaStatus_get_value);

static s16_t fpgaDeviceId_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)get_fpga_id();
    return sizeof(*uint_ptr);
}

static s16_t fpgaDeviceIdStr_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u16_t deviceId = htons(get_fpga_id());
    MEMCPY(value, &deviceId, 2);
    return 2;
}

static s16_t fpgaVersion_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)get_fpga_fw_ver();
    return sizeof(*uint_ptr);
}

static s16_t fpgaRevision_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)get_fpga_fw_rev();
    return sizeof(*uint_ptr);
}

static const struct snmp_scalar_node fpgaDeviceId = SNMP_SCALAR_CREATE_NODE_READONLY(
        2, SNMP_ASN1_TYPE_UNSIGNED32, fpgaDeviceId_get_value);
static const struct snmp_scalar_node fpgaDeviceIdStr = SNMP_SCALAR_CREATE_NODE_READONLY(
        3, SNMP_ASN1_TYPE_OCTET_STRING, fpgaDeviceId_get_value);
static const struct snmp_scalar_node fpgaVersion = SNMP_SCALAR_CREATE_NODE_READONLY(
        4, SNMP_ASN1_TYPE_UNSIGNED32, fpgaVersion_get_value);
static const struct snmp_scalar_node fpgaRevision = SNMP_SCALAR_CREATE_NODE_READONLY(
        5, SNMP_ASN1_TYPE_UNSIGNED32, fpgaRevision_get_value);

// --- PLL

static s16_t pllStatus_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)(getPllStatus());
    return sizeof(*uint_ptr);
}

static const struct snmp_scalar_node pllStatus = SNMP_SCALAR_CREATE_NODE_READONLY(
        1, SNMP_ASN1_TYPE_INTEGER, pllStatus_get_value);

// --- BMC

static s16_t bmcStatus_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)getSystemStatus();
    return sizeof(*uint_ptr);
}

static const struct snmp_scalar_node bmcStatus = SNMP_SCALAR_CREATE_NODE_READONLY(
        1, SNMP_ASN1_TYPE_INTEGER, bmcStatus_get_value);

static const struct snmp_node* const supply_nodes[] = {
    &supplyStatus.node.node,
    &supplySensorCount.node.node,
    &supplySensors_node.node
};

static const struct snmp_node* const fpga_nodes[] = {
    &fpgaStatus.node.node,
    &fpgaDeviceId.node.node,
    &fpgaDeviceIdStr.node.node,
    &fpgaVersion.node.node,
    &fpgaRevision.node.node,
};

// --- TTVXS
static const struct snmp_node* const pll_nodes[] = {
    &pllStatus.node.node
};

static const struct snmp_tree_node pll_node = SNMP_CREATE_TREE_NODE(1, pll_nodes);

static const struct snmp_node* const sfp_nodes[] = {
    &sfp_table.node.node
};

static const struct snmp_tree_node sfp_node = SNMP_CREATE_TREE_NODE(2, sfp_nodes);

static const struct snmp_node* const ttvxs_nodes[] = {
    &pll_node.node,
    &sfp_node.node
};

static const struct snmp_tree_node system_node = SNMP_CREATE_TREE_NODE(2, system_nodes);

static const struct snmp_tree_node sysmon_node = SNMP_CREATE_TREE_NODE(3, supply_nodes);

static const struct snmp_tree_node fpga_node = SNMP_CREATE_TREE_NODE(4, fpga_nodes);

static const struct snmp_tree_node ttvxs_node = SNMP_CREATE_TREE_NODE(5, ttvxs_nodes);

static const struct snmp_node* const ipmiBoard_nodes[] = {
    &boards_table.node.node,
    &ipmiSensorTable.node.node
};

static const struct snmp_tree_node ipmiBoard_node = SNMP_CREATE_TREE_NODE(6, ipmiBoard_nodes);

static const struct snmp_node* const bmc_nodes[] = {
    &bmcStatus.node.node,
    &system_node.node,
    &sysmon_node.node,
    &fpga_node.node,
    &ttvxs_node.node,
    &ipmiBoard_node.node
};
static const struct snmp_tree_node bmc_node = SNMP_CREATE_TREE_NODE(1, bmc_nodes);


static const u32_t prvmib_base_oid[] = { 1,3,6,1,4,1,53776,120,1 };
const struct snmp_mib mib_private = SNMP_MIB_CREATE(prvmib_base_oid, &bmc_node.node);

#if 0
/* for reference: we could also have expressed it like this: */

/* lwip .1.3.6.1.4.1.26381 */
static const struct snmp_node* const lwip_nodes[] = {
    &example_node.node
};
static const struct snmp_tree_node lwip_node = SNMP_CREATE_TREE_NODE(26381, lwip_nodes);

/* enterprises .1.3.6.1.4.1 */
static const struct snmp_node* const enterprises_nodes[] = {
    &lwip_node.node
};
static const struct snmp_tree_node enterprises_node = SNMP_CREATE_TREE_NODE(1, enterprises_nodes);

/* private .1.3.6.1.4 */
static const struct snmp_node* const private_nodes[] = {
    &enterprises_node.node
};
static const struct snmp_tree_node private_root = SNMP_CREATE_TREE_NODE(4, private_nodes);

static const u32_t prvmib_base_oid[] = { 1,3,6,1,4 };
const struct snmp_mib mib_private = SNMP_MIB_CREATE(prvmib_base_oid, &private_root.node);
#endif

/**
 * Initialises this private MIB before use.
 * @see main.c
 */
void
lwip_privmib_init(void)
{
    //  u8_t i;
    //  memset(sensors, 0, sizeof(sensors));
    //  printf("SNMP private MIB start, detecting sensors.\n");
    //  for (i = 0; i < SENSOR_COUNT; i++) {
    //    sensors[i].num = (u8_t)(i + 1);
    //  }
}


#endif /* LWIP_SNMP */
