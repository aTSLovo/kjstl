cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(kjstl LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file(GLOB sources CONFIGURE_DEPENDS *.cpp)

foreach(source IN ITEMS ${sources})
    get_filename_component(name "${source}" NAME_WLE)
    add_executable(${name} ${name}.cpp)
endforeach()
