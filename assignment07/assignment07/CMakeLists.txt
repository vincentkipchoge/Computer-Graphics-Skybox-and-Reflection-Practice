cmake_minimum_required(VERSION 3.17)
project(assign07)


link_libraries(cs4722  glfw3 opengl32 glu32)

set(CMAKE_CXX_STANDARD 20)

include_directories(cs4722lib/include C:/cs4722lib/include)
link_directories(cs4722lib C:/cs4722lib)

add_executable(assignment07 assignment07.cpp)

