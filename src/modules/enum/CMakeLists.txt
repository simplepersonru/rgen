include_directories(${LLVM_INCLUDE_DIRS})

set(MODULE_NAME Enum)

set(RGEN_HEADERS data.h)

add_library(${MODULE_NAME} OBJECT
    api.h
    registry.cpp
    printer.cpp
    converter.cpp
    ${RGEN_HEADERS}
)

module_configure_target(${MODULE_NAME})

add_subdirectory(test)

export_module_include(${MODULE_NAME})
export_module_template(${MODULE_NAME})
