if(NOT DEFINED MODULES)
    list(APPEND MODULES
        Enum
        Json
        QVariantMapSerializer
        JsonSchema
        OperatorEquals
        OperatorQStream
        OperatorLess
        QHasher
        Hasher
        Derive
        CustomModule
        )
endif()

set(APP redkit-gen)

include_directories(${LLVM_INCLUDE_DIRS})
add_executable(${APP} main.cpp)
target_link_libraries(${APP} core clangTooling ${MODULES})

# Подключать либу std::filesystem нужно только для Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${APP} stdc++fs)
endif()

string(TOLOWER "${CMAKE_SYSTEM_NAME}" LOWERCASE_SYSTEM_NAME)

# Куда следует поместить исполняемый файл redkit-gen при выполнении команды установки
install(TARGETS ${APP} DESTINATION "${CMAKE_INSTALL_PREFIX}/util/${LOWERCASE_SYSTEM_NAME}/bin")


# Сформировать ModulesInfo.* . Cтрого поле install для redkit-gen
install(SCRIPT "${CMAKE_SOURCE_DIR}/src/cmake/install.cmake")
