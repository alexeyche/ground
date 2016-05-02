cmake_minimum_required(VERSION 2.8)
project(ground C CXX)

set(GROUND_MAJOR_VERSION 0)
set(GROUND_MINOR_VERSION 1)
set(GROUND_PATCH_VERSION 0)
set(GROUND_VERSION
  ${GROUND_MAJOR_VERSION}.${GROUND_MINOR_VERSION}.${GROUND_PATCH_VERSION})

# First, define all the compilation options.
# We default to debugging mode for developers.
option(DEBUG "Compile with debugging information" OFF)
option(PROFILE "Compile with profiling information" OFF)

# Set the CFLAGS and CXXFLAGS depending on the options the user specified.
# Only GCC-like compilers support -Wextra, and other compilers give tons of
# output for -Wall, so only -Wall and -Wextra on GCC.
#if(CMAKE_COMPILER_IS_GNUCC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
#endif(CMAKE_COMPILER_IS_GNUCC)
include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)


# Debugging CFLAGS.  Turn optimizations off; turn debugging symbols on.
if(DEBUG)
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g -O0")
elseif(PROFILE)
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pg")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pg")
else()
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -std=c++14 -g -O3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math -std=gnu99 -g -O3")
endif(DEBUG)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})


add_subdirectory(contrib)
add_subdirectory(ground)
