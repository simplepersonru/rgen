set(LIB_NAME someLib)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(COMMON_INCLUDES
    ${CMAKE_SOURCE_DIR}/src/thirdparty/include/jsoncons/include
    ${CMAKE_SOURCE_DIR}/src/modules/_test_utils/fakeInclude
    ${CMAKE_BINARY_DIR}/include
)

set(MODULES Enum JsonSerializer Derive Hasher QHasher JsonSchema Json OperatorEquals OperatorLess OperatorQStream QMetaTypesRegistrator QVariantMapSerializer)

add_subdirectory(someLib)

set(TEST_PROJECT_NAME_OBJECT library_depends_object_test)
set(TEST_PROJECT_NAME_STATIC library_depends_static_test)
set(TEST_PROJECT_NAME_SHARED library_depends_shared_test)

# Object
set(PROJECT_OBJECT_FILES $<TARGET_OBJECTS:${LIB_NAME}>)
rgen_test_generate(${TEST_PROJECT_NAME_OBJECT} LIB_HEADERS MODULES PROJECT_OBJECT_FILES)
target_link_libraries(${TEST_PROJECT_NAME_OBJECT} PRIVATE gtest_main core Qt5::Core ${MODULES} clangTooling)
target_include_directories(${TEST_PROJECT_NAME_OBJECT} PRIVATE ${COMMON_INCLUDES})
gtest_discover_tests(${TEST_PROJECT_NAME_OBJECT})

# Static
rgen_test_generate(${TEST_PROJECT_NAME_STATIC} LIB_HEADERS MODULES)
target_link_libraries(${TEST_PROJECT_NAME_STATIC} PRIVATE gtest_main core Qt5::Core ${MODULES} clangTooling ${LIB_NAME}_static)
target_include_directories(${TEST_PROJECT_NAME_STATIC} PRIVATE ${COMMON_INCLUDES})
gtest_discover_tests(${TEST_PROJECT_NAME_STATIC})

# Shared
rgen_test_generate(${TEST_PROJECT_NAME_SHARED} LIB_HEADERS MODULES)
target_link_libraries(${TEST_PROJECT_NAME_SHARED} PRIVATE gtest_main core Qt5::Core ${MODULES} clangTooling ${LIB_NAME}_shared)
target_include_directories(${TEST_PROJECT_NAME_SHARED} PRIVATE ${COMMON_INCLUDES})
gtest_discover_tests(${TEST_PROJECT_NAME_SHARED})
