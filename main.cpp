#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// CONSTANTS
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char* vertexShaderSource1 = "./shaders/vertex/vertex_shader_1.vs";

const char* vertexShaderSource2 = "./shaders/vertex/vertex_shader_2.vs";

const char* fragShaderSource1 = "./shaders/fragment/fragment_shader_1.fs";

const char* fragShaderSource2 = "./shaders/fragment/fragment_shader_2.fs";

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

    Shader ShadersProgram2(vertexShaderSource2, fragShaderSource2);


    float vertices1[] = 
    {
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    };

    unsigned int indices1[] = {
        2, 1, 0,
    };

    unsigned int VBO1, VAO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

    float vertices2[] = {
        -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };

    unsigned int indices2[] = {
        0, 1, 2,
    };

    unsigned int VBO2, VAO2, EBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Цикл рендерингу
    while (!glfwWindowShouldClose(window))
    {
        // Обробка вводу
        processInput(window);

        // Виконування рендирингу
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ShadersProgram1.use();
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ShadersProgram2.use();
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // glfw: обмін вмістом front- і back-буферів. Відслідковування I/O подій.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // glfw: завершення, звільненнф всіх раніше задіяних ресурсів
    glfwTerminate();
    return 0;
}

// Обробка всіх подій вводу
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: щоразу, коли змінюються розміри вікна (користувачем або операційною системою), викликається дана callback-функція
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Переконуємось, що вікно перегляду відповідає новим розмірам вікна. 
    glViewport(0, 0, width, height);
}