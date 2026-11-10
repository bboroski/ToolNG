cmake_minimum_required(VERSION 3.20)

project(ToolzNG VERSION 0.6.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

function(toolzng_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Portable SAMBU core: policy, evidence assessment and bounded workflow manifests.
add_library(ToolzNG.SambuCore STATIC
    src/sambu_engine.c
    src/sambu_workflow.c
)
target_include_directories(ToolzNG.SambuCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
toolzng_enable_warnings(ToolzNG.SambuCore)

if(UNIX)
    target_link_libraries(ToolzNG.SambuCore PUBLIC m)
endif()

if(BUILD_TESTING)
    add_executable(ToolzNG.SambuTests tests/sambu_engine_tests.c)
    target_link_libraries(ToolzNG.SambuTests PRIVATE ToolzNG.SambuCore)
    toolzng_enable_warnings(ToolzNG.SambuTests)
    add_test(NAME sambu_engine_tests COMMAND ToolzNG.SambuTests)

    add_executable(ToolzNG.SambuWorkflowTests tests/sambu_workflow_tests.c)
    target_link_libraries(ToolzNG.SambuWorkflowTests PRIVATE ToolzNG.SambuCore)
    toolzng_enable_warnings(ToolzNG.SambuWorkflowTests)
    add_test(NAME sambu_workflow_tests COMMAND ToolzNG.SambuWorkflowTests)

    add_executable(ToolzNG.SambuRelaxationTests tests/sambu_relaxation_tests.c)
    target_link_libraries(ToolzNG.SambuRelaxationTests PRIVATE ToolzNG.SambuCore)
    toolzng_enable_warnings(ToolzNG.SambuRelaxationTests)
    add_test(NAME sambu_relaxation_tests COMMAND ToolzNG.SambuRelaxationTests)
endif()

# The desktop shell is platform-specific; the policy/workflow core remains testable.
if(NOT WIN32)
    message(STATUS "ToolzNG desktop target is Win32-only; portable SAMBU core and tests are enabled.")
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
