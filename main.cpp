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
#include "mesh.h"
#include "material.h"
#include "light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_roll_callback(GLFWwindow* window, double xpos, double ypos);

// CONSTANTS
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const char* vertexShaderSource1 = "./shaders/vertex/vertex_shader_1.vs";
const char* fragShaderSource1 = "./shaders/fragment/fragment_shader_1.fs";
const char* vertexShaderLightSource = "./shaders/vertex/vertex_shader_light.vs";
const char* fragShaderLightSource = "./shaders/fragment/fragment_shader_light.fs";
const char* textureSource1 = "./res/texture.jpg";
const char* textureSource2 = "./res/awesomeface.png";

// Variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT  / 2.0f;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool showTextures = true; 
bool tKeyPressed = false; 

int main() {
    // glfw: ініціалізація та конфігурація
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

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
    Shader LightCubeShader(vertexShaderLightSource, fragShaderLightSource);

    // CUBE
    glm::vec3 cubePositions[] = {
        glm::vec3( 1.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  0.0f,  0.0f), 
        glm::vec3( 3.0f,  0.0f,  0.0f),  
        glm::vec3( 4.0f,  0.0f,  0.0f),  
        glm::vec3( 5.0f,  0.0f, 0.0f),  
        glm::vec3( 6.0f,  0.0f,  0.0f),  
        glm::vec3( 7.0f,  0.0f,  0.0f),  
        glm::vec3( 8.0f,  0.0f,  0.0f), 
        glm::vec3( 9.0f,  0.0f,  0.0f), 
        glm::vec3( 10.0f,  0.0f, 0.0f) 
    };

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // ========================== LIGHT ===========================
    glm::vec3 lightPosition = glm::vec3(7.0f, 3.0f, 0.0f);

    Light mainLight;
    mainLight.position = lightPosition;
    mainLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    mainLight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    mainLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    mainLight.constant = 1.0f;
    mainLight.linear = 0.09f;
    mainLight.quadratic = 0.032f;

    // ============ Налаштування текстур ==============
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Texture texture1(textureSource1);
    Texture texture2(textureSource2);
    std::vector<Texture*> cubeTextures = {&texture1, &texture2};

    Material cubeMaterials[] = {
        Materials::Gold,
        Materials::Silver,
        Materials::Emerald,
        Materials::Bronze,
        Materials::Copper,
        Materials::Jade,
        Materials::Pearl,
        Materials::RedPlastic,
        Materials::BlackPlastic,
        Materials::WhiteRubber
    };

    std::vector<Cube> cubes;
    cubes.reserve(std::size(cubePositions));

    for (size_t i = 0; i < std::size(cubePositions); i++) {
        cubes.emplace_back(
            cubePositions[i], 
            glm::vec3(1.0f), 
            glm::vec3(1.0f), 
            ShadersProgram1, 
            cubeTextures,
            cubeMaterials[i % std::size(cubeMaterials)],
            true
        );
    }

    Cube lightCube(
        lightPosition, 
        glm::vec3(0.5f), 
        glm::vec3(1.0f), 
        LightCubeShader, 
        {},
        Materials::Pearl,
        false
    );

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

        // Виконування рендиринга
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Малюємо всі куби з освітленням
        for(int i = 0; i < std::size(cubePositions); i++){
            cubes[i].showTex = showTextures;
            cubes[i].draw(view, projection, mainLight, camera.Position);
        }

        // Малюємо джерело світла
        lightCube.draw(view, projection, mainLight, camera.Position);

        // glfw: обмін вмістом front- і back-буферів. Відслідковування I/O подій.
        glfwSwapBuffers(window);
        glfwPollEvents();  
    }

    cubes.clear();
    cubeTextures.clear();

    // glfw: завершення, звільнення всіх раніше задіяних ресурсів
    glfwTerminate();
    return 0;
}

// Обробка всіх подій вводу
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Toggle текстур
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tKeyPressed)
    {
        showTextures = !showTextures;
        tKeyPressed = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
    {
        tKeyPressed = false;
    }

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_roll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}