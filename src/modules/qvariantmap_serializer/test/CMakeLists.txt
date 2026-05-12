set(TEST_PROJECT_NAME ${MODULE_NAME}_test)

set(MODULES ${MODULE_NAME} OperatorEquals)

set(PROJECT_FILES qvariantmap_myown.h private/ownLib.h)

set(GEN_HEADERS structs.h)

rgen_test_generate(${TEST_PROJECT_NAME} GEN_HEADERS MODULES PROJECT_FILES)

# Используем библиотеку из Json
target_link_libraries(${TEST_PROJECT_NAME} PRIVATE extLib)

