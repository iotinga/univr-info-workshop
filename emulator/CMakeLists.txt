cmake_minimum_required(VERSION 3.5)

project(ProjectTemplateEmu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MAIN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../main/)

set(CFG_LOG_LEVEL 5 CACHE STRING "log level to use in the firmware (1=error, 2=warning, 3=info, 4=debug, 5=verbose)")

message("Building for emulator")

file(GLOB POSIX_ADAPTER_SOURCES "${MAIN_ROOT}/src/adapter/posix/*.c")
file(GLOB_RECURSE PROJECT_SOURCES "${MAIN_ROOT}/src/*.c")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "adapter/")

set(PROJECT_INCLUDE_DIRS "${MAIN_ROOT}/src/")

list(APPEND SOURCES 
    ${POSIX_ADAPTER_SOURCES} 
    ${PROJECT_SOURCES}
    "main.c"
)

list(APPEND INCLUDE_DIRS 
    ${PROJECT_INCLUDE_DIRS}
)

set(TARGET ProjectTemplateEmu)
add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${TARGET} PRIVATE pthread m)
target_compile_options(${TARGET} PRIVATE -fsanitize=address -g -O0 -fno-omit-frame-pointer)
target_link_options(${TARGET} PRIVATE -fsanitize=address)

message("LOG LEVEL ${CFG_LOG_LEVEL}")

message("Configuring target: ${TARGET}")
target_compile_definitions(${TARGET} PRIVATE
    EMULATOR=1
    VERSION_MAJOR=0
    VERSION_MINOR=0
    VERSION_PATCH=0
    VERSION="0.0.0-dev"
    LOG_LEVEL=${CFG_LOG_LEVEL}
)
