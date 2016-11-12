function(fixpp_test test_name)
    set(TEST_EXECUTABLE run_${test_name})
    set(TEST_SOURCE ${test_name}.cc)

    add_executable(${TEST_EXECUTABLE} ${TEST_SOURCE})
    target_link_libraries(${TEST_EXECUTABLE} gtest gtest_main)
    add_test(${test_name} ${TEST_EXECUTABLE})
endfunction()

fixpp_test(message_test)
