set(ENUM_HEADERS enum.h)

# Парметр ADDITIONAL_MODULES в rgen_process опционален. Необходим для добавления модулей,
# не являющихся модулями по умолчанию. Модули по умолчанию добавляются
# всегда. Здесь пример использования, ADDITIONAL_MODULES может быть убран в данном примере.
set(ADDITIONAL_MODULES Enum)

set(LIB_NAME mylib)
add_library(${LIB_NAME} OBJECT)
target_include_directories(${LIB_NAME} PRIVATE
        ${RGEN_DIR}/include
        )

rgen_process(${LIB_NAME} ENUM_HEADERS ADDITIONAL_MODULES)
