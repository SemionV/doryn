include_guard(GLOBAL)

function(AddCppCheck target)
    find_program(CPPCHECK_PATH cppcheck)
    if(CPPCHECK_PATH)
        set_target_properties(${target}
                PROPERTIES CXX_CPPCHECK
                "${CPPCHECK_PATH};--enable=warning;--error-exitcode=10;--suppress=preprocessorErrorDirective"
        )
    endif()
endfunction()