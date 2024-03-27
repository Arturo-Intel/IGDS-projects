cmake_minimum_required(VERSION 3.19)

project(geom_crash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_executable(main)
target_sources(main PRIVATE main.cpp)
target_link_libraries(main PRIVATE Vulkan::Vulkan)
