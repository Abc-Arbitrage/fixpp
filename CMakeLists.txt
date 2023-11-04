cmake_minimum_required (VERSION 2.8.7)
project (fixpp)
include (CheckCXXCompilerFlag)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)

if (COMPILER_SUPPORTS_CXX20)
    message(STATUS "Enabling C++20 mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
elseif (COMPILER_SUPPORTS_CXX20)
    message(STATUS "Enabling C++14 mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(GTest)
if (GTEST_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS})
else()
    ADD_SUBDIRECTORY(googletest-release-1.8.0)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
endif()

enable_testing()
add_subdirectory(tests)

