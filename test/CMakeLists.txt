cmake_minimum_required(VERSION 3.5)

project(ProjectTemplateTests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(MAIN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../main/)
set(CFG_LOG_LEVEL 5 CACHE STRING "log level to use in the firmware (1=error, 2=warning, 3=info, 4=debug, 5=verbose)")

message("Building for tests")

file(GLOB POSIX_ADAPTER_SOURCES "${MAIN_ROOT}/src/adapter/posix/*.c")
file(GLOB_RECURSE PROJECT_SOURCES "${MAIN_ROOT}/src/*.c")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "adapter/")

set(PROJECT_INCLUDE_DIRS "${MAIN_ROOT}/src/")

list(APPEND SOURCES 
    ${POSIX_ADAPTER_SOURCES} 
    ${PROJECT_SOURCES}
)

list(APPEND INCLUDE_DIRS 
    ${PROJECT_INCLUDE_DIRS}
)

include(../cmake/cmoka.cmake)

enable_testing()

function(add_test_file name)
    add_executable(${name} ${name}.c ${SOURCES})
    target_include_directories(${name} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${name} PRIVATE cmocka-static)
    target_compile_options(${name} PRIVATE -fsanitize=address -g -O0 -fno-omit-frame-pointer)
    target_link_options(${name} PRIVATE -fsanitize=address)
    target_compile_definitions(${name} PRIVATE
        EMULATOR=1
        VERSION_MAJOR=0
        VERSION_MINOR=0
        VERSION_PATCH=0
        VERSION="0.0.0-dev"
        LOG_LEVEL=${CFG_LOG_LEVEL}
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_file("test_led")

