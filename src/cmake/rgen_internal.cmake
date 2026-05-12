# Локальный внутренний юнит-тест (не модули)
function(add_google_test TEST_NAME FILES )
    add_executable(${TEST_NAME} ${FILES})
    target_link_libraries(${TEST_NAME} PRIVATE gtest_main core clangTooling)
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endfunction()

function(rgen_test_generate TARGET_TEST_EXEC)
    # Файлы для генерации
    set(GEN_HEADERS_LIST "${${ARGV1}}")

    # Дополнительные модули, которые будут обработаны
    set(MODULES "${${ARGV2}}")

    # Является ли данный проект библиотекой
    if(ARGC GREATER_EQUAL 5)
        set(IS_LIBRARY ${ARGV4})
    else()
        set(IS_LIBRARY FALSE)
    endif()

    if(${IS_LIBRARY})
        add_library(${TARGET_TEST_EXEC} OBJECT EXCLUDE_FROM_ALL)
    else()
        add_executable(${TARGET_TEST_EXEC} EXCLUDE_FROM_ALL main.cpp)
    endif()
    set_property(GLOBAL APPEND PROPERTY ALL_TEST_TARGETS ${TARGET_TEST_EXEC})

    # Дополнительные файлы в тесте, для которых не нужна генерация
    if(ARGC GREATER_EQUAL 4)
        target_sources(${TARGET_TEST_EXEC} PRIVATE "${${ARGV3}}")
    endif()

    string(JOIN "," MODULES_COMMA ${MODULES})

    foreach(HEADER IN LISTS GEN_HEADERS_LIST)
        get_filename_component(name "${HEADER}" NAME_WE)
        set(GEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_TEST_EXEC}_gen)
        set(FILE_RGEN_CXX_DIR "${GEN_OUTPUT_DIR}/${name}.rgen.cxx")
        set(FILE_H_CXX_DIR "${GEN_OUTPUT_DIR}/${name}/${name}.h.cxx")
        set(EXPECTED_OUTPUT ${FILE_RGEN_CXX_DIR} ${FILE_H_CXX_DIR})
        get_filename_component(HEADER_ABS "${HEADER}" ABSOLUTE)

        set(GEN_CMD $<TARGET_FILE:redkit-gen>
            --db_dir=${CMAKE_BINARY_DIR}
            --additional_modules=${MODULES_COMMA}
            --output_dir=${GEN_OUTPUT_DIR}
            --input=${HEADER_ABS}
            # --verbose
            --debug_print_artifacts_with_header
        )

        string(REPLACE ";" " " GEN_CMD_DEBUG "${GEN_CMD}")

        add_custom_command(
          OUTPUT
            ${EXPECTED_OUTPUT}
          COMMAND
          ${GEN_CMD}
          || (cmake -E echo "ERROR: redkit-gen failed to generate a file for testing. Command:"
              && (cmake -E echo "${GEN_CMD_DEBUG}") && false)
          DEPENDS
            redkit-gen
          IMPLICIT_DEPENDS
            CXX ${HEADER_ABS}
          COMMENT
            "Trying to generate from ${HEADER}. Command: ${GEN_CMD_DEBUG}"
        )
        target_sources(${TARGET_TEST_EXEC} PUBLIC ${HEADER} ${EXPECTED_OUTPUT})
        target_include_directories(${TARGET_TEST_EXEC} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
    target_include_directories(${TARGET_TEST_EXEC}
        PUBLIC
            ${CMAKE_BINARY_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include
    )
    find_package(Qt5 COMPONENTS Core REQUIRED)
    target_link_libraries(${TARGET_TEST_EXEC} PRIVATE gtest_main core clangTooling Qt5::Core ${MODULES})

    # Если функция применяется для библиотеки, то её не нужно добавлять в тесты
    if(NOT ${IS_LIBRARY})
        include(GoogleTest)
        gtest_discover_tests(${TARGET_TEST_EXEC})
    endif()
endfunction()

# Вызывается в корне проекта модуля. Подразумевается что есть директория export_includes
function(export_module_include MODULE_NAME)
    set(EXPORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export_includes")

    # Экспорт include
    export_common(
           MODULE_NAME ${MODULE_NAME}
           EXPORT_DIR ${EXPORT_DIR}
           DEST_SUBDIR "include/rgen"
           TARGET_SUFFIX "_include"
           PATTERN "*.h"
    )

    # Копируем для тестов в бинарную директорию (только для include)
    set(RGEN_MODULE_DIR "${CMAKE_BINARY_DIR}/include/rgen")
    file(COPY "${EXPORT_DIR}/" DESTINATION ${RGEN_MODULE_DIR})
endfunction()

# Вызывается в корне проекта модуля. Подразумевается что есть директория template
function(export_module_template MODULE_NAME)
    set(EXPORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/template")

    # Экспорт template
    export_common(
           MODULE_NAME ${MODULE_NAME}
           EXPORT_DIR ${EXPORT_DIR}
           DEST_SUBDIR "rgen-template"
           TARGET_SUFFIX "_template"
           PATTERN "*.j2"
    )
endfunction()

# Общая функция для экспорта файлов модуля
# MODULE_NAME - имя модуля
# EXPORT_DIR - директория с исходными файлами для экспорта
# DEST_SUBDIR - поддиректория относительно CMAKE_INSTALL_PREFIX
# TARGET_SUFFIX - суффикс для имени custom target
# PATTERN - шаблон для поиска файлов
function(export_common)
    # Список ключей нужен для парсинга именнованных параметров
    set(ONE_VALUE_ARGS MODULE_NAME EXPORT_DIR DEST_SUBDIR TARGET_SUFFIX PATTERN)
    cmake_parse_arguments(EXPORT_COMMON "" "${ONE_VALUE_ARGS}" "" ${ARGN})

    if (NOT EXISTS ${EXPORT_COMMON_EXPORT_DIR})
        message( WARNING "directory ${EXPORT_COMMON_EXPORT_DIR} does not exist" )
    endif()

    set(DST_DIR ${CMAKE_INSTALL_PREFIX}/${EXPORT_COMMON_DEST_SUBDIR})
    file(GLOB_RECURSE FILES "${EXPORT_COMMON_EXPORT_DIR}/${EXPORT_COMMON_PATTERN}")
    add_custom_target("${EXPORT_COMMON_MODULE_NAME}${EXPORT_COMMON_TARGET_SUFFIX}" SOURCES ${FILES})

    install(DIRECTORY "${EXPORT_COMMON_EXPORT_DIR}/"
            DESTINATION "${DST_DIR}"
            FILES_MATCHING
            PATTERN "${EXPORT_COMMON_PATTERN}")
endfunction()


function(module_configure_target MODULE_NAME)

    target_compile_definitions(${MODULE_NAME} PRIVATE
        RGEN_TEMPLATE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/template"
    )

    target_link_libraries(${MODULE_NAME} PRIVATE
        fmt::fmt-header-only
        spdlog::spdlog
        hjson
    )

endfunction()

