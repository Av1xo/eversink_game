#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_roll_callback(GLFWwindow* window, double xpos, double ypos);

// CONSTANTS
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char* vertexShaderSource1 = "./shaders/vertex/vertex_shader_1.vs";
const char* fragShaderSource1 = "./shaders/fragment/fragment_shader_1.fs";
const char* textureSource1 = "./res/texture.jpg";
const char* textureSource2 = "./res/awesomeface.png";

// Variables
Camera camera;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = 400, lastY = 300;

int main() {
    // glfw: ініціалізація та конфігурація
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw: створення вікна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Eversink", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: завантаження всіх вказівників на OpenGL-функції
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int success;
    char infoLog[512];


    // SHADER PROGRAM
    Shader ShadersProgram1(vertexShaderSource1, fragShaderSource1);

    float vertices[] = {
        // --- Front face ---
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

        // --- Back face ---
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

        // --- Left face ---
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

        // --- Right face ---
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

        // --- Top face ---
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // --- Bottom face ---
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9,10,10,11, 8,       // Left
        12,13,14,14,15,12,       // Right
        16,17,18,18,19,16,       // Top
        20,21,22,22,23,20        // Bottom
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f) 
    };

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
        );


    // ============ Налаштування текстур ==============
    Texture texture1(textureSource1);
    Texture texture2(textureSource2);
    ShadersProgram1.use();
    ShadersProgram1.setInt("texture1", 0);
    ShadersProgram1.setInt("texture2", 1);
    glActiveTexture(GL_TEXTURE0);
    texture1.bind();
    glActiveTexture(GL_TEXTURE1);
    texture2.bind();


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_roll_callback);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Цикл рендерингу
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обробка вводу
        processInput(window);

        // Виконування рендирингу
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        ShadersProgram1.use();

        view = glm::lookAt(
            camera.cameraPos,
            camera.cameraPos + camera.cameraFront,
            camera.cameraUp
        );

        projection = glm::perspective(glm::radians(camera.fov), 800.0f/600.0f, 0.1f, 100.0f);

        ShadersProgram1.setMat4("view", view);
        ShadersProgram1.setMat4("projection", projection);

        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * (i+1);
            if(i % 3 == 0)
            {
                model = glm::rotate(model, glm::radians(angle *(float)glfwGetTime()), glm::vec3(1.0f, 0.3f, 0.5f));
            } 
            else
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            ShadersProgram1.setMat4("model", model);
                
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        // glfw: обмін вмістом front- і back-буферів. Відслідковування I/O подій.
        glfwSwapBuffers(window);
        glfwPollEvents();  
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    // glfw: завершення, звільненнф всіх раніше задіяних ресурсів
    glfwTerminate();
    return 0;
}

// Обробка всіх подій вводу
void processInput(GLFWwindow *window)
{
    camera.cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveForward();

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveBackward();

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft();

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveRight();
}

// glfw: щоразу, коли змінюються розміри вікна (користувачем або операційною системою), викликається дана callback-функція
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Переконуємось, що вікно перегляду відповідає новим розмірам вікна. 
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    camera.setVertAngle(89.0f, -89.0f);

    camera.setDirection();
}

void mouse_roll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.setFOV(xoffset, yoffset);
}