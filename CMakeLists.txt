cmake_minimum_required(VERSION 3.20)

project(WDEP VERSION 0.6.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

function(wdep_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Portable SAMBU core: policy, evidence assessment and bounded workflow manifests.
add_library(WDEP.PolicyCore STATIC
    src/sambu_engine.c
    src/sambu_workflow.c
)
add_library(ToolzNG.SambuCore ALIAS WDEP.PolicyCore)
target_include_directories(WDEP.PolicyCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
wdep_enable_warnings(WDEP.PolicyCore)

if(UNIX)
    target_link_libraries(WDEP.PolicyCore PUBLIC m)
endif()

if(BUILD_TESTING)
    add_executable(WDEP.PolicyTests tests/sambu_engine_tests.c)
    add_executable(ToolzNG.SambuTests ALIAS WDEP.PolicyTests)
    target_link_libraries(WDEP.PolicyTests PRIVATE WDEP.PolicyCore)
    wdep_enable_warnings(WDEP.PolicyTests)
    add_test(NAME sambu_engine_tests COMMAND WDEP.PolicyTests)

    add_executable(WDEP.WorkflowTests tests/sambu_workflow_tests.c)
    add_executable(ToolzNG.SambuWorkflowTests ALIAS WDEP.WorkflowTests)
    target_link_libraries(WDEP.WorkflowTests PRIVATE WDEP.PolicyCore)
    wdep_enable_warnings(WDEP.WorkflowTests)
    add_test(NAME sambu_workflow_tests COMMAND WDEP.WorkflowTests)

    add_executable(WDEP.RelaxationTests tests/sambu_relaxation_tests.c)
    add_executable(ToolzNG.SambuRelaxationTests ALIAS WDEP.RelaxationTests)
    target_link_libraries(WDEP.RelaxationTests PRIVATE WDEP.PolicyCore)
    wdep_enable_warnings(WDEP.RelaxationTests)
    add_test(NAME sambu_relaxation_tests COMMAND WDEP.RelaxationTests)
endif()

# The desktop shell is platform-specific; the policy/workflow core remains testable.
if(NOT WIN32)
    message(STATUS "WDEP desktop target is Win32-only; portable SAMBU core and tests are enabled.")
    return()
endif()

enable_language(RC)

add_executable(WDEP WIN32
    src/main.c
    src/toolzng.rc
)
add_executable(ToolzNG ALIAS WDEP)
target_include_directories(WDEP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(WDEP PRIVATE
    WDEP.PolicyCore
    setupapi
    shell32
    advapi32
    ole32
    uuid
    comdlg32
)
wdep_enable_warnings(ToolzNG)

if(MSVC)
    target_link_options(ToolzNG PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
        /HIGHENTROPYVA
        /MANIFEST:NO
    )
endif()

set_target_properties(WDEP PROPERTIES
    OUTPUT_NAME "WDEP"
    WIN32_EXECUTABLE TRUE
)
