set(INC_DIR ${CMAKE_SOURCE_DIR}/cp/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/cp/src)

add_executable(game_server ${SRC_DIR}/server.cpp ${SRC_DIR}/utils.cpp 
            ${INC_DIR}/utils.h ${SRC_DIR}/shared_memory.cpp ${INC_DIR}/shared_memory.h)
target_include_directories(game_server PRIVATE ${INC_DIR})

add_executable(game_client ${SRC_DIR}/client.cpp ${SRC_DIR}/utils.cpp 
            ${INC_DIR}/utils.h ${SRC_DIR}/shared_memory.cpp ${INC_DIR}/shared_memory.h)
target_include_directories(game_client PRIVATE ${INC_DIR})

target_compile_features(game_client PRIVATE cxx_std_20)
target_compile_features(game_server PRIVATE cxx_std_20)
