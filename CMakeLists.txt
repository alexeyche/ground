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

SET_TARGET_PROPERTIES(ground PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(ground
    ${PROTOBUF_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    m
)
