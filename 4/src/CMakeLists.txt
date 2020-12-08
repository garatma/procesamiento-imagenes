cmake_minimum_required(VERSION 3.19)
project(galletitas)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(galletitas.out galletitas.cpp)
target_link_libraries(galletitas.out ${OpenCV_LIBS})
target_compile_options(galletitas.out PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./galletitas.out "$(ARGS)"
)
