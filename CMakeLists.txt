# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Chess)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)

add_executable(main src/main.cpp src/loadShader.cpp src/loadShader.h src/sourceDir.h src/texture.cpp src/texture.hpp src/board.cpp src/board.h src/piece.cpp src/piece.h src/chessMove.cpp src/chessMove.h src/ai.cpp src/ai.h)

if(EMSCRIPTEN)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/res")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/res")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/res")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/res")

else()

find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()


# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
    add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
        external/AntTweakBar-1.16/include/
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glew-1.13.0/include/
        external/assimp-3.0.1270/include/
        external/bullet-2.81-rev2613/src/
)

set(PROJECT_SOURCE_DIR C:/Users/16182/Documents/CPPStuff/ogl-master/ogl-master)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
        )

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)


# Simple Game

target_link_libraries(main
        ${ALL_LIBS}
        )

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )

endif()

