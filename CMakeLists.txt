cmake_minimum_required(VERSION 3.20)

project(ToolNG VERSION 0.6.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

function(toolng_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# ToolNG policy core: evidence assessment and bounded workflow manifests.
add_library(ToolNG.PolicyCore STATIC
    src/toolng_engine.c
    src/toolng_workflow.c
    src/toolng_platform.c
    src/toolng_guidance.c
)
target_include_directories(ToolNG.PolicyCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
toolng_enable_warnings(ToolNG.PolicyCore)

if(UNIX)
    target_link_libraries(ToolNG.PolicyCore PUBLIC m)
endif()

if(BUILD_TESTING)
    add_executable(ToolNG.PolicyTests tests/toolng_engine_tests.c)
    target_link_libraries(ToolNG.PolicyTests PRIVATE ToolNG.PolicyCore)
    toolng_enable_warnings(ToolNG.PolicyTests)
    add_test(NAME toolng_engine_tests COMMAND ToolNG.PolicyTests)

    add_executable(ToolNG.GuidanceTests tests/toolng_guidance_tests.c)
    target_link_libraries(ToolNG.GuidanceTests PRIVATE ToolNG.PolicyCore)
    toolng_enable_warnings(ToolNG.GuidanceTests)
    add_test(NAME toolng_guidance_tests COMMAND ToolNG.GuidanceTests)

    add_executable(ToolNG.WorkflowTests tests/toolng_workflow_tests.c)
    target_link_libraries(ToolNG.WorkflowTests PRIVATE ToolNG.PolicyCore)
    toolng_enable_warnings(ToolNG.WorkflowTests)
    add_test(NAME toolng_workflow_tests COMMAND ToolNG.WorkflowTests)

    add_executable(ToolNG.RelaxationTests tests/toolng_relaxation_tests.c)
    target_link_libraries(ToolNG.RelaxationTests PRIVATE ToolNG.PolicyCore)
    toolng_enable_warnings(ToolNG.RelaxationTests)
    add_test(NAME toolng_relaxation_tests COMMAND ToolNG.RelaxationTests)

    add_executable(ToolNG.PlatformTests tests/toolng_platform_tests.c)
    target_link_libraries(ToolNG.PlatformTests PRIVATE ToolNG.PolicyCore)
    toolng_enable_warnings(ToolNG.PlatformTests)
    add_test(NAME toolng_platform_tests COMMAND ToolNG.PlatformTests)

    add_executable(ToolNG.CorrelationTests tests/toolng_correlation_tests.c)
    target_include_directories(ToolNG.CorrelationTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    toolng_enable_warnings(ToolNG.CorrelationTests)
    add_test(NAME toolng_correlation_tests COMMAND ToolNG.CorrelationTests)
endif()

# The desktop shell is platform-specific; the policy/workflow core remains testable.
if(NOT WIN32)
    message(STATUS "ToolNG desktop target is Win32-only; portable policy core and tests are enabled.")
    return()
endif()

enable_language(RC)

add_executable(ToolNG WIN32
    src/main.c
    src/toolng.rc
)
target_include_directories(ToolNG PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(ToolNG PRIVATE
    ToolNG.PolicyCore
    setupapi
    shell32
    advapi32
    ole32
    uuid
    comdlg32
)
toolng_enable_warnings(ToolNG)

if(MSVC)
    target_link_options(ToolNG PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
        /HIGHENTROPYVA
        /MANIFEST:NO
    )
endif()

set_target_properties(ToolNG PROPERTIES
    OUTPUT_NAME "ToolNG"
    WIN32_EXECUTABLE TRUE
)
