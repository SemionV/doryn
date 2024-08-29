include_guard(GLOBAL)
set(BUILDINFO_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/buildinfo")

string(TIMESTAMP BUILD_TIMESTAMP "%d:%m:%Y %H:%M" UTC)
find_program(GIT_PATH git REQUIRED)
execute_process(COMMAND
        ${GIT_PATH} log --pretty=format:%h -n 1
        OUTPUT_VARIABLE COMMIT_SHA)

configure_file(
        "${BUILDINFO_TEMPLATE_DIR}/buildinfo.yaml.in"
        "${DESTINATION}/buildinfo.yaml" @ONLY
)

function(BuildInfoSettings installDestination installComponent)
    install(FILES ${DESTINATION}/buildinfo.yaml DESTINATION ${installDestination} COMPONENT ${installComponent})
endfunction()