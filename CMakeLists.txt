cmake_minimum_required(VERSION 3.10.0)
project(win32_test VERSION 0.1.0 LANGUAGES C CXX)

add_executable(win32_test main.cpp)

set_target_properties(win32_test PROPERTIES
    LINK_FLAGS "/ENTRY:WinMainCRTStartup"
)

target_link_libraries(win32_test user32 gdi32)