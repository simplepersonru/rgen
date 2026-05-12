include_directories(${LLVM_INCLUDE_DIRS})

add_library(core OBJECT
            comment.cpp comment.h
            command_line_args.h
            file.cpp file.h
            string_util.cpp string_util.h
            utils.cpp utils.h
            asthelper.cpp asthelper.h
            registry.cpp registry.h

            common_constants.h
            doxygen_constants.h
            logger.cpp logger.h
            visitor.h
            extracteddata.h
            ppcallbacks.h
            ppcallbacks.cpp
            inheritance.h inheritance.cpp
            derive_convert.h derive_convert.cpp
            derive_data.h
            template_loader.h)

target_link_libraries(core PUBLIC fmt::fmt hjson spdlog::spdlog)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(core PRIVATE stdc++fs)
endif()
