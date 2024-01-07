cmake_minimum_required(VERSION 3.5)
project(ComputerGraphics VERSION 0.1.0)

set(APP_NAME ComputerGraphics)
set(CMAKE_CXX_STANDARD 17)


add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL_image/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glm)

add_executable(${APP_NAME} main.cpp)

set_property(TARGET SDL2 PROPERTY INTERFACE_SDL2_SHARED FALSE)
set_property(TARGET SDL2_image PROPERTY INTERFACE_SDL2_SHARED FALSE)

target_link_libraries(${APP_NAME} SDL2-static SDL2_image)