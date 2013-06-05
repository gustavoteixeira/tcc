cmake_minimum_required(VERSION 2.8)

project (tcc CXX C)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories("glew/include")


add_executable(heightmap_visualizer heightmap_visualizer.cc)

target_link_libraries(heightmap_visualizer ${OPENGL_LIBRARIES}
                                           ${CMAKE_CURRENT_SOURCE_DIR}/glew32.lib
                                           "C:/MinGW/lib/libfreeglut.a")