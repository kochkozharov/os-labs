add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(lab1_test lab1_test.cpp ../lab1/include/lab1.h ../lab1/src/lab1.c)
target_link_libraries(lab1_test PRIVATE GTest::GTest)
target_include_directories(lab1_test PRIVATE ../lab1/include)
add_test(lab1_test lab1_test)

add_executable(lab2_test lab2_test.cpp ../lab2/include/blur.h ../lab2/src/blur.c)
target_link_libraries(lab2_test PRIVATE GTest::GTest)
target_include_directories(lab2_test PRIVATE ../lab2/include)
add_test(lab2_test lab2_test)
