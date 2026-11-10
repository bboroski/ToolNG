cmake_minimum_required(VERSION 3.20)

project(ToolzNG VERSION 0.4.0 LANGUAGES C)

if(NOT WIN32)
    message(FATAL_ERROR "ToolzNG is a native Windows application. Build it on Windows with MSVC or MinGW-w64.")
endif()

enable_language(RC)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(ToolzNG WIN32
    src/main.c
    src/toolzng.rc
)

target_include_directories(ToolzNG PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Windows feature macros are declared once in src/main.c for the native C build.

target_link_libraries(ToolzNG PRIVATE
    setupapi
    shell32
    advapi32
    ole32
    uuid
    comdlg32
)

if(MSVC)
    target_compile_options(ToolzNG PRIVATE /W4 /utf-8 /permissive-)
    target_link_options(ToolzNG PRIVATE /DYNAMICBASE /NXCOMPAT /HIGHENTROPYVA /MANIFEST:NO)
else()
    target_compile_options(ToolzNG PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(ToolzNG PROPERTIES
    OUTPUT_NAME "ToolzNG"
    WIN32_EXECUTABLE TRUE
)
