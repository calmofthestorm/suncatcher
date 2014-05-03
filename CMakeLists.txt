# Copyright (c) 2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 2.6)
project(suncatcher)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    set(SYSTEM_LIBRARIES        "")
    add_definitions (/D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")
    set(SYSTEM_LIBRARIES        "rt")
    add_definitions (-std=c++11)
endif()
set(CPPLINT_ARG_VERBOSE "--verbose=1")

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected; defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# List all sources/headers files
# used only for cpplint: adding a file still require modifing the CMakeLists.txt
# so that CMake know that it should rebuild the project (and better practice)
file(GLOB all_source_files
    "${PROJECT_SOURCE_DIR}/src/*.hh"
    "${PROJECT_SOURCE_DIR}/src/*.cc"
)


set( BOOST_COMPONENTS_NEEDED )

# The following verifyies that BOOST_ROOT is set properly.
if(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
    FILE( TO_CMAKE_PATH $ENV{BOOST_ROOT} BOOST_ROOT )
    if( NOT EXISTS ${BOOST_ROOT} ) 
        MESSAGE( STATUS  ${BOOST_ROOT} " does not exist. Checking if BOOST_ROOT was a quoted string.." )
        STRING( REPLACE "\"" "" BOOST_ROOT ${BOOST_ROOT} ) 
        if( EXISTS ${BOOST_ROOT} ) 
            MESSAGE( STATUS "After removing the quotes " ${BOOST_ROOT} " was now found by CMake" )
        endif( EXISTS ${BOOST_ROOT})
    endif( NOT EXISTS ${BOOST_ROOT} )

# Save the BOOST_ROOT in the cache
    if( NOT EXISTS ${BOOST_ROOT} ) 
        MESSAGE( WARNING ${BOOST_ROOT} " does not exist." )
    else(NOT EXISTS ${BOOST_ROOT})
        SET (BOOST_ROOT ${BOOST_ROOT} CACHE STRING "Set the value of BOOST_ROOT to point to the root folder of your boost install." FORCE)
        #SET (BOOST_INCLUDEDIR ${BOOST_ROOT}/Include)
        #SET (BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
    endif( NOT EXISTS ${BOOST_ROOT} )

endif(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")

if( WIN32 AND NOT BOOST_ROOT )
    MESSAGE( WARNING "Please set the BOOST_ROOT environment variable." )
endif( WIN32 AND NOT BOOST_ROOT )

set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0")
set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE(Boost 1.47.0 COMPONENTS ${BOOST_COMPONENTS_NEEDED})
if(Boost_FOUND)
    MESSAGE( STATUS "Setting up boost." )
    include_directories(${Boost_INCLUDE_DIRS})
    if(Boost_DEBUG) 
        MESSAGE( STATUS "BOOST Libraries " ${Boost_LIBRARIES} )
        FOREACH(BOOST_COMPONENT ${BOOST_COMPONENTS_NEEDED})
            STRING( TOUPPER ${BOOST_COMPONENT} BOOST_COMPONENT_UPCASE )
            MESSAGE( STATUS "Boost " ${BOOST_COMPONENT} ": " ${Boost_${BOOST_COMPONENT_UPCASE}_LIBRARY} )
            MESSAGE( STATUS "Boost " ${BOOST_COMPONENT} " Debug: " ${Boost_${BOOST_COMPONENT_UPCASE}_LIBRARY_DEBUG} )
            MESSAGE( STATUS "Boost " ${BOOST_COMPONENT} " Release: " ${Boost_${BOOST_COMPONENT_UPCASE}_LIBRARY_RELEASE} )
        ENDFOREACH(BOOST_COMPONENT)
    endif(Boost_DEBUG)
endif(Boost_FOUND)

# cppcheck config file
configure_file(std.cfg std.cfg COPYONLY)

# maps for test cases
if (NOT EXISTS ${CMAKE_BINARY_DIR}/maps)
  file(COPY maps DESTINATION . )
endif()

# Buid directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# All includes are relative to the "src" directory
include_directories ("${PROJECT_SOURCE_DIR}/include")

# Source for the library itself.
file (GLOB suncatcher_srcs ${PROJECT_SOURCE_DIR}/src/*.cc)

# Source for the testing support code. (Common code and non-unit tests)
file (GLOB test_srcs
    ${PROJECT_SOURCE_DIR}/src/test/*.cc
    ${PROJECT_SOURCE_DIR}/src/test/micropather/*.cc)

# Source for the unit tests. (The actual test cases for googletest)
file (GLOB unittest_srcs ${PROJECT_SOURCE_DIR}/test/*.cc)

# Only combine common objects once.
add_library(suncatcher_objs OBJECT
    ${suncatcher_srcs}
  )

add_library(test_objs OBJECT
    ${test_srcs}
  )

add_library(unittest_objs OBJECT
    ${unittest_srcs}
  )

# add the repl loop that lets you load up and play with 2D maps interactively.
add_executable(repl
    ${PROJECT_SOURCE_DIR}/src/tool/repl.cc
    $<TARGET_OBJECTS:test_objs>
    $<TARGET_OBJECTS:suncatcher_objs>)
target_link_libraries (repl ${SYSTEM_LIBRARIES})

# add door_torture, a brute force all pairs door tester.
add_executable(door_torture
    ${PROJECT_SOURCE_DIR}/src/tool/door_torture.cc
    $<TARGET_OBJECTS:test_objs>
    $<TARGET_OBJECTS:suncatcher_objs>)
target_link_libraries (door_torture ${SYSTEM_LIBRARIES})

# add a cpplint target to the "all" target
add_custom_target(cpplint
    ALL
    COMMAND python cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_VERBOSE} ${all_source_files}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# add a cppcheck target to the "all" target
add_custom_target(cppcheck
    ALL
    COMMAND cppcheck -j 4 cppcheck --enable=style --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
)

enable_testing()

# run unit tests after every build by default.
add_custom_target(unittest
    ALL
    DEPENDS run_unit_tests
    COMMAND ${CMAKE_BINARY_DIR}/bin/run_unit_tests --output-on-failure
)

# add a Doxygen target to the "all" target
if (NOT DEFINED ENV{TRAVIS})
    # if not runing on a Travis CI Virtual Machine
    add_custom_target(doxygen
        ALL
        COMMAND doxygen Doxyfile > ${DEV_NULL}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else()
    # but no Doxygen under Travis CI: too costly and no real benefit
    message("no Doxygen target when TRAVIS is defined")
endif()


# add the subdirectory containing the CMakeLists.txt for the gtest library
add_subdirectory (thirdparty)
include_directories ("${PROJECT_SOURCE_DIR}/thirdparty/gtest/include")
include_directories ("${PROJECT_SOURCE_DIR}/thirdparty/gmock/include")

# add the test executable
add_executable (run_unit_tests
    $<TARGET_OBJECTS:suncatcher_objs>
    $<TARGET_OBJECTS:test_objs>
    $<TARGET_OBJECTS:unittest_objs>)
target_link_libraries (run_unit_tests gtest_main ${SYSTEM_LIBRARIES})

set_target_properties(cpplint cppcheck doxygen  PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
