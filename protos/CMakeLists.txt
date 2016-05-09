file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

set(PROTOBUF_IMPORT_DIRS ${PROTOBUF_INCLUDE_DIRS})

PROTOBUF_GENERATE_CPP_DNN(PROTO_SRCS PROTO_HDRS PROTOFILES ${ProtoFiles} PROTOROOT ${PROJECT_SOURCE_DIR} OUTPATH ${CMAKE_BINARY_DIR})

add_library(groundpb SHARED ${ProtoFiles} ${PROTO_SRCS})

SET_TARGET_PROPERTIES(groundpb PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(groundpb
    ${PROTOBUF_LIBRARY}
)
install(TARGETS groundpb  DESTINATION lib/ COMPONENT)

install(
    DIRECTORY
    ${CMAKE_BINARY_DIR}/ground/protos
    DESTINATION
    include/ground
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.py"
)
install(CODE
    "EXECUTE_PROCESS(
        COMMAND 
        ${CMAKE_SOURCE_DIR}/scripts/touch.py ${CMAKE_INSTALL_PREFIX}/include/ground/__init__.py ${CMAKE_INSTALL_PREFIX}/include/ground/protos/__init__.py
    )"
)
