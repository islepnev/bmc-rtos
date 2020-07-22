
file(GLOB SRCS
    ad9516/*
    ad9545/*
    digipot/*
    fpga/*
    powermon/*
    ttvxs_auxpll/*
)

set_property(TARGET ${ELF} APPEND PROPERTY SOURCES ${SRCS})

target_include_directories(${ELF} PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}
    ad9516
    ad9545
    digipot
    fpga
    powermon
    ttvxs_auxpll
    ttvxs_clkmux
    vxsiicm
    vxsiics
)

target_include_directories(${ELF} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#target_sources(${ELF} PRIVATE
#    ${Dev_SRCS}
#)
