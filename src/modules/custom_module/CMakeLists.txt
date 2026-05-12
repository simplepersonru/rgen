include_directories(${LLVM_INCLUDE_DIRS})

set(MODULE_NAME CustomModule)

add_library(${MODULE_NAME} OBJECT
    api.h
    register.cpp
    data.h
    data.cpp
    printer.cpp
    converter.cpp
)

module_configure_target(${MODULE_NAME})
