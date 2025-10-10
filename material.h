#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include "shader.h"

struct Material
{
    glm::vec3 albedo;      // Базовий колір
    float metallic;        // Металічність (0.0 = діелектрик, 1.0 = метал)
    float roughness;       // Шорсткість (0.0 = дзеркало, 1.0 = матовий)
    float ao;              // Ambient Occlusion
    float alpha;           // Прозорість (0.0 = прозорий, 1.0 = непрозорий)

    void setShaderUniforms(Shader& shader) const {
        shader.setVec3("material.albedo", albedo);
        shader.setFloat("material.metallic", metallic);
        shader.setFloat("material.roughness", roughness);
        shader.setFloat("material.ao", ao);
        shader.setFloat("material.alpha", alpha);
    }
};

namespace Materials {
    // МЕТАЛИ (metallic = 1.0, різна roughness)
    const Material Gold = {
        glm::vec3(1.0f, 0.765557f, 0.336057f),
        1.0f, 0.3f, 1.0f, 1.0f
    };

    const Material Silver = {
        glm::vec3(0.972f, 0.960f, 0.915f),
        1.0f, 0.2f, 1.0f, 1.0f
    };

    const Material Copper = {
        glm::vec3(0.955f, 0.637f, 0.538f),
        1.0f, 0.4f, 1.0f, 1.0f
    };

    const Material Bronze = {
        glm::vec3(0.804f, 0.498f, 0.196f),
        1.0f, 0.5f, 1.0f, 1.0f
    };

    // ДІЕЛЕКТРИКИ
    const Material Emerald = {
        glm::vec3(0.07568f, 0.61424f, 0.07568f),
        0.0f, 0.3f, 1.0f, 0.7f  // Трохи прозорий
    };

    const Material Jade = {
        glm::vec3(0.54f, 0.89f, 0.63f),
        0.0f, 0.6f, 1.0f, 0.85f  // Трохи прозорий
    };

    const Material Pearl = {
        glm::vec3(1.0f, 0.829f, 0.829f),
        0.0f, 0.5f, 1.0f, 0.9f  // Легка прозорість
    };

    // ПЛАСТИК
    const Material RedPlastic = {
        glm::vec3(0.8f, 0.1f, 0.1f),
        0.0f, 0.4f, 1.0f, 1.0f
    };

    const Material BlackPlastic = {
        glm::vec3(0.02f, 0.02f, 0.02f),
        0.0f, 0.5f, 1.0f, 1.0f
    };

    // ГУМА
    const Material WhiteRubber = {
        glm::vec3(0.8f, 0.8f, 0.8f),
        0.0f, 0.9f, 1.0f, 1.0f
    };

    // ПРОЗОРІ МАТЕРІАЛИ
    const Material Glass = {
        glm::vec3(0.95f, 0.95f, 0.95f),
        0.0f, 0.05f, 1.0f, 0.3f  // Дуже прозоре
    };

    const Material ColoredGlass = {
        glm::vec3(0.4f, 0.7f, 0.9f),  // Блакитне скло
        0.0f, 0.1f, 1.0f, 0.4f
    };

    const Material FrostedGlass = {
        glm::vec3(0.9f, 0.9f, 0.9f),
        0.0f, 0.6f, 1.0f, 0.5f  // Матове скло
    };

    const Material Ice = {
        glm::vec3(0.85f, 0.95f, 1.0f),  // Холодний відтінок
        0.0f, 0.15f, 1.0f, 0.6f
    };

    const Material Water = {
        glm::vec3(0.1f, 0.3f, 0.5f),
        0.0f, 0.1f, 1.0f, 0.5f
    };

    // ДОДАТКОВІ МЕТАЛИ
    const Material Chrome = {
        glm::vec3(0.55f, 0.55f, 0.55f),
        1.0f, 0.05f, 1.0f, 1.0f
    };

    const Material RoughIron = {
        glm::vec3(0.56f, 0.57f, 0.58f),
        1.0f, 0.8f, 1.0f, 1.0f
    };

    // НАПІВПРОЗОРІ
    const Material Amber = {
        glm::vec3(1.0f, 0.75f, 0.0f),
        0.0f, 0.3f, 1.0f, 0.65f
    };

    const Material Crystal = {
        glm::vec3(0.95f, 0.95f, 1.0f),
        0.0f, 0.05f, 1.0f, 0.4f
    };
}

#endif