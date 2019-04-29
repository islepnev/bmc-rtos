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

#include "snmp_ipmi_sensors_table.h"
#include <string.h>

#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_scalar.h"
#include "lwip/apps/snmp_table.h"
#include "dev_common_types.h"
#include "ipmi_sensor_types.h"
#include "dev_pm_sensors_config.h"
#include "dev_vxsiic_types.h"
#include "devices_types.h"
#include "app_shared_data.h"

//static s16_t scalar_node_get_value(struct snmp_node_instance* instance, void* value)
//{
//    LWIP_UNUSED_ARG(instance);
//    u32_t *uint_ptr = (u32_t*)value;
//    *uint_ptr = (u32_t)12345;
//    return sizeof(*uint_ptr);
//}

//void snmp_ipmi_sensors_table_init(void)
//{
//    struct snmp_tree_node value_node;
//    value_node.node.node_type = SNMP_NODE_SCALAR;
//    value_node.node.oid = 1;
//    value_node.subnode_count = 0;
//    value_node.subnodes = NULL;

//    struct snmp_scalar_node scalar_node;
//    scalar_node.node.node.node_type = SNMP_NODE_SCALAR;
//    scalar_node.node.node.oid = 1;
//    scalar_node.node.get_instance = snmp_scalar_get_instance;
//    scalar_node.node.get_next_instance = snmp_scalar_get_next_instance;
//    scalar_node.asn1_type = SNMP_NODE_SCALAR;
//    scalar_node.access = SNMP_NODE_INSTANCE_READ_ONLY;
//    scalar_node.get_value = scalar_node_get_value;
//    scalar_node.set_test = NULL;
//    scalar_node.set_value = NULL;

////    struct snmp_tree_node tree_node = SNMP_CREATE_TREE_NODE(26381, lwip_nodes);

//}

/* --- ipNetToMediaTable --- */

/* list of allowed value ranges for incoming OID */
static const struct snmp_oid_range ipmiSensorTable_oid_ranges[] = {
{ 1, 2 }, // IpmiSensorEntry size
{ 1, VXSIIC_SLOTS }, // boardIndex
{ 1, POWERMON_SENSORS }, // sensorIndex
};

static snmp_err_t
ipmiSensorTable_get_cell_value_core(const struct GenericSensor *sensor_ptr, const u32_t *column, union snmp_variant_value *value, u32_t *value_len)
{
    /* value */
    switch (*column) {
//    case 1: // ipmiSensorIndex (not-accessible)
//        value->u32 = 1;
//        break;
    case 2: { // ipmiSensorName
        size_t len = strlen(sensor_ptr->name);
        MEMCPY(value, sensor_ptr->name, len);
        *value_len = len;
        break;
    }
    default:
        return SNMP_ERR_NOSUCHINSTANCE;
    }
    return SNMP_ERR_NOERROR;
}

static snmp_err_t
ipmiSensorTable_get_cell_value(const u32_t *column, const u32_t *row_oid, u8_t row_oid_len, union snmp_variant_value *value, u32_t *value_len)
{
  /* check if incoming OID length and if values are in plausible range */
  if (!snmp_oid_in_range(row_oid, row_oid_len, ipmiSensorTable_oid_ranges, LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges))) {
    return SNMP_ERR_NOSUCHINSTANCE;
  }

  /* get indices from incoming OID */
  u8_t board_index = (u8_t)row_oid[0];
  u8_t sensor_index = (u8_t)row_oid[1];

  if (board_index == 0 || board_index > VXSIIC_SLOTS)
      return SNMP_ERR_NOSUCHINSTANCE;
  if (sensor_index == 0 || sensor_index > MAX_SENSOR_COUNT)
      return SNMP_ERR_NOSUCHINSTANCE;

  const Devices *dev = getDevicesConst();
  const struct GenericSensor *sensor_ptr = &dev->vxsiic.status.slot[board_index-1].sensors[sensor_index-1];

  return ipmiSensorTable_get_cell_value_core(sensor_ptr, column, value, value_len);
}

static snmp_err_t
ipmiSensorTable_get_next_cell_instance_and_value(const u32_t *column, struct snmp_obj_id *row_oid, union snmp_variant_value *value, u32_t *value_len)
{
  struct snmp_next_oid_state state;
  u32_t result_temp[LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges)];

  /* init struct to search next oid */
  snmp_next_oid_init(&state, row_oid->id, row_oid->len, result_temp, LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges));

  /* iterate over all possible OIDs to find the next one */
  Devices *dev = getDevices();
  for (size_t i = 0; i < VXSIIC_SLOTS; i++)
      for (size_t j = 0; j < MAX_SENSOR_COUNT; j++)
      {
          struct GenericSensor *sensor_ptr = &dev->vxsiic.status.slot[i].sensors[j];
          u32_t test_oid[LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges)];
          test_oid[0] = i+1;
          test_oid[1] = j+1;
          /* check generated OID: is it a candidate for the next one? */
          snmp_next_oid_check(&state, test_oid, LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges), sensor_ptr);
      }

  /* did we find a next one? */
  if (state.status == SNMP_NEXT_OID_STATUS_SUCCESS) {
    snmp_oid_assign(row_oid, state.next_oid, state.next_oid_len);
    /* fill in object properties */
    return ipmiSensorTable_get_cell_value_core((const struct GenericSensor *)(state.reference), column, value, value_len);
  }

  /* not found */
  return SNMP_ERR_NOSUCHINSTANCE;
}

static const struct snmp_table_simple_col_def ipmiSensorTable_columns[] = {
  {  1, SNMP_ASN1_TYPE_INTEGER,      SNMP_VARIANT_VALUE_TYPE_U32 }, // sensorIndex
  {  2, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_VARIANT_VALUE_TYPE_CONST_PTR } // sensorName
};

const struct snmp_table_simple_node ipmiSensorTable =
        SNMP_TABLE_CREATE_SIMPLE(1, ipmiSensorTable_columns, ipmiSensorTable_get_cell_value, ipmiSensorTable_get_next_cell_instance_and_value);
