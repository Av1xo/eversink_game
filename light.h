#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "shader.h"

enum class LightType {
    DIRECTIONAL = 0,  // Напрямлене (сонце)
    POINT = 1,        // Точкове (лампочка)
    SPOT = 2          // Прожектор (ліхтарик)
};

struct Light{
    LightType type;
    
    // Для точкового та прожектора
    glm::vec3 position;
    
    // Для напрямленого та прожектора
    glm::vec3 direction;
    
    // Кольори
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    // Атенуація (для точкового та прожектора)
    float constant;
    float linear;
    float quadratic;
    
    // Для прожектора
    float cutOff;        // Внутрішній кут (в радіанах)
    float outerCutOff;   // Зовнішній кут (в радіанах)

    void setShaderUniforms(Shader& shader) const {
        shader.setInt("light.type", static_cast<int>(type));
        shader.setVec3("light.position", position);
        shader.setVec3("light.direction", direction);
        
        shader.setVec3("light.ambient", ambient);
        shader.setVec3("light.diffuse", diffuse);
        shader.setVec3("light.specular", specular);
        
        shader.setFloat("light.constant", constant);
        shader.setFloat("light.linear", linear);
        shader.setFloat("light.quadratic", quadratic);
        
        shader.setFloat("light.cutOff", cutOff);
        shader.setFloat("light.outerCutOff", outerCutOff);
    }
};

namespace Lights {
    // Сонце (напрямлене світло)
    inline Light CreateDirectional(
        const glm::vec3& direction = glm::vec3(-0.2f, -1.0f, -0.3f),
        const glm::vec3& ambient = glm::vec3(0.2f),
        const glm::vec3& diffuse = glm::vec3(0.8f),
        const glm::vec3& specular = glm::vec3(1.0f)
    ) {
        Light light;
        light.type = LightType::DIRECTIONAL;
        light.direction = glm::normalize(direction);
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;
        // Атенуація не використовується
        light.constant = 1.0f;
        light.linear = 0.0f;
        light.quadratic = 0.0f;
        return light;
    }

    // Лампочка (точкове світло)
    inline Light CreatePoint(
        const glm::vec3& position,
        const glm::vec3& ambient = glm::vec3(0.2f),
        const glm::vec3& diffuse = glm::vec3(0.8f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float constant = 1.0f,
        float linear = 0.09f,
        float quadratic = 0.032f
    ) {
        Light light;
        light.type = LightType::POINT;
        light.position = position;
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;
        light.constant = constant;
        light.linear = linear;
        light.quadratic = quadratic;
        return light;
    }

    // Ліхтарик (прожектор)
    inline Light CreateSpot(
        const glm::vec3& position,
        const glm::vec3& direction,
        const glm::vec3& ambient = glm::vec3(0.1f),
        const glm::vec3& diffuse = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float cutOff = glm::radians(12.5f),
        float outerCutOff = glm::radians(17.5f),
        float constant = 1.0f,
        float linear = 0.09f,
        float quadratic = 0.032f
    ) {
        Light light;
        light.type = LightType::SPOT;
        light.position = position;
        light.direction = glm::normalize(direction);
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;
        light.cutOff = cutOff;
        light.outerCutOff = outerCutOff;
        light.constant = constant;
        light.linear = linear;
        light.quadratic = quadratic;
        return light;
    }
}

#endif