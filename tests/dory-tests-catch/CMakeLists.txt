project(dory-tests-catch)

add_executable(${PROJECT_NAME} linearAlgebra.cpp
        templates.cpp
        eventTests.cpp
        messagesTests.cpp
        repositoryTests.cpp
        typeMapping.cpp
        matrixCalculatorTests.cpp
        pipelineServiceTests.cpp
        dataLayoutTests.cpp
        objectVisitorTests.cpp
        concurrency.cpp
        atm_book.cpp
        scriptService.cpp
        serializationTests.cpp
        configurationTests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        engine_include
        refl-cpp
        nlohmann_json::nlohmann_json
        ryml::ryml
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        Catch2::Catch2WithMain)

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)