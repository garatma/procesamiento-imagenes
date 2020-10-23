cmake_minimum_required(VERSION 2.8)
project(galletitas)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(galletitas galletitas.cpp)
target_link_libraries(galletitas ${OpenCV_LIBS})
target_compile_options(galletitas PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./galletitas "$(ARGS)"
)
