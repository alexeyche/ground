cmake_minimum_required(VERSION 2.8)

set(SOURCES
    bin_serial.cpp
    serial_base.cpp
    proto_serial.cpp
    meta_proto_serial.cpp
    util.cpp
)

set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(GROUND_SRCS ${GROUND_SRCS} ${DIR_SRCS} PARENT_SCOPE)
