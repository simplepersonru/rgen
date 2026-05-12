find_package(Qt5 COMPONENTS Core)
find_package(Python REQUIRED COMPONENTS Development Interpreter)

message(STATUS "Found Python ${Python_VERSION}")
message(STATUS "Found QT ${Qt5_VERSION}")

if(NOT DEFINED RGEN_DIR)
    message(FATAL_ERROR "Задайте RGEN_DIR до директории redkit-gen-intergration")
endif()

set(TEMPLATE_DIR "${RGEN_DIR}/rgen-template")
set(MODULE_EXTENSIONS_PATH "${RGEN_DIR}/cmake/ModuleExtensions.json")
set(PY_COMPILER_COMMANDS_GENERATOR_PATH "${RGEN_DIR}/cmake/get_cc.py")

cmake_file_api(
    QUERY
    API_VERSION 1
    CODEMODEL 2.3
    TOOLCHAINS 1
)

include(${RGEN_DIR}/cmake/JSONParser.cmake)

function(get_rgen_binary RGEN_BIN_EXTERN)
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYSTEM_LOW_STRING)
    set(RGEN_BIN_LOCAL_TMP "${RGEN_DIR}/util/${SYSTEM_LOW_STRING}/bin/redkit-gen")

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(RGEN_BIN_LOCAL_TMP "${RGEN_BIN_LOCAL_TMP}.exe")
    endif()

    # Нормализуем путь (убираем /../../)
    get_filename_component(RGEN_BIN_LOCAL "${RGEN_BIN_LOCAL_TMP}" REALPATH)

    # Проверка на существование подобного файла
    if(NOT EXISTS ${RGEN_BIN_LOCAL})
        message(FATAL_ERROR "RGEN_BIN_LOCAL error: ${RGEN_BIN_LOCAL}")
    endif()

    set(${RGEN_BIN_EXTERN} "${RGEN_BIN_LOCAL}" PARENT_SCOPE)
endfunction()

function(rgen_includes_and_link TARGET_LIB)
    # Инклюды для работы redkit-gen
    set(SERVICE_INCLUDE "${RGEN_DIR}/util/include")
    target_include_directories(${TARGET_LIB} PUBLIC "${SERVICE_INCLUDE}")

    # Инклюды текущей библиотеки
    get_filename_component(INCL_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
    target_include_directories(${TARGET_LIB} PUBLIC ${INCL_DIR})

    if(Qt5_FOUND)
            target_link_libraries(${TARGET_LIB} PRIVATE Qt5::Core)
    endif()
endfunction()

function(add_python_lib LIBRARY_NAME)
    target_include_directories(${LIBRARY_NAME} PRIVATE ${Python_INCLUDE_DIRS})
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${Python_LIBRARY_RELEASE})

    # Дополнительный макросы, которые нужны для Windows
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(WIN_DEFINES
            _ICC
            _INTEL_COMPILER )

        # Добавить макросы только для дебаг режима
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            list(APPEND WIN_DEFINES
                 _DEBUG
                 SWIG_PYTHON_INTERPRETER_NO_DEBUG )
        endif()

        target_compile_definitions(${LIBRARY_NAME} PRIVATE ${WIN_DEFINES})
    endif()
endfunction()


