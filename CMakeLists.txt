cmake_minimum_required(VERSION 3.30)

project(graphics_library)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(driver ./main.cpp)
target_link_libraries(driver PRIVATE spscqueue)

add_library(spscqueue INTERFACE)
target_include_directories(spscqueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/spsc-queue)

add_executable(test_spsc_queue tests/include/test_spsc_queue.cpp)
target_link_libraries(test_spsc_queue PRIVATE spscqueue)
add_test(NAME test_spsc_queue COMMAND test_spsc_queue)

target_compile_options(test_spsc_queue PRIVATE -fsanitize=thread -O3 -Wall -Wextra -Wpedantic -Werror)
target_link_options(test_spsc_queue PRIVATE -fsanitize=thread)
