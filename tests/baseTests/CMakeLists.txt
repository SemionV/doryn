cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

add_executable(baseTests linearAlgebra.cpp
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

target_link_libraries(baseTests PRIVATE
        inc_engine
        refl-cpp
        nlohmann_json::nlohmann_json
        ryml::ryml
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        Catch2::Catch2WithMain)

#target_precompile_headers(baseTests PRIVATE dependencies.h)

install(TARGETS baseTests DESTINATION . COMPONENT Tests)