# TODO распилить бы эту функцию на несколько поменьше
function(rgen_process TARGET_LIB)

    # RGEN_HEADERS - файлы для генерации
    # RGEN_ADDITIONAL_MODULES - дополнительные модули для обработки. Обрабатываются модули по умолчанию
    # и модули, перечисленные в этой переменной
    if(ARGC EQUAL 1)
        # Если передали только TARGET_LIB, то подключаем только инклюды и зависимости
        rgen_includes_and_link(${TARGET_LIB})
        return()
    elseif(ARGC EQUAL 2)
        set(RGEN_HEADERS "${${ARGV1}}")
    elseif(ARGC EQUAL 3)
        set(RGEN_HEADERS "${${ARGV1}}")
        set(RGEN_ADDITIONAL_MODULES "${${ARGV2}}")
    else()
        message(FATAL_ERROR "rgen_process: expected 1, 2 or 3 arguments, got ${ARGC}")
    endif()

    if(NOT RGEN_HEADERS)
        message(FATAL_ERROR "RGEN_HEADERS variable is not set")
    endif()

    target_sources(${TARGET_LIB} PRIVATE ${RGEN_HEADERS})

    get_rgen_binary(RGEN_BIN)
    get_filename_component(TARGET_STEM ${CMAKE_CURRENT_BINARY_DIR} NAME_WLE)

    # Из списка получаем строку разделенную запятой
    if(RGEN_ADDITIONAL_MODULES)
        string(JOIN "," RGEN_ADDITIONAL_MODULES_ARGS_STR ${RGEN_ADDITIONAL_MODULES})
        set(RGEN_ADDITIONAL_MODULES_ARGS "--additional_modules=${RGEN_ADDITIONAL_MODULES_ARGS_STR}")
    endif()

    # Разбираем каждый файл
    foreach(FILE IN LISTS RGEN_HEADERS)
        get_filename_component(FILE_ABS_PATH ${FILE} ABSOLUTE)
        get_filename_component(FILE_STEM ${FILE} NAME_WLE)

        set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_STEM}_rgen")
        set(COMPILE_COMMANDS_PATH "${OUTPUT_DIR}/${FILE_STEM}/compile_commands.json")

        file(MAKE_DIRECTORY "${OUTPUT_DIR}")

        add_custom_command(
            OUTPUT
                ${COMPILE_COMMANDS_PATH}
            COMMAND
                ${Python_EXECUTABLE} ${PY_COMPILER_COMMANDS_GENERATOR_PATH} "--srcDir=${CMAKE_SOURCE_DIR}"
                                                                            "--inputFile=${FILE_ABS_PATH}"
                                                                            "--buildDir=${CMAKE_BINARY_DIR}"
                                                                            "--compile_commands_path=${COMPILE_COMMANDS_PATH}"
                                                                            "--targetName=${TARGET_LIB}"
            DEPENDS
                ${Python_EXECUTABLE}
                ${PY_COMPILER_COMMANDS_GENERATOR_PATH}
                ${FILE_ABS_PATH}
            COMMENT
                "generate compile_commands.json | File: ${FILE_ABS_PATH}"
            BYPRODUCTS
                ${COMPILE_COMMANDS_PATH}
        )


        # === JSON PARSER START ===

        file(READ ${MODULE_EXTENSIONS_PATH} jsonTest)
        sbeParseJson(MODULE_INFO jsonTest)

        # Перебор модулей из списка
        set(ARTIFACT_PATHS)
        set(MODULE_INFO_INDEX_NAME 0)

        foreach(var ${MODULE_INFO})
            if(MODULE_INFO_${MODULE_INFO_INDEX_NAME}.name)
                list(FIND RGEN_ADDITIONAL_MODULES ${MODULE_INFO_${MODULE_INFO_INDEX_NAME}.name} found_index)

                if((NOT found_index EQUAL -1) OR ${MODULE_INFO_${MODULE_INFO_INDEX_NAME}.default})
                    set(MODULE_NAME ${MODULE_INFO_${MODULE_INFO_INDEX_NAME}.name})
                    if(NOT DEFINED MODULE_NAME)
                        break()
                    endif()

                    foreach(ext_index IN LISTS MODULE_INFO_${MODULE_INFO_INDEX_NAME}.extension)
                        set(SIFFIX_FILENAME ${MODULE_INFO_${MODULE_INFO_INDEX_NAME}.extension_${ext_index}.name})
                        set(ARTIFACT_PATH "${OUTPUT_DIR}/${FILE_STEM}${SIFFIX_FILENAME}")

                        if (${MODULE_INFO_${MODULE_INFO_INDEX_NAME}.name} STREQUAL "SwigInterface")
                            set(SWIG_INPUT_FILE ${ARTIFACT_PATH})
                        endif()

                        list(APPEND ARTIFACT_PATHS ${ARTIFACT_PATH})
                    endforeach()
                endif()
            endif()

            math(EXPR MODULE_INFO_INDEX_NAME "${MODULE_INFO_INDEX_NAME} + 1")
        endforeach()
        # === JSON PARSER END ===

        list(REMOVE_DUPLICATES ARTIFACT_PATHS)

        add_custom_command(
            OUTPUT
                ${ARTIFACT_PATHS}
            COMMAND
                ${RGEN_BIN} "--input=${FILE_ABS_PATH}"
                            "--db_dir=${COMPILE_COMMANDS_PATH}"
                            "--output_dir=${OUTPUT_DIR}"
                             "--template_dir=${TEMPLATE_DIR}"
                            ${RGEN_ADDITIONAL_MODULES_ARGS}
                            # "--debug" "--verbose"
            DEPENDS
                ${RGEN_BIN}
                ${FILE_ABS_PATH}
                ${COMPILE_COMMANDS_PATH}
            COMMENT
                "run Redkit-gen | File: ${FILE_ABS_PATH}"
            BYPRODUCTS
                ${ARTIFACT_PATHS}
        )

        target_sources(${TARGET_LIB} PRIVATE ${ARTIFACT_PATHS})

        set(ARTIFACT_DIRS "")

        # Формируем список папок для include path
        foreach(ARTIFACT_PATH IN LISTS ARTIFACT_PATHS)
            get_filename_component(PATH_P "${ARTIFACT_PATH}" DIRECTORY)

            list(APPEND ARTIFACT_DIRS ${PATH_P})
        endforeach()

        target_include_directories(${TARGET_LIB} PUBLIC ${ARTIFACT_DIRS})

        # Если есть модуль SWIG, то следует продолжить выполнение программы в отдельной функции
        if (DEFINED SWIG_INPUT_FILE AND EXISTS "${SWIG_INPUT_FILE}" AND TARGET ${TARGET_LIB})
            # Добавляем проверку на тип библиотеки
            get_target_property(TARGET_TYPE ${TARGET_LIB} TYPE)
            if(NOT TARGET_TYPE STREQUAL "OBJECT_LIBRARY")
                swig_generator(${TARGET_LIB} ${FILE_ABS_PATH} ${SWIG_INPUT_FILE})
            else()
                message(STATUS "SWIG generation skipped for OBJECT library ${TARGET_LIB}")
            endif()
        endif()
    endforeach()

    rgen_includes_and_link(${TARGET_LIB})

