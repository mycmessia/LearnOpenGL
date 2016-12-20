//
//  main.cpp
//  TutorialGeometryShader
//
//  Created by 梅宇宸 on 16/12/20.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "../../../Engines/Shader.h"

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // This is vital on MacOSX
    
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    
    // Options
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();
    
    int width, height;
    
    // This step is a must for retina maybe
    glfwGetFramebufferSize (window, &width, &height);
    glViewport (0, 0, width, height);
    
    // Setup and compile our shaders
    Shader shader(SHADER_FULL_DIR"geometry.vs", SHADER_FULL_DIR"geometry.frag", SHADER_FULL_DIR"geometry.gs");
    
    // Vertex data
    GLfloat points[] = {
        // positions  // colors
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // Bottom-left
    };
    
    GLuint VBO, VAO;
    
    glGenVertexArrays (1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray (VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
        // These 4 sentences must bind vao and vbo
        // which means these operations must affect this vao and vbo at the same time
        // In other words, these operations must both vao and vbo to work on
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Check and call events
        // This also make the while doesn't run too fast
        glfwPollEvents();
        
        // Clear buffers
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw points
        shader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}