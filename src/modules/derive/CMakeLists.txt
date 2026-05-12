include_directories(${LLVM_INCLUDE_DIRS})

set(MODULE_NAME Derive)

add_library(${MODULE_NAME} OBJECT
    api.h
    registry.cpp
    converter.cpp
)

module_configure_target(${MODULE_NAME})

add_subdirectory(test)

export_module_include(${MODULE_NAME})

target_link_libraries(${MODULE_NAME} PRIVATE OperatorEquals OperatorLess OperatorQStream Json QHasher Hasher QVariantMapSerializer)

