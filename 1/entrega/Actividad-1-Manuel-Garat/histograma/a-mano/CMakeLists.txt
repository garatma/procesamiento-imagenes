cmake_minimum_required(VERSION 2.8)
project(histograma)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(histograma.out histograma.cpp)
target_link_libraries(histograma.out ${OpenCV_LIBS})
target_compile_options(histograma.out PRIVATE -Wpedantic -Werror -Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(run
  COMMAND ./histograma.out "$(ARGS)"
)
