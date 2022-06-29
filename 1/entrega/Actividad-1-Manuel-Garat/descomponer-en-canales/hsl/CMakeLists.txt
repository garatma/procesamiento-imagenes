cmake_minimum_required(VERSION 2.8)
project(hsl)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(hsl.out hsl.cpp)
target_link_libraries(hsl.out ${OpenCV_LIBS})
target_compile_options(hsl.out PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./hsl.out "$(ARGS)"
)
