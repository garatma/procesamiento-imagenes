cmake_minimum_required(VERSION 2.8)
project(rgb)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(rgb rgb.cpp)
target_link_libraries(rgb ${OpenCV_LIBS})
target_compile_options(rgb PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./rgb "$(ARGS)"
)
