cmake_minimum_required(VERSION 3.30)

project(graphics_library)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Libraries
add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable
add_executable(driver ./main.cpp)
target_compile_options(driver PRIVATE -O3 -Wall -Wpedantic -Werror -Wextra)
target_link_libraries(driver PRIVATE core)

# Tests
add_executable(test_spsc_queue tests/test_spsc_queue.cpp)
target_link_libraries(test_spsc_queue PRIVATE core)
target_compile_options(test_spsc_queue PRIVATE -fsanitize=thread -O3 -Wall -Wextra -Wpedantic -Werror)
target_link_options(test_spsc_queue PRIVATE -fsanitize=thread)
add_test(NAME test_spsc_queue COMMAND test_spsc_queue)
