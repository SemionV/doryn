cmake_minimum_required(VERSION 3.26)

project(dory)

add_subdirectory(base)
add_subdirectory(testDomain)
add_subdirectory(testApp)

install(TARGETS testApp COMPONENT Example)
install(TARGETS testDomain COMPONENT Example)
install(TARGETS base COMPONENT Engine)