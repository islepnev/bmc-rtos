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
#include "vxsiicm/dev_vxsiicm_types.h"
#include "devices_types.h"
#include "app_shared_data.h"

/* list of allowed value ranges for incoming OID */
static const struct snmp_oid_range ipmiSensorTable_oid_ranges[] = {
{ 1, VXSIIC_SLOTS }, // boardIndex
{ 1, MAX_SENSOR_COUNT }, // sensorIndex
};

static u32_t encode_index(u32_t board_index, u32_t sensor_index)
{
    return  (board_index << 8) | sensor_index;
}

static u8_t decode_board_index(u32_t index)
{
    return index >> 8;
}

static u8_t decode_sensor_index(u32_t index)
{
    return index & 0xFF;
}

static snmp_err_t
ipmiSensorTable_get_cell_value_core(u32_t board_index, u32_t sensor_index, const u32_t *column, union snmp_variant_value *value, u32_t *value_len)
{
//    u8_t board_index = info->boardIndex;
//    u8_t sensor_index = info->sensorIndex;

    if (board_index == 0 || board_index > VXSIIC_SLOTS)
        return SNMP_ERR_NOSUCHINSTANCE;
    if (sensor_index == 0 || sensor_index > MAX_SENSOR_COUNT)
        return SNMP_ERR_NOSUCHINSTANCE;

    const Devices *dev = getDevicesConst();
    const struct GenericSensor *sensor_ptr = &dev->vxsiicm.status.slot[board_index-1].mcu_sensors.sensors[sensor_index-1];

    /* value */
    switch (*column) {
    case 1: // ipmiSensorIndex
        value->u32 = encode_index(board_index, sensor_index);
        *value_len = sizeof(u32_t);
        break;
    case 2: { // ipmiSensorName
        size_t len = strlen(sensor_ptr->name);
        MEMCPY(value->ptr, sensor_ptr->name, len);
        *value_len = len;
        break;
    }
    case 3: { // ipmiSensorType
        value->u32 = sensor_ptr->hdr.b.type;
        *value_len = sizeof(u32_t);
        break;
    }
    case 4: { // ipmiSensorStatus
        value->u32 = sensor_ptr->hdr.b.state;
        *value_len = sizeof(u32_t);
        break;
    }
    case 5: { // ipmiSensorValue
        value->u32 = sensor_ptr->value * 1000;
        *value_len = sizeof(u32_t);
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
  const u8_t board_index = (u8_t)row_oid[0];
  const u8_t sensor_index = (u8_t)row_oid[1];

  if (board_index == 0 || board_index > VXSIIC_SLOTS)
      return SNMP_ERR_NOSUCHINSTANCE;
  if (sensor_index == 0 || sensor_index > MAX_SENSOR_COUNT)
      return SNMP_ERR_NOSUCHINSTANCE;

  return ipmiSensorTable_get_cell_value_core(board_index, sensor_index, column, value, value_len);
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
  for (size_t i = 0; i < VXSIIC_SLOTS; i++) {
//      u32_t boardIndex = (u32_t)vxsiic_map_slot_to_number[i];
      if (!dev->vxsiicm.status.slot[i].present)
          continue;
      for (size_t j = 0; j < MAX_SENSOR_COUNT; j++)
      {
          struct GenericSensor *sensor_ptr = &dev->vxsiicm.status.slot[i].mcu_sensors.sensors[j];
          if (sensor_ptr->hdr.b.state == SENSOR_UNKNOWN) continue;
          u32_t test_oid[LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges)];
          const u8_t board_index = i+1;
          const u8_t sensor_index = j+1;
          test_oid[0] = board_index;
          test_oid[1] = sensor_index;
          /* check generated OID: is it a candidate for the next one? */
          snmp_next_oid_check(&state, test_oid, LWIP_ARRAYSIZE(ipmiSensorTable_oid_ranges), (void *)encode_index(board_index, sensor_index));
      }
  }

  /* did we find a next one? */
  if (state.status == SNMP_NEXT_OID_STATUS_SUCCESS) {
    snmp_oid_assign(row_oid, state.next_oid, state.next_oid_len);
    /* fill in object properties */
    return ipmiSensorTable_get_cell_value_core(decode_board_index((uint32_t)state.reference), decode_sensor_index((uint32_t)state.reference), column, value, value_len);
  }

  /* not found */
  return SNMP_ERR_NOSUCHINSTANCE;
}

static const struct snmp_table_simple_col_def ipmiSensorTable_columns[] = {
  {  1, SNMP_ASN1_TYPE_INTEGER,      SNMP_VARIANT_VALUE_TYPE_U32 }, // sensorIndex
  {  2, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_VARIANT_VALUE_TYPE_CONST_PTR }, // sensorName
  {  3, SNMP_ASN1_TYPE_INTEGER,      SNMP_VARIANT_VALUE_TYPE_U32 }, // sensorType
  {  4, SNMP_ASN1_TYPE_INTEGER,      SNMP_VARIANT_VALUE_TYPE_U32 }, // sensorStatus
  {  5, SNMP_ASN1_TYPE_INTEGER,      SNMP_VARIANT_VALUE_TYPE_U32 }, // sensorValue
};

const struct snmp_table_simple_node ipmiSensorTable =
        SNMP_TABLE_CREATE_SIMPLE(2, ipmiSensorTable_columns, ipmiSensorTable_get_cell_value, ipmiSensorTable_get_next_cell_instance_and_value);
