include_guard(GLOBAL)

function(AddCppCheck target)
    find_program(CPPCHECK_PATH cppcheck)
    if(CPPCHECK_PATH)
        set_target_properties(${target}
                PROPERTIES CXX_CPPCHECK
                "${CPPCHECK_PATH};--enable=warning;--error-exitcode=10;--suppress=preprocessorErrorDirective;--suppress=arrayIndexOutOfBounds:*glm/detail/*;--suppress=objectIndex:*glm/detail/*;--suppress=funcArgOrderDifferent:*glm/detail/*"
        )
    endif()
endfunction()