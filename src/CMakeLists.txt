# compiler settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Убираем шлак в выводе сборки на Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-enum-enum-conversion \
                                            -Wno-deprecated-anon-enum-enum-conversion \
                                            -Wno-ambiguous-reversed-operator")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include)

# additional cmake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include clang
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Found Clang ${Clang_PACKAGE_VERSION}")

include_directories(${CLANG_INCLUDE_DIRS})

# add cmake
include(rgen_internal)

enable_testing()

# Для совместимости тестов с Docker-контейнером. Зависимость необходима для Qt
set(CLANG_INCLUDE_PATH "/tools/clang-19/lib/clang/19/include")
if(EXISTS ${CLANG_INCLUDE_PATH})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CLANG_INCLUDE_PATH}")
endif()


# add subdirectories
add_subdirectory(bin)
add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(tests)

get_property(ALL_TEST_TARGETS GLOBAL PROPERTY ALL_TEST_TARGETS)

if(ALL_TEST_TARGETS)
    add_custom_target(build_module_tests
        DEPENDS ${ALL_TEST_TARGETS}
    )
endif()

# enable googletest
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Для Windows, чтобы тесты собирались нормально
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
endif()
FetchContent_MakeAvailable(googletest)

# enabled fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        11.0.2
)
set(FMT_INSTALL OFF) # Не трогаем библиотеку при выполнении команды install
FetchContent_MakeAvailable(fmt)

#enable spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0
)
set(SPDLOG_FMT_EXTERNAL_HO ON) # Использовать внешний fmt, поставляемый как header-only (HO)
FetchContent_MakeAvailable(spdlog)

#enable hjson

set(HJSON_ENABLE_TEST OFF CACHE BOOL "")
set(HJSON_ENABLE_PERFTEST OFF CACHE BOOL "")
set(HJSON_ENABLE_INSTALL OFF CACHE BOOL "")

FetchContent_Declare(
  hjson
  GIT_REPOSITORY https://github.com/hjson/hjson-cpp.git
  GIT_TAG        2.6   # можно заменить на релизный тег
)

FetchContent_MakeAvailable(hjson)
