add_executable(lab1 
               main.c 
               include/lab1.h src/lab1.c 
               include/utils.h src/utils.c 
               include/error_handling.h
               )

add_executable(child
               src/child.c
               include/utils.h src/utils.c 
               include/error_handling.h
               )

target_include_directories(lab1 PRIVATE include)
target_include_directories(child PRIVATE include)

add_dependencies(lab1 child)