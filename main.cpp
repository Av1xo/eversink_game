#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
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

bool spotlightEnabled = true;
bool fKeyPressed = false; 

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

    // SHADER PROGRAM
    Shader ShadersProgram1(vertexShaderSource1, fragShaderSource1);
    // CUBE POSITIONS
    glm::vec3 cubePositions[] = {
        // РЯД 1: Метали (Y = 0.0f)
        glm::vec3(-5.0f, 0.0f, 0.0f),  // Gold
        glm::vec3(-2.5f, 0.0f, 0.0f),  // Silver
        glm::vec3( 0.0f, 0.0f, 0.0f),  // Bronze
        glm::vec3( 2.5f, 0.0f, 0.0f),  // Chrome
        glm::vec3( 5.0f, 0.0f, 0.0f),  // Copper
        
        // РЯД 2: Опакові діелектрики (Y = 2.5f)
        glm::vec3(-5.0f, 2.5f, 0.0f),  // BlackPlastic
        glm::vec3(-2.5f, 2.5f, 0.0f),  // RedPlastic
        glm::vec3( 0.0f, 2.5f, 0.0f),  // WhiteRubber
        glm::vec3( 2.5f, 2.5f, 0.0f),  // RoughIron
        glm::vec3( 5.0f, 2.5f, 0.0f),  // Pearl

        // РЯД 3: Прозорі (Y = 5.0f)
        glm::vec3(-5.0f, 5.0f, 0.0f),  // Glass
        glm::vec3(-2.5f, 5.0f, 0.0f),  // ColoredGlass
        glm::vec3( 0.0f, 5.0f, 0.0f),  // Crystal
        glm::vec3( 2.5f, 5.0f, 0.0f),  // Ice
        glm::vec3( 5.0f, 5.0f, 0.0f),  // Water

        // РЯД 4: Спеціальні (Y = 7.5f)
        glm::vec3(-5.0f, 7.5f, 0.0f),  // Amber
        glm::vec3(-2.5f, 7.5f, 0.0f),  // Emerald
        glm::vec3( 0.0f, 7.5f, 0.0f),  // Jade
        glm::vec3( 2.5f, 7.5f, 0.0f),  // FrostedGlass
        glm::vec3( 5.0f, 7.5f, 0.0f)   // WhiteRubber
    };

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // ========================== LIGHTS ===========================
    glm::vec3 lightPosition = glm::vec3(0.0f, 7.0f, 5.0f);

    std::vector<Light> sceneLights = {
        // 1. DIRECTIONAL (Слабке загальне освітлення)
        Lights::CreateDirectional(
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.015f),              // Дуже слабкий ambient
            glm::vec3(0.15f, 0.15f, 0.18f), // Ледь помітне денне світло
            glm::vec3(0.3f)
        ),
        // 2. POINT #1 (Тепле оранжеве світло зліва)
        Lights::CreatePoint(
            glm::vec3(-6.0f, 3.0f, 3.0f),
            glm::vec3(0.02f), 
            glm::vec3(2.0f, 0.8f, 0.3f),   // Оранжеве
            glm::vec3(1.0f),
            1.0f,
            0.045f,
            0.0075f
        ),
        // 3. POINT #2 (Холодне синє світло справа)
        Lights::CreatePoint(
            glm::vec3(6.0f, 3.0f, 3.0f),
            glm::vec3(0.02f), 
            glm::vec3(0.3f, 0.8f, 2.0f),   // Синє
            glm::vec3(1.0f),
            1.0f,
            0.045f,
            0.0075f
        ),
        // 4. POINT #3 (Зелене світло ззаду зверху)
        Lights::CreatePoint(
            glm::vec3(0.0f, 6.0f, -3.0f),
            glm::vec3(0.02f), 
            glm::vec3(0.4f, 2.0f, 0.6f),   // Зелене
            glm::vec3(1.0f),
            1.0f,
            0.045f,
            0.0075f
        ),
        // 5. SPOT (Ліхтарик камери)
        Lights::CreateSpot(
            camera.Position,
            camera.Front,
            glm::vec3(0.0f),
            glm::vec3(4.0f, 4.0f, 5.0f),   // Яскраве біло-синє
            glm::vec3(3.0f),
            glm::radians(20.0f),
            glm::radians(30.0f),
            1.0f,
            0.027f,
            0.0028f
        )
    };

    std::cout << "=== Джерела світла ===" << std::endl;
    std::cout << "Кількість джерел: " << sceneLights.size() << std::endl;
    for (size_t i = 0; i < sceneLights.size(); ++i) {
        std::cout << "Світло " << i << ": тип = " << static_cast<int>(sceneLights[i].type) << std::endl;
    }
    
    std::cout << "\n=== Керування ===" << std::endl;
    std::cout << "WASD - рух камери" << std::endl;
    std::cout << "Миша - огляд" << std::endl;
    std::cout << "Колесо миші - зум" << std::endl;
    std::cout << "T - увімкнути/вимкнути текстури" << std::endl;
    std::cout << "F - увімкнути/вимкнути ліхтарик (spotlight)" << std::endl;
    std::cout << "ESC - вихід\n" << std::endl;;

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
        Materials::Gold,
        Materials::Silver,
        Materials::Bronze,
        Materials::Chrome,
        Materials::Copper,
        Materials::BlackPlastic,
        Materials::RedPlastic,
        Materials::WhiteRubber,
        Materials::RoughIron,
        Materials::Pearl,
        Materials::Glass,
        Materials::ColoredGlass,
        Materials::Crystal,
        Materials::Ice,
        Materials::Water,
        Materials::Amber,
        Materials::Emerald,
        Materials::Jade,
        Materials::FrostedGlass,
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


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_roll_callback);

    // Цикл рендерингу
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Оновлюємо Spotlight (індекс 4 - останній в масиві)
        int spotlightIndex = 4;
        sceneLights[spotlightIndex].position = camera.Position;
        sceneLights[spotlightIndex].direction = camera.Front;
        
        // Вмикаємо/вимикаємо spotlight через яскравість
        if (spotlightEnabled) {
            sceneLights[spotlightIndex].diffuse = glm::vec3(4.0f, 4.0f, 5.0f);
            sceneLights[spotlightIndex].specular = glm::vec3(3.0f);
        } else {
            sceneLights[spotlightIndex].diffuse = glm::vec3(0.0f);
            sceneLights[spotlightIndex].specular = glm::vec3(0.0f);
        }
        
        // Малюємо непрозорі куби
        for(size_t i = 0; i < cubes.size(); i++){
            if(cubeMaterials[i % std::size(cubeMaterials)].alpha >= 0.99f) {
                cubes[i].showTex = showTextures;
                cubes[i].draw(view, projection, sceneLights, camera.Position, camera);
            }
        }

        // Малюємо прозорі куби
        for(size_t i = 0; i < cubes.size(); i++){
            if(cubeMaterials[i % std::size(cubeMaterials)].alpha < 0.99f) {
                cubes[i].showTex = showTextures;
                cubes[i].draw(view, projection, sceneLights, camera.Position, camera);
            }
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();  
    }

    cubes.clear();
    cubeTextures.clear();

    glfwTerminate();
    return 0;
}

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
        std::cout << "Текстури: " << (showTextures ? "ВКЛ" : "ВИКЛ") << std::endl;
    }
    
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
    {
        tKeyPressed = false;
    }

    // Toggle spotlight (клавіша F)
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed)
    {
        spotlightEnabled = !spotlightEnabled;
        fKeyPressed = true;
        std::cout << "Spotlight: " << (spotlightEnabled ? "ВКЛ" : "ВИКЛ") << std::endl;
    }
    
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        fKeyPressed = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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