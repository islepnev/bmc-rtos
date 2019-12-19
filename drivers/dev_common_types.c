
#include "dev_common_types.h"
#include "display.h"

const char *sensor_status_ansi_str(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return STR_RESULT_WARNING;
    case SENSOR_NORMAL:   return STR_RESULT_NORMAL;
    case SENSOR_WARNING:  return STR_RESULT_WARNING;
    case SENSOR_CRITICAL: return STR_RESULT_CRIT;
    default: return STR_RESULT_FAIL;
    }
}

//const char *sensor_status_text(SensorStatus state)
//{
//    switch(state) {
//    case SENSOR_UNKNOWN:  return "  UNKNOWN";
//    case SENSOR_NORMAL:   return "  NORMAL";
//    case SENSOR_WARNING:  return " WARNING";
//    case SENSOR_CRITICAL: return "CRITICAL";
//    default: return "FAIL";
//    }
//}
