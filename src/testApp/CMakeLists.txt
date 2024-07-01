cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-testApp)

add_executable(testApp ${PLATFORM} main.cpp)
target_precompile_headers(testApp PRIVATE dependencies.h)
target_link_libraries(testApp application dory-openGL)

install(TARGETS testApp DESTINATION . COMPONENT Example)
install(DIRECTORY configuration DESTINATION . COMPONENT Example)
install(FILES settings.yaml DESTINATION . COMPONENT Example)