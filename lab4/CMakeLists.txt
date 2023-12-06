set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lab4/include)

add_library(lib1 SHARED src/lib1.c ${INCLUDE_DIR}/lib.h)
add_library(lib2 SHARED src/lib2.c ${INCLUDE_DIR}/lib.h)

target_include_directories(lib1 PUBLIC ${INCLUDE_DIR})
target_include_directories(lib2 PUBLIC ${INCLUDE_DIR})

add_executable(static_main static_main.cpp)
add_executable(dynamic_main dynamic_main.cpp)

target_link_libraries(static_main PRIVATE lib1)