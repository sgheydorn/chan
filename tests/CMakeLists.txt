add_executable(tests main.cpp)
target_link_libraries(tests chan)

add_test(NAME AllTests COMMAND tests)
