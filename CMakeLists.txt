cmake_minimum_required(VERSION 3.14)
project(AudioDelayEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(googletest)

add_executable(
    audio_delay_engine_test
    test.cpp
)

target_link_libraries(
    audio_delay_engine_test
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(audio_delay_engine_test)