endfunction()

# Дополнительная генерация по SWIG
function(swig_generator TARGET_LIB HEADER_FILE ARG_SWIG_INPUT_FILE)
    find_program(SWIG_EXUTABLE NAMES swig)

    get_target_property(TARGET_INCLUDE_PATH ${TARGET_LIB} INCLUDE_DIRECTORIES)

    set(ARGS_INCLUDE_PATHS ${CMAKE_SYSTEM_PREFIX_PATH} ${CMAKE_SYSTEM_INCLUDE_PATH} ${TARGET_INCLUDE_PATH})
    set(ARG_INCLUDE_PATH "-I${CMAKE_SOURCE_DIR}")
    foreach(IPATH IN LISTS ARGS_INCLUDE_PATHS)
       list(APPEND ARG_INCLUDE_PATH "-I${IPATH}")
    endforeach()

    get_filename_component(ARG_OUTDIR ${HEADER_FILE} DIRECTORY)

    # ARTIFACT_SWIG_PATH - файл, который ожидаем получить на выходе
    # ARG_CONST - всякий стационарные параметры
    # ARG_OUTDIR - Папка, куда стоит поместить артефакт
    # ARG_INCLUDE_PATH - список инклюдпасов
    # ARG_SWIG_INPUT_FILE - файл с настройками для SWIG формата *.i

    set(ARG_CONST "-v" "-c++" "-python" "-threads")
    get_filename_component(DIR_SRC ${ARG_SWIG_INPUT_FILE} DIRECTORY)
    get_filename_component(PREPARE_NAME ${ARG_SWIG_INPUT_FILE} NAME_WLE)
    set(WRAP_NAME "${PREPARE_NAME}_wrap.cxx")
    cmake_path(APPEND ARTIFACT_SWIG_PATH "${DIR_SRC}" "${WRAP_NAME}")

    add_custom_command(
        OUTPUT
            ${ARTIFACT_SWIG_PATH}
        COMMAND
            ${SWIG_EXUTABLE} ${ARG_CONST}
                             ${ARG_INCLUDE_PATH}
                             "-outdir" ${ARG_OUTDIR}
                             ${ARG_SWIG_INPUT_FILE}
        DEPENDS
            ${SWIG_EXUTABLE}
            ${ARG_SWIG_INPUT_FILE}
        COMMENT
            "run SWIG | File: ${ARG_SWIG_INPUT_FILE}"
        BYPRODUCTS
            ${ARTIFACT_SWIG_PATH}
    )

    add_python_lib(${TARGET_LIB})
    target_sources(${TARGET_LIB} PRIVATE ${ARTIFACT_SWIG_PATH})

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(SUFFIX_FOR_COPY ".pyd")
    else()
        set(SUFFIX_FOR_COPY ".so")
    endif()

    # Перемещение динамической библиотеки для работы python к исходным файлам
    add_custom_command(
        TARGET ${TARGET_LIB}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET_LIB}>
            "${ARG_OUTDIR}/_${TARGET_LIB}_swigGen${SUFFIX_FOR_COPY}"
        COMMENT "Copying ${TARGET_LIB} after build"
    )

endfunction()

