#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include "shader.h"

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    void setShaderUniforms(Shader& shader) const {
        shader.setVec3("material.ambient", ambient);
        shader.setVec3("material.diffuse", diffuse);
        shader.setVec3("material.specular", specular);
        shader.setFloat("material.shininess", shininess);
    }
};

namespace Materials {
    const Material Emerald = {
        glm::vec3(0.0215f, 0.1745f, 0.0215f),
        glm::vec3(0.07568f, 0.61424f, 0.07568f),
        glm::vec3(0.633f, 0.727811f, 0.633f),
        0.6f * 128.0f
    };

    const Material Jade = {
        glm::vec3(0.135f, 0.2225f, 0.1575f),
        glm::vec3(0.54f, 0.89f, 0.63f),
        glm::vec3(0.316228f, 0.316228f, 0.316228f),
        0.1f * 128.0f
    };

    const Material Gold = {
        glm::vec3(0.24725f, 0.1995f, 0.0745f),
        glm::vec3(0.75164f, 0.60648f, 0.22648f),
        glm::vec3(0.628281f, 0.555802f, 0.366065f),
        0.4f * 128.0f
    };

    const Material Silver = {
        glm::vec3(0.19225f, 0.19225f, 0.19225f),
        glm::vec3(0.50754f, 0.50754f, 0.50754f),
        glm::vec3(0.508273f, 0.508273f, 0.508273f),
        0.4f * 128.0f
    };

    const Material Copper = {
        glm::vec3(0.19125f, 0.0735f, 0.0225f),
        glm::vec3(0.7038f, 0.27048f, 0.0828f),
        glm::vec3(0.256777f, 0.137622f, 0.086014f),
        0.1f * 128.0f
    };

    const Material BlackPlastic = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.01f, 0.01f, 0.01f),
        glm::vec3(0.50f, 0.50f, 0.50f),
        0.25f * 128.0f
    };

    const Material RedPlastic = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(0.7f, 0.6f, 0.6f),
        0.25f * 128.0f
    };

    const Material WhiteRubber = {
        glm::vec3(0.05f, 0.05f, 0.05f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        0.078125f * 128.0f
    };

    const Material Bronze = {
        glm::vec3(0.2125f, 0.1275f, 0.054f),
        glm::vec3(0.714f, 0.4284f, 0.18144f),
        glm::vec3(0.393548f, 0.271906f, 0.166721f),
        0.2f * 128.0f
    };

    const Material Pearl = {
        glm::vec3(0.25f, 0.20725f, 0.20725f),
        glm::vec3(1.0f, 0.829f, 0.829f),
        glm::vec3(0.296648f, 0.296648f, 0.296648f),
        0.088f * 128.0f
    };
}

#endif