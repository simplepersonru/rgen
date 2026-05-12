set(TEST_PROJECT_NAME ${MODULE_NAME}_test)

set(PROJECT_FILES json_myown.h ownLib.h)

set(MODULES ${MODULE_NAME} OperatorEquals)

set(GEN_HEADERS structs.h)

add_subdirectory(externalLib)

rgen_test_generate(${TEST_PROJECT_NAME} GEN_HEADERS MODULES PROJECT_FILES)

target_include_directories(${TEST_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/thirdparty/include/jsoncons/include
)

target_link_libraries(${TEST_PROJECT_NAME} PRIVATE extLib)


