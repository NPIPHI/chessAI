#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "loadShader.h"
#include "texture.hpp"
#include "board.h"
#include "ai.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <windows.h>
#endif

//these are in the global scope for emscripten compatibility
GLFWwindow* window;
GLuint boardProgram;
GLuint pieceProgram;
GLuint boardVertex;
GLuint piecesVertex;
GLuint vertexArrayID;
GLuint piecesTexture;
GLuint pieceVertexLocation;
GLuint pieceUVLocation;

struct {
    square start;
    square end;
    bool selectedStart;
} mouse;

std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

bool dirty = true;
board mainBoard;
int move;

void cleanUp();
void mainLoop();
void glInit();

void mouseCallback(GLFWwindow *, int, int, int);

int main( void ){
    glInit();

    piecesTexture = loadBMP_custom("res/piecesSmall.bmp");

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create and compile our GLSL program from the shaders
    boardProgram = LoadShaders("res/boardVertex.glsl", "res/boardFragment.glsl" );
    pieceProgram = LoadShaders("res/pieceVertex.glsl", "res/pieceFragment.glsl" );

    pieceVertexLocation = glGetAttribLocation(pieceProgram, "vertexPosition");
    pieceUVLocation = glGetAttribLocation(pieceProgram, "uv");

    //fill vertex buffer with triangle
    static const GLfloat boardVertexData[] = {
            -1.0f, -1.0f,
            -1.0f, 3.0f,
            3.0f,  -1.0f,
    };

    glGenBuffers(1, &boardVertex);
    glBindBuffer(GL_ARRAY_BUFFER, boardVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardVertexData), boardVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &piecesVertex);

    mainBoard = {};
    mainBoard.setDefault();

    glfwSetMouseButtonCallback(window, mouseCallback);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
	do{
	    mainLoop();
	    Sleep(16);
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
    cleanUp();
#endif
    return 0;
}

void mouseCallback(GLFWwindow * clickedWindow, int button, int action, int mods){
    if(clickedWindow == window){
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double mouseX, mouseY;
            int windowWidth, windowHeight;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            square selected(char((1-mouseY/windowWidth) * 8), char(mouseX/windowWidth * 8));
            if(!mouse.selectedStart){
                mouse.selectedStart = true;
                mouse.start = selected;
                std::cout << "on" << std::endl;
            } else {
                mouse.selectedStart = false;
                mouse.end = selected;
                if(mainBoard.isValid({mouse.start, mouse.end}, white)){
                    mainBoard = mainBoard.applyMove({mouse.start, mouse.end});
                    std::cout << mainBoard.print() << std::endl;
                    mainBoard = mainBoard.applyMove(ai::bestMove(mainBoard, 5, black));
                    std::cout << mainBoard.print() << std::endl;
                    dirty = true;
                }
                std::cout << "off" << std::endl;
            }
        }
    }
}

void glInit(){
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_SAMPLES, 4);
//    glEnable(GL_MULTISAMPLE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 768, 768, "Chess", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

}

void mainLoop() {
    if(dirty) {
        dirty = false;

        auto pieceBuffer = mainBoard.toBuffer();
        int pieceVertexCount = pieceBuffer.size()/4;

        glBindBuffer(GL_ARRAY_BUFFER, piecesVertex);
        glBufferData(GL_ARRAY_BUFFER, pieceBuffer.size() * sizeof(float), pieceBuffer.data(), GL_STATIC_DRAW);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Use our shader
        glUseProgram(boardProgram);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, boardVertex);
        glVertexAttribPointer(
                0,                  // location of our vertex attribute
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized, does not matter when input type is float
                0,                  // stride
                (void *) 0            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(0);

        glUseProgram(pieceProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, piecesTexture);
        glUniform1i(GL_TEXTURE_2D, 0);

        glEnableVertexAttribArray(pieceVertexLocation);
        glBindBuffer(GL_ARRAY_BUFFER, piecesVertex);
        glVertexAttribPointer(
                pieceVertexLocation,                  // location of our vertex attribute
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized, does not matter when input type is float
                16,                  // stride in bytes
                (void *) 0            // array buffer offset
        );

        glEnableVertexAttribArray(pieceUVLocation);
        glBindBuffer(GL_ARRAY_BUFFER, piecesVertex);
        glVertexAttribPointer(
                pieceUVLocation,                  // location of our vertex attribute
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized, does not matter when input type is float
                16,                  // stride in bytes
                (void *) 8          // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, pieceVertexCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
    }
    glfwPollEvents();
}

void cleanUp(){
    // Cleanup VBO
    glDeleteBuffers(1, &boardVertex);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(boardProgram);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}