find_package(Benchmark)

function(fixpp_test test_name)
    set(TEST_EXECUTABLE run_${test_name})
    set(TEST_SOURCE ${test_name}.cc)

    add_executable(${TEST_EXECUTABLE} ${TEST_SOURCE})
    target_link_libraries(${TEST_EXECUTABLE} gtest gtest_main)
    add_test(${test_name} ${TEST_EXECUTABLE})
endfunction()

function(fixpp_bench bench_name)
    if (BENCHMARK_FOUND)
        set(BENCH_EXECUTABLE run_${bench_name})
        set(BENCH_SOURCE ${bench_name}.cc)

        add_executable(${BENCH_EXECUTABLE} ${BENCH_SOURCE})
        target_link_libraries(${BENCH_EXECUTABLE} benchmark pthread)
    endif()
endfunction()

fixpp_test(message_test)
fixpp_test(visitor_test)
fixpp_test(writer_test)

add_executable(run_bench_test bench_test.cc)
# configure_file(${CMAKE_SOURCE_DIR}/tests/data/fix42.log ${CMAKE_CURRENT_BINARY_DIR}/data/fix42.log COPYONLY)

fixpp_bench(serialize_bench)
fixpp_bench(visit_bench)
fixpp_bench(misc_bench)
