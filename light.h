#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "shader.h"

struct Light{
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    void setShaderUniforms(Shader& shader) const {
        shader.setVec3("light.position", position);

        shader.setVec3("light.ambient", ambient);
        shader.setVec3("light.diffuse", diffuse);
        shader.setVec3("light.specular", specular);

        shader.setFloat("light.constant", constant);
        shader.setFloat("light.linear", linear);
        shader.setFloat("light.quadratic", quadratic);
    }
};

namespace Lights{

}

#endif