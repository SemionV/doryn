include_guard(GLOBAL)

include(FetchContent)
FetchContent_Declare(
        memcheck-cover
        GIT_REPOSITORY https://github.com/Farigh/memcheck-cover.git
        GIT_TAG        release-1.2
)
FetchContent_MakeAvailable(memcheck-cover)

function(AddMemcheck target)
    set(MEMCHECK_PATH ${memcheck-cover_SOURCE_DIR}/bin)
    set(REPORT_PATH "${CMAKE_BINARY_DIR}/valgrind-${target}")

    add_custom_target(memcheck-${target}
            COMMAND ${MEMCHECK_PATH}/memcheck_runner.sh -o
            "${REPORT_PATH}/report"
            -- $<TARGET_FILE:${target}>
            COMMAND ${MEMCHECK_PATH}/generate_html_report.sh
            -i ${REPORT_PATH}
            -o ${REPORT_PATH}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()

function(GenerateValgrindScript target installDestination installComponent)
    set(BUILDINFO_TEMPLATE_DIR ${DORY_CMAKE_SCRIPTS_PATH})
    set(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/scripts")

    set(MEMCHECK_PATH ${memcheck-cover_SOURCE_DIR}/bin)
    set(REPORT_PATH "memcheck-report")
    set(EXECUTABLE ${target})

    configure_file(
            "${BUILDINFO_TEMPLATE_DIR}/run-memcheck.sh.in"
            "${DESTINATION}/run-memcheck.sh" @ONLY
    )

    install(FILES ${DESTINATION}/run-memcheck.sh DESTINATION ${installDestination} COMPONENT ${installComponent})
endfunction()