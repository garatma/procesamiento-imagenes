cmake_minimum_required(VERSION 2.8)
project(rgb)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(rgb.out rgb.cpp)
target_link_libraries(rgb.out ${OpenCV_LIBS})
target_compile_options(rgb.out PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./rgb.out "$(ARGS)"
)
