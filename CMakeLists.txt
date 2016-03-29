cmake_minimum_required(VERSION 2.8.4)
project(imageTexDemo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (NOT WIN32)
set(CMAKE_MODULE_PATH /usr/local/lib/cmake /usr/local/lib/x86_64-linux-gnu/cmake )
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/glfw)
endif (NOT WIN32)

find_package (PkgConfig REQUIRED)
find_package (OpenGL REQUIRED)

if (WIN32)
  include_directories("C:/GLFW/include" "C:/glm" "C:/glew/include")
  link_directories("C:/glew/lib/Release/x64" "C:/GLFW/lib-mingw" )
else (WIN32)
  find_package (GLEW REQUIRED STATIC)
  find_package (glfw3 REQUIRED)
  #find_package (GLM REQUIRED)
endif (WIN32)
if (APPLE)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVID_LIBRARY CoreVideo REQUIRED)
  add_definitions(-DAPPLE -I/usr/local/include) 
endif (APPLE)

set(SOURCE_FILES main.cpp gl_utils.cpp maths_funcs.cpp)

## "runDemo" is the name of the executable
add_executable(runDemo ${SOURCE_FILES})
  target_link_libraries (runDemo
          ${GLFW3_LIBRARY}
          ${OPENGL_LIBRARIES}
          ${GLEW_LIBRARY}
          ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY})
if (WIN32)
  target_link_libraries (runDemo
          ${OPENGL_LIBRARIES} glfw3 glew32s glu32 opengl32)
endif (WIN32)

#if (UNIX)
#  target_link_libraries (runDemo
#          ${OPENGL_LIBRARIES}
#          ${GLFW3_LIBRARY}
#          ${GLEW_LIBRARY}
#          Xxf86vm pthread Xrandr Xinerama Xi Xcursor)
#endif (UNIX)
