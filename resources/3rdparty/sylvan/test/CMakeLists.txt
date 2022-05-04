include(CTest)

add_executable(test_basic)
target_sources(test_basic PRIVATE test_basic.c)
target_link_libraries(test_basic PRIVATE Sylvan::Sylvan)
target_compile_features(test_basic PRIVATE c_std_11)
target_compile_options(test_basic PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(test_cxx)
target_sources(test_cxx PRIVATE test_cxx.cpp)
target_link_libraries(test_cxx PRIVATE Sylvan::Sylvan)
target_compile_features(test_cxx PRIVATE cxx_std_11)
target_compile_options(test_cxx PRIVATE -Wall -Wextra -Wno-deprecated)

add_test(test_basic test_basic)
add_test(test_cxx test_cxx)

add_code_coverage()
target_code_coverage(test_basic)
target_code_coverage(test_cxx)
