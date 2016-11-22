cmake_minimum_required(VERSION 3.6)
project(tunescope)
include_directories(include)
set(SOURCES src/audio.cpp src/gui.cpp src/tone.cpp src/main.cpp)
add_executable(tunescope ${SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Jack REQUIRED)

include_directories(${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${JACK_INCLUDE_DIR})
target_link_libraries(tunescope ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${JACK_LIBRARIES})
