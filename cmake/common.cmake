# get project version from git
find_package(Git)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "git not found")
endif()

execute_process(COMMAND git describe --tags --abbrev=100 --always OUTPUT_VARIABLE GIT_DESCRIBE_OUT)
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" CFG_FW_VERSION_MAJOR "${GIT_DESCRIBE_OUT}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" CFG_FW_VERSION_MINOR "${GIT_DESCRIBE_OUT}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" CFG_FW_VERSION_PATCH "${GIT_DESCRIBE_OUT}")
set(CFG_FW_VERSION "${CFG_FW_VERSION_MAJOR}.${CFG_FW_VERSION_MINOR}.${CFG_FW_VERSION_PATCH}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(SDKCONFIG sdkconfig.production)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Delivery")
    set(SDKCONFIG sdkconfig.delivery)
else()
    set(SDKCONFIG sdkconfig.develop)
endif()

set(COMPONENTS main)

add_compile_options(-Wall -Wextra -Wno-unused-parameter -Werror -Wno-error=cpp)

message("~~~ESP32 PROJECT TEMPLATE~~~")
message("Version ${CFG_FW_VERSION} type ${CMAKE_BUILD_TYPE}")