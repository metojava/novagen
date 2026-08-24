cmake_minimum_required(VERSION 3.20)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(novagen_tests
    test_model.cpp
    test_generator.cpp
)

target_include_directories(novagen_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries(novagen_tests PRIVATE
    novagen
    gtest_main
    ${SQLITE3_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(novagen_tests)
