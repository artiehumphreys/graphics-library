cmake_minimum_required(VERSION 3.30)

project(graphics_library)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Libraries
add_library(core INTERFACE)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(window src/platform/macos/window.mm)
target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(window PRIVATE "-framework Cocoa")
set_source_files_properties(src/platform/macos/window.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")

add_library(draw_context src/platform/macos/draw_context.mm)
target_include_directories(draw_context PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(draw_context PRIVATE "-framework CoreGraphics")
set_source_files_properties(src/platform/macos/draw_context.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")

# Main executable
add_executable(driver ./main.cpp)
target_compile_options(driver PRIVATE -O3 -Wall -Wpedantic -Werror -Wextra)
target_link_libraries(driver PRIVATE core)

# Graphics demo
add_executable(graphics_demo src/demo.cpp)
target_compile_options(graphics_demo PRIVATE -O3 -Wall -Wpedantic -Werror -Wextra)
target_link_libraries(graphics_demo PRIVATE core window draw_context "-framework CoreGraphics")

# Tests
add_executable(test_spsc_queue tests/test_spsc_queue.cpp)
target_link_libraries(test_spsc_queue PRIVATE core)
target_compile_options(test_spsc_queue PRIVATE -fsanitize=thread -O3 -Wall -Wextra -Wpedantic -Werror)
target_link_options(test_spsc_queue PRIVATE -fsanitize=thread)
add_test(NAME test_spsc_queue COMMAND test_spsc_queue)
