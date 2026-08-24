cmake_minimum_required(VERSION 3.20)
project(novagen VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(novagen
    src/main.cpp
    src/model.cpp
    src/generator.cpp
    src/flask_fastapi_generators.cpp
    src/react_generators.cpp
    src/angular_generators.cpp
    src/springboot_generators.cpp
    src/android_generators.cpp
)

target_include_directories(novagen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/novagen
    ${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries(novagen PRIVATE
    ${SQLITE3_LIBRARIES}
)

install(TARGETS novagen RUNTIME DESTINATION bin)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
