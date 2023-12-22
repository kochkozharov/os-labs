set(INC_DIR ${CMAKE_SOURCE_DIR}/lab5/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/lab5/src)

add_library(zmqsocket STATIC ${SRC_DIR}/socket.cpp ${INCLUDE_DIR}/socket.h)
add_library(zmqnode STATIC ${SRC_DIR}/node.cpp ${INCLUDE_DIR}/node.h ${SRC_DIR}/topology.cpp ${INCLUDE_DIR}/topology.h)

target_link_libraries(zmqnode zmqsocket)
target_include_directories(zmqsocket${INCLUDE_DIR})
target_include_directories(zmqnode ${INCLUDE_DIR})

add_executable(client ${SRC_DIR}/client.cpp ${INCLUDE_DIR}/node.h)
add_executable(server ${SRC_DIR}/server.cpp ${INCLUDE_DIR}/node.h)
target_include_directories(client PRIVATE ${INCLUDE_DIR})
target_include_directories(server PRIVATE ${INCLUDE_DIR})

target_link_libraries(client zmq zmqnode)
target_link_libraries(server zmq zmqnode)