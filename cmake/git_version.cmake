find_package (Git)

if (GIT_FOUND)
    # Get description
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --match [0-9]*.[0-9]* --long --dirty --always
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_COMMAND_RESULT
        )
    if (GIT_COMMAND_RESULT EQUAL 0)
        message(STATUS "Version string: ${GIT_DESCR}")
    else()
        message(WARNING "Unable to find version tag matching [0-9]*.[0-9]*")
    endif()
endif()


if (GIT_FOUND AND GIT_COMMAND_RESULT EQUAL 0)

    # Split into a list of numbers
    string(REGEX MATCHALL "[0-9]+" ${GIT_DESCR}_LIST
        ${GIT_DESCR}
        )
    list(GET ${GIT_DESCR}_LIST
        0 GIT_TAG_MAJOR_NUM)
    list(GET ${GIT_DESCR}_LIST
        1 GIT_TAG_MINOR_NUM)
    list(GET ${GIT_DESCR}_LIST
        2 GIT_COMMIT_COUNT)

    message(STATUS "Version parsed: ${GIT_TAG_MAJOR_NUM}, ${GIT_TAG_MINOR_NUM}, ${GIT_COMMIT_COUNT}")

#    # Get the latest tag matching x.y
#    execute_process(
#        COMMAND ${GIT_EXECUTABLE} describe --tags --match [0-9]*.[0-9]* --abbrev=0
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        OUTPUT_VARIABLE GIT_TAG_VERSION
#        OUTPUT_STRIP_TRAILING_WHITESPACE
#        )
#    message("GIT_TAG_VERSION=${GIT_TAG_VERSION}")

#    # Get number of commits since tag
#    execute_process(
#        COMMAND ${GIT_EXECUTABLE} rev-list ${GIT_TAG_VERSION}..HEAD --count
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#        OUTPUT_VARIABLE GIT_COMMIT_COUNT
#        OUTPUT_STRIP_TRAILING_WHITESPACE
#        )
#    message("GIT_COMMIT_COUNT=${GIT_COMMIT_COUNT}")

else()
    message(WARNING "Unable to get version from git tags")
    set(GIT_DESCR unversioned)
    set(GIT_TAG_MAJOR_NUM 0)
    set(GIT_TAG_MINOR_NUM 0)
    set(GIT_COMMIT_COUNT 0)
endif()
