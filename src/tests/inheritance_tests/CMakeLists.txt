project(Inheritance_tests)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "*.h")

set(ALL_MODULES
    Derive
    OperatorEquals
    OperatorLess
    OperatorQStream
    Json
    QHasher
    Hasher
    QVariantMapSerializer
)

rgen_test_generate(Inheritance_tests HEADER_FILES ALL_MODULES)

target_include_directories(Inheritance_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/thirdparty/include/jsoncons/include
    ${CMAKE_SOURCE_DIR}/src/modules/_test_utils/fakeInclude
)
