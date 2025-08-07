#specify MinGW Makefile usage instead of default MSVC
#cmake -DCMAKE_BUILD_TYPE=Debug ../ -G "MinGW Makefiles"

#build exe but clean first
#cmake --build . --clean-first

cmake_minimum_required(VERSION 3.10)  # Minimum version of CMake required

project (wavemage)

set(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")

add_compile_options(-m64)

include_directories(C:/Users/user/proj/headers/include/SDL2)
include_directories(C:/Users/user/proj/headers/include/imgui-master)
include_directories(C:/Users/user/proj/headers/include/implot-master)
include_directories(C:/Users/user/proj/headers/include/imgui-master/backends)
include_directories(C:/Users/USER/proj/headers/include/ma)

#include_directories(C:/Users/user/proj/headers/include/raudio)
#include_directories(C:/Users/user/proj/headers/include/raudio/external)

link_directories(C:/Users/user/proj/lib)

set(CMAKE_PREFIX_PATH "C:/Users/user/proj/headers/lib/cmake/SDL2")

find_package(SDL2 REQUIRED)

file(GLOB SOURCES 
                 #${CMAKE_CURRENT_SOURCE_DIR}/headers/include/raudio/external/miniaudio.h
                  ${CMAKE_CURRENT_SOURCE_DIR}headers/include/ma/miniaudio.h
                 #${CMAKE_CURRENT_SOURCE_DIR}/headers/include/raudio/raudio.h
                 #${CMAKE_CURRENT_SOURCE_DIR}/headers/include/raudio/raudio.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/*.h 
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/implot-master/*.h 
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/implot-master/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_sdl2.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_sdl2.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_opengl2.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/headers/include/imgui-master/backends/imgui_impl_opengl2.cpp)

set(WAVEMAGE_SRCS main.cpp ${SOURCES})

set(CMAKE_EXE_LINKER_FLAGS "--static -static-libgcc -static-libstdc++")  #static linking

add_executable(${PROJECT_NAME} ${WAVEMAGE_SRCS})

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 OpenGL32)

if(WIN32)
    #set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-mconsole")
    message("Windows Detected")
    message("Building For Windows...")
else()
    set(WAVEMAGE_SRCS ${WAVEMAGE_SRCS} UnixSupport.c)
endif() 