find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(lab1_test lab1_test.cpp ../lab1/include/lab1.h ../lab1/src/lab1.c)
target_link_libraries(lab1_test GTest::gtest GTest::gtest_main)
target_include_directories(lab1_test PRIVATE ../lab1/include)
add_test(lab1_test lab1_test)

add_executable(lab2_test lab2_test.cpp ../lab2/include/blur.h ../lab2/src/blur.c)
target_link_libraries(lab2_test GTest::gtest GTest::gtest_main)
target_include_directories(lab2_test PRIVATE ../lab2/include)
add_test(lab2_test lab2_test)

add_executable(lab4_lib1_test lab4_lib1_test.cpp)
target_link_libraries(lab4_lib1_test GTest::gtest GTest::gtest_main lib1_static)
target_include_directories(lab4_lib1_test PRIVATE ../lab4/include)
add_test(lab4_lib1_test lab4_lib1_test)

add_executable(lab4_lib2_test lab4_lib2_test.cpp)
target_link_libraries(lab4_lib2_test GTest::gtest GTest::gtest_main lib2_static)
target_include_directories(lab4_lib2_test PRIVATE ../lab4/include)
add_test(lab4_lib2_test lab4_lib2_test)