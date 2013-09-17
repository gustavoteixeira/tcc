cmake_minimum_required(VERSION 2.8)

project (tcc CXX C)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

add_definitions(-DGLEW_STATIC)
add_subdirectory(glew)
include_directories("glew/include")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(heightmap_visualizer heightmap_visualizer.cc mapprocessor.cc opengl/Exception.cc opengl/vertexbuffer.cc)

target_link_libraries(heightmap_visualizer
                                           ${GLEW_LIBRARY}
                                           ${OPENGL_LIBRARIES}
                                           "C:/MinGW/lib/libfreeglut.a")