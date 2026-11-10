cmake_minimum_required(VERSION 3.20)

project(ToolzNG VERSION 0.6.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

# The SAMBU engine is portable C17. Keep it buildable and testable without Win32.
add_library(ToolzNG.SambuCore STATIC
    src/sambu_engine.c
)
target_include_directories(ToolzNG.SambuCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

function(toolzng_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

toolzng_enable_warnings(ToolzNG.SambuCore)

if(BUILD_TESTING)
    add_executable(sambu_engine_tests
        tests/sambu_engine_tests.c
    )
    target_link_libraries(sambu_engine_tests PRIVATE ToolzNG.SambuCore)
    toolzng_enable_warnings(sambu_engine_tests)
    add_test(NAME sambu_engine_tests COMMAND sambu_engine_tests)
endif()

if(NOT WIN32)
    message(STATUS
        "ToolzNG's Win32 desktop application is Windows-only. "
        "Building the portable SAMBU core and its tests on this platform.")
    return()
endif()

enable_language(RC)

add_executable(ToolzNG WIN32
    src/main.c
    src/toolzng.rc
)

target_include_directories(ToolzNG PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(ToolzNG PRIVATE
    ToolzNG.SambuCore
    setupapi
    shell32
    advapi32
    ole32
    uuid
    comdlg32
)

toolzng_enable_warnings(ToolzNG)

if(MSVC)
    target_link_options(ToolzNG PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
        /HIGHENTROPYVA
        /MANIFEST:NO
    )
endif()

set_target_properties(ToolzNG PROPERTIES
    OUTPUT_NAME "ToolzNG"
    WIN32_EXECUTABLE TRUE
)
