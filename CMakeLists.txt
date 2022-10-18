cmake_minimum_required(VERSION 3.16)
project(udp_echo)

set(CMAKE_CXX_STANDARD 20)
add_library(UdpEcho SHARED UdpEcho.cpp UdpEcho.h)
set_property(TARGET UdpEcho PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(udp_echo main.cpp )
target_include_directories(udp_echo PRIVATE UdpEcho.h)

target_link_libraries(udp_echo UdpEcho)