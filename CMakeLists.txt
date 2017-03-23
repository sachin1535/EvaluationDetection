cmake_minimum_required(VERSION 2.8)
project(EvaluationDetectionPerformance-C++)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(OpenCV REQUIRED)
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(eval ${SOURCES})
target_link_libraries(eval ${OpenCV_LIBS})

