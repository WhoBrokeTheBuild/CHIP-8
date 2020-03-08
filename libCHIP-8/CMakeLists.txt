
# libCHIP-8

FILE(
    GLOB_RECURSE
    _SOURCES
    src/*.h
    src/*.c
)

ADD_LIBRARY(
    libCHIP-8
    ${_SOURCES}
)

SET_TARGET_PROPERTIES(
    libCHIP-8 PROPERTIES
    OUTPUT_NAME libCHIP-8
)

TARGET_INCLUDE_DIRECTORIES(
    libCHIP-8
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/CHIP-8>
        $<INSTALL_INTERFACE:include>
)

# Tests

IF(BUILD_TESTS)
    FILE(GLOB_RECURSE
        _TESTS
        "tests/*_test.c"
    )

    FOREACH(test_source ${_TESTS})
        GET_FILENAME_COMPONENT(test_target ${test_source} NAME_WE)
        
        ADD_EXECUTABLE(
            ${test_target}
            ${test_source}
        )

        TARGET_LINK_LIBRARIES(
            ${test_target}
            PRIVATE
                libCHIP-8
        )

        ADD_TEST(
            NAME ${test_target}
            COMMAND $<TARGET_FILE:${test_target}>
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        )
    ENDFOREACH()
ENDIF()
