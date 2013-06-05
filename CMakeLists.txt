cmake_minimum_required(VERSION 2.8)

project (tcc CXX C)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

add_executable(heightmap_visualizer heightmap_visualizer.cc)

target_link_libraries(heightmap_visualizer ${OPENGL_LIBRARIES}
                                           "C:/MinGW/lib/libfreeglut.a")