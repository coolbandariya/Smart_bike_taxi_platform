cmake_minimum_required(VERSION 3.16)

project(SmartBikeTaxiPlatform VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(smart_bike_taxi main.cpp)
target_include_directories(smart_bike_taxi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(smart_bike_taxi_tests tests.cpp)
target_include_directories(smart_bike_taxi_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target smart_bike_taxi smart_bike_taxi_tests)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endforeach()
endif()

enable_testing()
add_test(NAME smart_bike_taxi_tests COMMAND smart_bike_taxi_tests)
