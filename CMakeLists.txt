#----------------------------------------------------------------------------
#   CMakeLists.txt    Defines the build and install configurations for the 
#                     MBR Tool app (mbr-tool)
#             
#   Copyright (c) 2020 Mark Elrod. All rights reserved. This file is 
#   distributed under the MIT License. See LICENSE.TXT for details.
#----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Specify the minimum version of CMake required to build the project.
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

# ----------------------------------------------------------------------------
# Set the name of the project.
# ----------------------------------------------------------------------------
set(name "mbr-tool")
project(${name})

# ----------------------------------------------------------------------------
# Add the include directory to the include search paths.
# ----------------------------------------------------------------------------
include_directories(include)

if(WIN32)

# ----------------------------------------------------------------------------
# /std:c++14    Use the C++ 14 standard.
# ----------------------------------------------------------------------------
    set(CMAKE_CXX_FLAGS "/std:c++14 ${CMAKE_CXX_FLAGS}")
else()

# ----------------------------------------------------------------------------
# C++ compiler flags:
# -O0                   Turn off optimizations to allow for easier debugging.
# -std=c++14            Use the C++ 14 standard.
# -Wall                 Enable all warnings.
# ----------------------------------------------------------------------------
	set(CMAKE_CXX_FLAGS "-O0 -std=c++14 -Wall ${CMAKE_CXX_FLAGS}")
endif()

# ----------------------------------------------------------------------------
# Set the list of source files.
# ----------------------------------------------------------------------------
FILE (GLOB SRC_FILES src/*.cpp)

# ----------------------------------------------------------------------------
# Set the list of include files.
# ----------------------------------------------------------------------------
FILE (GLOB HEADER_FILES include/*.h)

# ----------------------------------------------------------------------------
# Create the executable target
# ----------------------------------------------------------------------------
add_executable(${name} ${SRC_FILES} ${HEADER_FILES})

# ----------------------------------------------------------------------------
# Install the app in the bin directory
# ----------------------------------------------------------------------------
install(TARGETS ${name} 
    RUNTIME DESTINATION bin)


