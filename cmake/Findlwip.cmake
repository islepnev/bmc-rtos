### lwIP
include(${LWIP_DIR}/src/Filelists.cmake)
#include(${LWIP_CONTRIB_DIR}/Filelists.cmake)
SET(LWIP_SOURCES
   ${lwipcore_SRCS}
   ${lwipcore4_SRCS}
   ${lwipapi_SRCS}
   ${lwipnetif_SRCS}
   ${lwipsntp_SRCS}
   ${lwipsnmp_SRCS}
   ${MY_SRC_DIR}/system/OS/sys_arch.c
)

FIND_PATH(LWIP_INCLUDE_DIRS lwip/opt.h
    PATH_SUFFIXES include
    HINTS ${LWIP_DIR}/src/include
    CMAKE_FIND_ROOT_PATH_BOTH
)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(lwip DEFAULT_MSG LWIP_INCLUDE_DIRS LWIP_SOURCES)
message("lwip_FOUND=${CMSIS_FOUND}")
