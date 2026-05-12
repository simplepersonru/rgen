enable_testing()

set(LIB_HEADERS liby.h liby_global.h)

rgen_test_generate(${LIB_NAME} LIB_HEADERS MODULES LIB_HEADERS TRUE)

# Object
target_link_libraries(${LIB_NAME} PRIVATE Qt5::Core ${MODULES})
target_include_directories(${LIB_NAME} PRIVATE ${COMMON_INCLUDES})
set_target_properties(${LIB_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${LIB_NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN hidden)

# Static
add_library(${LIB_NAME}_static STATIC EXCLUDE_FROM_ALL $<TARGET_OBJECTS:${LIB_NAME}>)
set_property(GLOBAL APPEND PROPERTY ALL_TEST_TARGETS ${LIB_NAME}_static)
set_target_properties(${LIB_NAME}_static PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${LIB_NAME}_static PROPERTIES VISIBILITY_INLINES_HIDDEN hidden)

# Shared
add_library(${LIB_NAME}_shared SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:${LIB_NAME}>)
set_property(GLOBAL APPEND PROPERTY ALL_TEST_TARGETS ${LIB_NAME}_shared)
set_target_properties(${LIB_NAME}_shared PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${LIB_NAME}_shared PROPERTIES VISIBILITY_INLINES_HIDDEN hidden)
