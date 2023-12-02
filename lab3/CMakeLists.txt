add_executable(lab3 
               main.c 
               include/lab3.h src/lab3.c 
               include/utils.h src/utils.c 
               include/error_handling.h
               )

add_executable(child
               src/child.c
               include/utils.h src/utils.c 
               include/error_handling.h
               )

target_include_directories(lab3 PRIVATE include)
target_include_directories(child PRIVATE include)

add_dependencies(lab3 child)