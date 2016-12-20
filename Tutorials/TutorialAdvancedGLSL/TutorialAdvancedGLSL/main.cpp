// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "../../../Engines/Shader.h"
#include "../../../Engines/Camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
    
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    // Setup and compile our shaders
    Shader shaderRed(SHADER_FULL_DIR"ubo.vs", SHADER_FULL_DIR"red.frag");
    Shader shaderGreen(SHADER_FULL_DIR"ubo.vs", SHADER_FULL_DIR"green.frag");
    Shader shaderBlue(SHADER_FULL_DIR"ubo.vs", SHADER_FULL_DIR"blue.frag");
    Shader shaderYellow(SHADER_FULL_DIR"ubo.vs", SHADER_FULL_DIR"yellow.frag");
    
#pragma region "object_initialization"
    
    GLfloat cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f
    };
    
    // Setup cube VAO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
#pragma endregion
    
    // Create a uniform buffer object
    // First. We get the relevant block indices
    GLuint uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.Program, "Matrices");
    GLuint uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.Program, "Matrices");
    GLuint uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.Program, "Matrices");
    GLuint uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.Program, "Matrices");
    // Then we link each shader's uniform block to this uniform binding point
    glUniformBlockBinding(shaderRed.Program, uniformBlockIndexRed, 0);
    glUniformBlockBinding(shaderGreen.Program, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(shaderBlue.Program, uniformBlockIndexBlue, 0);
    glUniformBlockBinding(shaderYellow.Program, uniformBlockIndexYellow, 0);
    // Now actually create the buffer
    GLuint uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        // just ask for the buffer space without fill it
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    // Define the range of the buffer that links to a uniform binding point
    // first 0 (second argument means index as line 143 144 145 146 set second 0 is offset)
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    
    // Store the projection matrix (we only have to do this once) (note: we're not using zoom anymore by changing the FoV. We only create the projection matrix once now)
    glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        // first part is proj matrix as we declared in ubo.vs
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set the view and projection matrix in the uniform block - we only have to do this once per loop iteration.
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        // Draw 4 cubes
        // RED
        glBindVertexArray(cubeVAO);
        shaderRed.Use();
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));	// Move top-left
        glUniformMatrix4fv(glGetUniformLocation(shaderRed.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // GREEN
        shaderGreen.Use();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));		// Move top-right
        glUniformMatrix4fv(glGetUniformLocation(shaderGreen.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // BLUE
        shaderBlue.Use();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));	// Move bottom-left
        glUniformMatrix4fv(glGetUniformLocation(shaderBlue.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // YELLOW
        shaderYellow.Use();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));	// Move bottom-right
        glUniformMatrix4fv(glGetUniformLocation(shaderYellow.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
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