cmake_minimum_required(VERSION 2.8) 
project(genetic-ir CXX)

file(GLOB SOURCE *.cpp)

add_executable(genetic-ir ${SOURCE})
target_link_libraries(genetic-ir oi_static textio_static)
