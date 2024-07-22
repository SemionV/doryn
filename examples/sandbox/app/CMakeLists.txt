cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(sandbox-app)

add_executable(${PROJECT_NAME} ${PLATFORM} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "sandbox")

target_link_libraries(${PROJECT_NAME} PRIVATE engine mainModule dory-bootstrap_mod)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define the source and destination directories
set(DATA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/configuration)
set(DATA_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/configuration)

# Custom command to copy the directory
add_custom_command(
        OUTPUT ${DATA_DEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SOURCE_DIR} ${DATA_DEST_DIR}
        DEPENDS ${DATA_SOURCE_DIR}
        COMMENT "Copying configuration directory to output directory"
)

# Custom target to invoke the custom command
add_custom_target(copy_configuration ALL DEPENDS ${DATA_DEST_DIR})

# List of files to copy
set(FILES_TO_COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/settings.yaml
)

# List to store the copy commands' output files
set(COPIED_FILES)

# Define custom commands for each file
foreach(file ${FILES_TO_COPY})
    get_filename_component(filename ${file} NAME)
    set(destination ${CMAKE_CURRENT_BINARY_DIR}/${filename})
    add_custom_command(
            OUTPUT ${destination}
            COMMAND ${CMAKE_COMMAND} -E copy ${file} ${destination}
            DEPENDS ${file}
            COMMENT "Copying ${file} to ${destination}"
    )
    list(APPEND COPIED_FILES ${destination})
endforeach()

# Custom target to invoke the custom commands
add_custom_target(copy_files ALL DEPENDS ${COPIED_FILES})

# Define a custom clean target
add_custom_target(custom_clean_sandbox
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/configuration/*
        COMMENT "Custom clean: Removing contents of configuration directory"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/settings.yaml
        COMMENT "Custom clean: Removing setting.yaml"
)

# Ensure the custom clean target is executed before the default clean target
add_dependencies(clean custom_clean_sandbox)

# Ensure the copy_data target is built before the executable
add_dependencies(${PROJECT_NAME} copy_configuration copy_files)


install(TARGETS ${PROJECT_NAME} DESTINATION ${DORY_SANDBOX_INSTALL_PREFIX} COMPONENT ${DORY_SANDBOX_COMPONENT})
install(DIRECTORY configuration DESTINATION ${DORY_SANDBOX_INSTALL_PREFIX} COMPONENT ${DORY_SANDBOX_COMPONENT})
install(FILES settings.yaml DESTINATION ${DORY_SANDBOX_INSTALL_PREFIX} COMPONENT ${DORY_SANDBOX_COMPONENT})