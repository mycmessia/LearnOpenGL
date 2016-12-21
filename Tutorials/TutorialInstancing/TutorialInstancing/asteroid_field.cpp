//
//  main.cpp
//  TutorialInstancing
//
//  Created by 梅宇宸 on 16/12/21.
//  Copyright © 2016年 梅宇宸. All rights reserved.
//

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "../../../Engines/Shader.h"
#include "../../../Engines/Camera.h"
#include "../../../Engines/Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 55.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

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
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();
    
    int width, height;
    
    // This step is a must for retina maybe
    glfwGetFramebufferSize (window, &width, &height);
    glViewport (0, 0, width, height);
    
    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    // Setup and compile our shaders
    Shader shader(SHADER_FULL_DIR"instancing_asteroid.vs", SHADER_FULL_DIR"instancing_asteroid.frag");
    
    // Load models
    Model rock(MODEL_FULL_DIR"rock/rock.obj");
    Model planet(MODEL_FULL_DIR"planet/planet.obj");
    
    // Set projection matrix
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 10000.0f);
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Generate a large list of semi-random model transformation matrices
    GLuint amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed
    GLfloat radius = 50.0;
    GLfloat offset = 2.5f;
    for(GLuint i = 0; i < amount; i++)
    {
        glm::mat4 model;
        // 1. Translation: displace along circle with 'radius' in range [-offset, offset]
        GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
        GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat x = sin(angle) * radius + displacement;
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat y = displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));
        
        // 2. Scale: Scale between 0.05 and 0.25f
        GLfloat scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));
        
        // 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
        GLfloat rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        
        // 4. Now add to list of matrices
        modelMatrices[i] = model;
    }
    
    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents();
        Do_Movement();
        
        // Clear buffers
        glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Add transformation matrices
        shader.Use();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        
        // Draw Planet
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        planet.Draw(shader);
        
        // Draw meteorites
        for(GLuint i = 0; i < amount; i++)
        {
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrices[i]));
            rock.Draw(shader);
        }
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}	

#pragma endregion