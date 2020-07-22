
file(GLOB_RECURSE Src_SRCS
    *.h
    *.c
)

target_include_directories(${ELF} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${ELF} PUBLIC
    ${Src_SRCS}
)
