include(CTest)

add_executable(test_basic)
target_sources(test_basic PRIVATE test_basic.c)
target_link_libraries(test_basic PRIVATE sylvan::sylvan)
target_compile_features(test_basic PRIVATE c_std_11)
target_compile_options(test_basic PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(test_cxx)
target_sources(test_cxx PRIVATE test_cxx.cpp)
target_link_libraries(test_cxx PRIVATE sylvan::sylvan)
target_compile_features(test_cxx PRIVATE cxx_std_11)
target_compile_options(test_cxx PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(test_zdd)
target_sources(test_zdd PRIVATE test_zdd.c)
target_link_libraries(test_zdd PRIVATE sylvan::sylvan)
target_compile_features(test_zdd PRIVATE c_std_11)
target_compile_options(test_zdd PRIVATE -Wall -Wextra -Werror -Wno-deprecated)

add_test(test_basic test_basic)
add_test(test_cxx test_cxx)
add_test(test_zdd test_zdd)
