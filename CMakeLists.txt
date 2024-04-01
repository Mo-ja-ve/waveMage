cmake_minimum_required(VERSION 3.10)  # Minimum version of CMake required

project (wavemage)

include_directories(C:/Users/user/proj/headers/include/SDL2)
include_directories(C:/Users/user/proj/headers/include/imgui-master)
include_directories(C:/Users/user/proj/headers/include/imgui-master/backends)

link_directories(C:/Users/user/proj/lib)

set(CMAKE_PREFIX_PATH "C:/Users/user/proj/headers/lib/cmake/SDL2")

find_package(SDL2 REQUIRED)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/*.h 
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_sdl2.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_sdl2.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_opengl2.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_opengl2.cpp)

set(WAVEMAGE_SRCS main.cpp ${SOURCES})

add_executable(${PROJECT_NAME} ${WAVEMAGE_SRCS})

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 OpenGL32 )

if(WIN32)
    #set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-mconsole")
    message("Windows Detected")
    message("Building For Windows...")
else()
    set(WAVEMAGE_SRCS ${WAVEMAGE_SRCS} UnixSupport.c)
endif()
