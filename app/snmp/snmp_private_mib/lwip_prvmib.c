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

#include "lwip/apps/snmp_table.h"
#include "lwip/apps/snmp_scalar.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "devices_types.h"
#include "dev_pm_sensors_config.h"
#include "dev_pm_sensors_types.h"
#include "snmp_powermon_table.h"

/*
  This example presents a table for a few (at most 10) sensors.
  Sensor detection takes place at initialization (once only).
  Sensors may and can not be added or removed after agent
  has started. Note this is only a limitation of this crude example,
  the agent does support dynamic object insertions and removals.
*/

static s16_t      sensor_count_get_value(struct snmp_node_instance* instance, void* value);

/* sensorcount .1.3.6.1.4.1.26381.1.2 */
static const struct snmp_scalar_node sensor_count = SNMP_SCALAR_CREATE_NODE_READONLY(
  2, SNMP_ASN1_TYPE_INTEGER, sensor_count_get_value);


/* example .1.3.6.1.4.1.26381.1 */
static const struct snmp_node* const example_nodes[] = {
    &sensor_table.node.node,
    &sensor_count.node.node
};
static const struct snmp_tree_node example_node = SNMP_CREATE_TREE_NODE(1, example_nodes);

static const u32_t prvmib_base_oid[] = { 1,3,6,1,4,1,26381,120,1,1 };
const struct snmp_mib mib_private = SNMP_MIB_CREATE(prvmib_base_oid, &example_node.node);

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

/* sensorcount .1.3.6.1.4.1.26381.1.2 */
static s16_t
sensor_count_get_value(struct snmp_node_instance* instance, void* value)
{
//  size_t count = 0;
  u32_t *uint_ptr = (u32_t*)value;

  LWIP_UNUSED_ARG(instance);

  *uint_ptr = (u32_t)POWERMON_SENSORS;
  return sizeof(*uint_ptr);

//  for(count=0; count<LWIP_ARRAYSIZE(sensors); count++) {
//    if(sensors[count].num == 0) {
//      *uint_ptr = (u32_t)count;
//      return sizeof(*uint_ptr);
//    }
//  }

  return 0;
}


#endif /* LWIP_SNMP */
