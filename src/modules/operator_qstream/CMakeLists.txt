include_directories(${LLVM_INCLUDE_DIRS})

set(MODULE_NAME OperatorQStream)

add_library(${MODULE_NAME} OBJECT
    api.h
    printer.cpp
    register.cpp
    converter.cpp
)

module_configure_target(${MODULE_NAME})

add_subdirectory(test)

export_module_include(${MODULE_NAME})
export_module_template(${MODULE_NAME})
