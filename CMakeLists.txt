cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(FindThreads)

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(lab5_7)
add_subdirectory(tests)
add_subdirectory(cp)