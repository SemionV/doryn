set(DORY_MODULE_NAME ${PROJECT_NAME}-testModule)

add_library(${DORY_MODULE_NAME} MODULE ${PLATFORM}
        testModule2.cpp
        factory.cpp)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "testModule")

target_link_libraries(${DORY_MODULE_NAME} modular-example-client_include)

install(TARGETS ${DORY_MODULE_NAME} DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular/modules" COMPONENT Example)