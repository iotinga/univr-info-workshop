cmake_minimum_required(VERSION 3.5)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

# include ESP-IDF
include(${CMAKE_CURRENT_SOURCE_DIR}/esp-idf/tools/cmake/project.cmake)

# project name
project(ProjectTemplate VERSION ${CFG_FW_VERSION})
