cmake_minimum_required(VERSION 3.12)

cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME "mutfunc")
set(PROJECT_VERSION 0.1.0)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

set(CMAKE_DEBUG_POSTFIX d)
    
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_library(mutfunc INTERFACE)
target_sources(mutfunc PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "include"
  FILES "include/mutfunc.hpp"
)

add_executable(mutfunc_test)
target_link_libraries(mutfunc_test mutfunc)
target_sources(mutfunc_test PRIVATE
    "src/test.cpp"
)
