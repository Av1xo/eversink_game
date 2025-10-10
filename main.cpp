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
Camera camera(glm::vec3(0.0f, 4.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -5.0f);
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
    // РЯД 1: Метали (Яскраві відбиття, низька шорсткість) - Y = 0.0f
    glm::vec3(-5.0f, 0.0f, 0.0f),  // Gold
    glm::vec3(-2.5f, 0.0f, 0.0f),  // Silver
    glm::vec3( 0.0f, 0.0f, 0.0f),  // Bronze
    glm::vec3( 2.5f, 0.0f, 0.0f),  // Chrome
    glm::vec3( 5.0f, 0.0f, 0.0f),  // Copper
    
    // РЯД 2: Опакові (Непрозорі) Діелектрики - Y = 2.5f
    glm::vec3(-5.0f, 2.5f, 0.0f),  // BlackPlastic
    glm::vec3(-2.5f, 2.5f, 0.0f),  // RedPlastic
    glm::vec3( 0.0f, 2.5f, 0.0f),  // WhiteRubber (Висока шорсткість)
    glm::vec3( 2.5f, 2.5f, 0.0f),  // RoughIron (Хоча метал, тут для контрасту)
    glm::vec3( 5.0f, 2.5f, 0.0f),  // Pearl (Слабо-напівпрозорий/матовий)

    // РЯД 3: Прозорі та Напівпрозорі (З низькою шорсткістю) - Y = 5.0f
    glm::vec3(-5.0f, 5.0f, 0.0f),  // Glass
    glm::vec3(-2.5f, 5.0f, 0.0f),  // ColoredGlass
    glm::vec3( 0.0f, 5.0f, 0.0f),  // Crystal
    glm::vec3( 2.5f, 5.0f, 0.0f),  // Ice
    glm::vec3( 5.0f, 5.0f, 0.0f),  // Water

    // РЯД 4: Спеціальні та Кольорові Прозорі (Для кольорової передачі) - Y = 7.5f
    glm::vec3(-5.0f, 7.5f, 0.0f),  // Amber
    glm::vec3(-2.5f, 7.5f, 0.0f),  // Emerald
    glm::vec3( 0.0f, 7.5f, 0.0f),  // Jade
    glm::vec3( 2.5f, 7.5f, 0.0f),  // FrostedGlass (Для контрасту з Glass)
    glm::vec3( 5.0f, 7.5f, 0.0f)   // WhiteRubber (Duplicate)
    };

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // ========================== LIGHT ===========================
    glm::vec3 lightPosition = glm::vec3(0.0f, 7.0f, 5.0f);

    // Light mainLight = Lights::CreateDirectional(
    //     glm::vec3(-0.2f, -1.0f, -0.3f),  // Напрямок світла (зверху-справа)
    //     glm::vec3(0.08f, 0.08f, 0.08f),     // Ambient
    //     glm::vec3(0.8f, 0.8f, 0.8f),     // Diffuse
    //     glm::vec3(1.0f, 1.0f, 1.0f)      // Specular
    // );

    Light mainLight = Lights::CreatePoint(
        glm::vec3(0.0f, 7.0f, 5.0f),                    // Позиція
        glm::vec3(0.5f),     // Ambient
        glm::vec3(7.0f, 7.0f, 7.0f),     // Diffuse
        glm::vec3(1.0f, 5.0f, 1.0f),     // Specular
        1.0f,                             // Constant
        0.09f,                            // Linear
        0.032f                            // Quadratic
    );

    // Light mainLight = Lights::CrateSpot(
    //     camera.Position,                  // Позиція (в позиції камери)
    //     camera.Front,                     // Напрямок (куди дивиться камера)
    //     glm::vec3(0.1f, 0.1f, 0.1f),     // Ambient
    //     glm::vec3(1.0f, 1.0f, 1.0f),     // Diffuse
    //     glm::vec3(1.0f, 1.0f, 1.0f),     // Specular
    //     glm::radians(12.5f),              // Внутрішній кут
    //     glm::radians(17.5f)               // Зовнішній кут
    // );

    // ============ Налаштування текстур ==============
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Texture texture1(textureSource1);
    Texture texture2(textureSource2);
    std::vector<Texture*> cubeTextures = {&texture1, &texture2};

    Material cubeMaterials[] = {
        Materials::Amber,
        Materials::BlackPlastic,
        Materials::Bronze,
        Materials::Chrome,
        Materials::ColoredGlass,
        Materials::Copper,
        Materials::Crystal,
        Materials::Emerald,
        Materials::FrostedGlass,
        Materials::Glass,
        Materials::Gold,
        Materials::Ice,
        Materials::Jade,
        Materials::Pearl,
        Materials::RedPlastic,
        Materials::RoughIron,
        Materials::Silver,
        Materials::Water,
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

        // Для прожектора треба оновлювати позицію і напрямок в циклі:
        if (mainLight.type == LightType::SPOT) {
            mainLight.position = camera.Position;
            mainLight.direction = camera.Front;
        }



        // Малюємо всі куби з освітленням
        for(int i = 0; i < std::size(cubePositions); i++){
            if(cubeMaterials[i % std::size(cubeMaterials)].alpha >= 0.99f) {
                cubes[i].showTex = showTextures;
                cubes[i].draw(view, projection, mainLight, camera.Position);
            }
        }

        for(int i = 0; i < std::size(cubePositions); i++){
            if(cubeMaterials[i % std::size(cubeMaterials)].alpha < 0.99f) {
                cubes[i].showTex = showTextures;
                cubes[i].draw(view, projection, mainLight, camera.Position);
            }
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