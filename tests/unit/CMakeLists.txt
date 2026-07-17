add_executable(test_engine
        main.cpp
        test_telex.cpp
        test_engine.cpp
)
target_link_libraries(test_engine PRIVATE engine)
add_test(NAME core COMMAND test_engine)

