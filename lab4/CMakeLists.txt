set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lab4/include)

add_library(lib1_static STATIC src/lib1.cpp ${INCLUDE_DIR}/lib.h src/utils.cpp ${INCLUDE_DIR}/utils.h)
add_library(lib2_static STATIC src/lib2.cpp ${INCLUDE_DIR}/lib.h src/utils.cpp ${INCLUDE_DIR}/utils.h)
add_library(lib1 SHARED src/lib1.cpp ${INCLUDE_DIR}/lib.h src/utils.cpp ${INCLUDE_DIR}/utils.h)
add_library(lib2 SHARED src/lib2.cpp ${INCLUDE_DIR}/lib.h src/utils.cpp ${INCLUDE_DIR}/utils.h)

target_include_directories(lib1_static PUBLIC ${INCLUDE_DIR})
target_include_directories(lib1 PUBLIC ${INCLUDE_DIR})

target_include_directories(lib2_static PUBLIC ${INCLUDE_DIR})
target_include_directories(lib2 PUBLIC ${INCLUDE_DIR})

add_executable(static_main static_main.cpp)
add_executable(dynamic_main dynamic_main.cpp)

target_link_libraries(static_main PRIVATE lib1_static)