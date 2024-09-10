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
    set(TEMPLATES_DIR ${DORY_CMAKE_SCRIPTS_PATH}/templates)
    set(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/scripts")

    get_target_property(EXECUTABLE_FILENAME ${target} OUTPUT_NAME)
    set(DESTINATION_SCRIPT_FILE "${DESTINATION}/memcheck-${EXECUTABLE_FILENAME}.sh")

    configure_file(
            "${TEMPLATES_DIR}/memcheck.sh.in"
            ${DESTINATION_SCRIPT_FILE} @ONLY
    )

    set(TARGET_MEMCHECK_COMPONENT ${installComponent}-MEMCHECK)

    install(FILES ${DESTINATION_SCRIPT_FILE} DESTINATION ${installDestination} COMPONENT ${TARGET_MEMCHECK_COMPONENT})
    install(FILES ${TEMPLATES_DIR}/valgrind.supp DESTINATION ${installDestination} COMPONENT ${TARGET_MEMCHECK_COMPONENT})
endfunction()