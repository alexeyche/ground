cmake_minimum_required(VERSION 2.8)

set(SOURCES
    any.cpp
    proto_options.cpp
    string.cpp
    parse_parenthesis.cpp
    fs.cpp
    pretty_print.cpp
    protobuf.cpp
    json.cpp
    thread.cpp
    random.cpp
    rand.cpp
    ptr.cpp
    stat_gatherer.cpp
    matrix.cpp
)


set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(GROUND_SRCS ${GROUND_SRCS} ${DIR_SRCS})

set(DIRS
    log
    serial
    server
    ts
    protos
    contrib
)

foreach(dir ${DIRS})
  add_subdirectory(${dir})
endforeach()

add_library(ground SHARED ${GROUND_SRCS})

include_directories(${PROTOBUF_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${CMAKE_BINARY_DIR})

SET_TARGET_PROPERTIES(ground PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(ground
    groundpb
    ${PROTOBUF_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    m
)

install(TARGETS ground  DESTINATION lib/ COMPONENT)

install(
    DIRECTORY
    ${PROJECT_SOURCE_DIR}/ground
    DESTINATION
    include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)
install(
    DIRECTORY
    ${CMAKE_BINARY_DIR}/ground/protos
    DESTINATION
    include/ground
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

PATCH_APPLE(lib/libground.dylib)