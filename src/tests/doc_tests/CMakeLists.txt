file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "*.h")

set(MODULES
    Enum
    Derive
    OperatorEquals
    OperatorLess
    OperatorQStream
    Json
    QHasher
    Hasher
    QVariantMapSerializer
)

rgen_test_generate(doc_tests HEADER_FILES MODULES)

target_include_directories(doc_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/thirdparty/include/jsoncons/include
    ${CMAKE_SOURCE_DIR}/src/modules/_test_utils/fakeInclude
)

target_link_libraries(doc_tests PRIVATE OperatorEquals OperatorLess OperatorQStream Json QHasher Hasher)
