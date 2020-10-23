cmake_minimum_required(VERSION 2.8)
project(cartoon)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(cartoon cartoon.cpp)
target_link_libraries(cartoon ${OpenCV_LIBS})
target_compile_options(cartoon PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./cartoon "$(ARGS)"
)
