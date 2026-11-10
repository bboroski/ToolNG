cmake_minimum_required(VERSION 3.20)

project(WDEC VERSION 0.6.1 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

function(wdec_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Portable WDEC core: policy, evidence assessment and bounded workflow manifests.
add_library(WDEC.WdecCore STATIC
    src/sambu_engine.c
    src/sambu_workflow.c
)
target_include_directories(WDEC.WdecCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
wdec_enable_warnings(WDEC.WdecCore)

if(UNIX)
    target_link_libraries(WDEC.WdecCore PUBLIC m)
endif()

if(BUILD_TESTING)
    add_executable(WDEC.WdecTests tests/wdec_engine_tests.c)
    target_link_libraries(WDEC.WdecTests PRIVATE WDEC.WdecCore)
    wdec_enable_warnings(WDEC.WdecTests)
    add_test(NAME wdec_engine_tests COMMAND WDEC.WdecTests)

    add_executable(WDEC.WdecWorkflowTests tests/wdec_workflow_tests.c)
    target_link_libraries(WDEC.WdecWorkflowTests PRIVATE WDEC.WdecCore)
    wdec_enable_warnings(WDEC.WdecWorkflowTests)
    add_test(NAME wdec_workflow_tests COMMAND WDEC.WdecWorkflowTests)

    add_executable(WDEC.WdecRelaxationTests tests/wdec_relaxation_tests.c)
    target_link_libraries(WDEC.WdecRelaxationTests PRIVATE WDEC.WdecCore)
    wdec_enable_warnings(WDEC.WdecRelaxationTests)
    add_test(NAME wdec_relaxation_tests COMMAND WDEC.WdecRelaxationTests)
endif()

# The desktop shell is platform-specific; the policy/workflow core remains testable.
if(NOT WIN32)
    message(STATUS "WDEC desktop target is Win32-only; portable WDEC core and tests are enabled.")
    return()
endif()

enable_language(RC)

add_executable(WDEC WIN32
    src/main.c
    src/wdec.rc
)
target_include_directories(WDEC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(WDEC PRIVATE
    WDEC.WdecCore
    setupapi
    shell32
    advapi32
    ole32
    uuid
    comdlg32
)
wdec_enable_warnings(WDEC)

if(MSVC)
    target_link_options(WDEC PRIVATE
        /DYNAMICBASE
        /NXCOMPAT
        /HIGHENTROPYVA
        /MANIFEST:NO
    )
endif()

set_target_properties(WDEC PROPERTIES
    OUTPUT_NAME "WDEC"
    WIN32_EXECUTABLE TRUE
)
