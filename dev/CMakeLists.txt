
file(GLOB Dev_SRCS
    ad9516/*
    ad9545/*
    digipot/*
    fpga/*
    powermon/*
    ttvxs_auxpll/*
)

target_include_directories(${ELF} PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}
    ad9516
    ad9545
    digipot
    fpga
    powermon
    ttvxs_auxpll
    ttvxs_clkmux
    ttvxs_vxsiic
)

target_sources(${ELF} PRIVATE
    ${Dev_SRCS}
)
