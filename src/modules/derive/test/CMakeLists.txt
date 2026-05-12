set(TEST_PROJECT_NAME ${MODULE_NAME}_test)

set(MODULES ${MODULE_NAME} OperatorEquals OperatorLess OperatorQStream Json Hasher QHasher QVariantMapSerializer)

file(GLOB_RECURSE GEN_HEADERS CONFIGURE_DEPENDS "*.h")

rgen_test_generate(${TEST_PROJECT_NAME} GEN_HEADERS MODULES)

target_include_directories(${TEST_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/thirdparty/include/jsoncons/include
)

include_directories(../../_test_utils/fakeInclude)
