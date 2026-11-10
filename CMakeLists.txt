cmake_minimum_required(VERSION 3.20)

project(ToolzNG VERSION 0.6.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

# The SAMBU policy/workflow core is platform-independent and can be tested
# on any C17 host, while the desktop application remains native Win32.
add_library(ToolzNG.SambuCore STATIC
    src/sambu_engine.c
    src/sambu_workflow.c
)
target_include_directories(ToolzNG.SambuCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(MSVC)
    target_compile_options(ToolzNG.SambuCore PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(ToolzNG.SambuCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(ToolzNG.SambuTests tests/sambu_engine_tests.c)
    target_link_libraries(ToolzNG.SambuTests PRIVATE ToolzNG.SambuCore)
    add_test(NAME sambu_engine_tests COMMAND ToolzNG.SambuTests)

    add_executable(ToolzNG.SambuWorkflowTests tests/sambu_workflow_tests.c)
    target_link_libraries(ToolzNG.SambuWorkflowTests PRIVATE ToolzNG.SambuCore)
    add_test(NAME sambu_workflow_tests COMMAND ToolzNG.SambuWorkflowTests)
endif()

if(NOT WIN32)
    message(STATUS "ToolzNG desktop target is Win32-only; portable SAMBU tests are available.")
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
