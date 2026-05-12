set(RG_BIN "redkit-gen")

if(WIN32)
    set(LOWERCASE_SYSTEM_NAME "windows")
    set(RG_BIN "${RG_BIN}.exe")
else()
    set(LOWERCASE_SYSTEM_NAME "linux")
endif()

function(run_rgen GET_ALL_MODULES_INFO MODULE_INFO_TYPE)
    set(BIN_PATH "${CMAKE_INSTALL_PREFIX}/util/${LOWERCASE_SYSTEM_NAME}/bin/${RG_BIN}")
    execute_process(
        COMMAND 
            "${BIN_PATH}" "--get_all_modules_info=${GET_ALL_MODULES_INFO}" 
                          "--module_info_type=${MODULE_INFO_TYPE}"
        RESULT_VARIABLE exit_code
        OUTPUT_VARIABLE stdout_output
        ERROR_VARIABLE stderr_output
    )

    if(NOT exit_code EQUAL 0)
        message(WARNING "Error running redkit-gen:")
        message(WARNING "Exit code: ${exit_code}")

        if(stderr_output)
            message(STATUS "Stderr:\n${stderr_output}")
        elseif(stdout_output)
            message(STATUS "Stdout:\n${stdout_output}")
        else()
            message(STATUS "No output captured.")
        endif()
    endif()
endfunction()

run_rgen("${CMAKE_INSTALL_PREFIX}/cmake/" "json")
