
#include "dev_common_types.h"

#include <assert.h>

#include "display.h"


const char *sensor_status_ansi_str(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return STR_RESULT_UNKNOWN;
    case SENSOR_NORMAL:   return STR_RESULT_NORMAL;
    case SENSOR_WARNING:  return STR_RESULT_WARNING;
    case SENSOR_CRITICAL: return STR_RESULT_CRIT;
    default: return STR_RESULT_FAIL;
    }
}

const char *sensor_status_text(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return " UNKNOWN";
    case SENSOR_NORMAL:   return "  NORMAL";
    case SENSOR_WARNING:  return " WARNING";
    case SENSOR_CRITICAL: return "CRITICAL";
    default: return "FAIL";
    }
}

char *deviceStatusResultStr(DeviceStatus status)
{
    switch (status) {
    case DEVICE_UNKNOWN:
        return STR_RESULT_UNKNOWN;
    case DEVICE_NORMAL:
        return STR_RESULT_NORMAL;
    case DEVICE_FAIL:
        return STR_RESULT_FAIL;
    default:
        return "???";
    }
}

void create_device(DeviceBase *d, void *priv, DeviceClass class, const BusInterface bus)
{
    d->class = class;
    d->bus = bus;
    d->priv = priv;
    if (deviceList.count >= DEVICE_LIST_SIZE) {
        return;
    }
    deviceList.list[deviceList.count++] = d;
}

DeviceBase *find_device(DeviceClass class)
{
    for (int i=0; i<deviceList.count; i++) {
        if (deviceList.list[i]->class == class) {
            assert(deviceList.list[i]);
            return deviceList.list[i];
        }
    }
    assert(0);
    return 0;
}

const DeviceBase *find_device_const(DeviceClass class)
{
    return find_device(class);
}

void *device_priv(DeviceBase *d)
{
    assert(d);
    assert(d->priv);
    return d->priv;
}

const void *device_priv_const(const DeviceBase *d)
{
    assert(d);
    assert(d->priv);
    return d->priv;
}

