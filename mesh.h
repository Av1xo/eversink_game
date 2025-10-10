#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "material.h"
#include "light.h"

class Mesh {
    public:
        virtual void draw(const glm::mat4& view, const glm::mat4& projection, const Light& light, const glm::vec3& viewPos) = 0;
        virtual ~Mesh() = default;
};

struct CubeVertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Cube : public Mesh
{
    public:
        unsigned int VAO, VBO, EBO;
        Shader& shader;
        std::vector<Texture*> textures;
        glm::vec3 position;
        glm::vec3 size;
        bool showTex;
        Material material;
        
        Cube(
            const glm::vec3& pos,
            const glm::vec3& cubeSize, 
            const glm::vec3& color, 
            Shader& shaderRef, 
            const std::vector<Texture*>& texs,
            const Material& mat = Materials::Silver, 
            bool showTex = true)
        : position(pos), shader(shaderRef), size(cubeSize), material(mat),showTex(showTex)
        {
            std::cout << "CUBE::START_INIT" << std::endl;
            for (auto tex : texs)
            {
                textures.push_back(tex);
            }
            auto vertices = generateCubeVertices(position, size, color);
            auto indicies = generateCubeIndices();

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CubeVertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);

            // POSITION
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, position));
            glEnableVertexAttribArray(0);

            // COLOR
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, color));
            glEnableVertexAttribArray(1);

            // NORMAL
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, normal));
            glEnableVertexAttribArray(2);

            // TEXTURE
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, texCoord));
            glEnableVertexAttribArray(3);

            glBindVertexArray(0);
            std::cout << "CUBE::END_INIT" << std::endl;
        };
        
        void draw(const glm::mat4& view, const glm::mat4& projection, const Light& light, const glm::vec3& viewPos) override
        {
            shader.use();

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

            shader.setVec3("viewPos", viewPos);    // Додано позицію камери
            shader.setMat4("model", model);
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            light.setShaderUniforms(shader);
            material.setShaderUniforms(shader);

            // Перевіряємо чи є текстури
            bool hasTextures = !textures.empty();
            shader.setBool("useTextures", hasTextures && showTex);

            if (hasTextures)
            {
                for (unsigned int i = 0; i < textures.size(); i++)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    textures[i]->bind();
                    shader.setInt("texture" + std::to_string(i), i);
                }
            }

            if (showTex && hasTextures)
            {
                shader.setFloat("colorAlpha", 0.0f);
            }
            else
            {
                shader.setFloat("colorAlpha", 1.0f);
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        ~Cube() override
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        };

    private:
        std::vector<CubeVertex> generateCubeVertices(glm::vec3 position, glm::vec3 size, glm::vec3 color){
            std::cout << "CUBE::START_VERTEX" << std::endl;
            float halfx = size.x / 2.0f;
            float halfy = size.y / 2.0f;
            float halfz = size.z / 2.0f;

            glm::vec3 p[8] = {
                {position.x - halfx, position.y - halfy, position.z + halfz},
                {position.x + halfx, position.y - halfy, position.z + halfz},
                {position.x + halfx, position.y + halfy, position.z + halfz},
                {position.x - halfx, position.y + halfy, position.z + halfz},
                {position.x - halfx, position.y - halfy, position.z - halfz},
                {position.x + halfx, position.y - halfy, position.z - halfz},
                {position.x + halfx, position.y + halfy, position.z - halfz},
                {position.x - halfx, position.y + halfy, position.z - halfz},
            };

            glm::vec3 normals[] = {
                {0.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, -1.0f},
                {-1.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, -1.0f, 0.0f},
            };

            glm::vec2 tex[] {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f},
            };

            std::vector<CubeVertex> vertices;
            // front
            vertices.push_back({p[0], color, normals[0], tex[0]});
            vertices.push_back({p[1], color, normals[0], tex[1]});
            vertices.push_back({p[2], color, normals[0], tex[2]});
            vertices.push_back({p[3], color, normals[0], tex[3]});

            // back
            vertices.push_back({p[5], color, normals[1], tex[0]});
            vertices.push_back({p[4], color, normals[1], tex[1]});
            vertices.push_back({p[7], color, normals[1], tex[2]});
            vertices.push_back({p[6], color, normals[1], tex[3]});

            // left
            vertices.push_back({p[4], color, normals[2], tex[0]});
            vertices.push_back({p[0], color, normals[2], tex[1]});
            vertices.push_back({p[3], color, normals[2], tex[2]});
            vertices.push_back({p[7], color, normals[2], tex[3]});

            // right
            vertices.push_back({p[1], color, normals[3], tex[0]});
            vertices.push_back({p[5], color, normals[3], tex[1]});
            vertices.push_back({p[6], color, normals[3], tex[2]});
            vertices.push_back({p[2], color, normals[3], tex[3]});

            // top
            vertices.push_back({p[3], color, normals[4], tex[0]});
            vertices.push_back({p[2], color, normals[4], tex[1]});
            vertices.push_back({p[6], color, normals[4], tex[2]});
            vertices.push_back({p[7], color, normals[4], tex[3]});

            // bottom
            vertices.push_back({p[4], color, normals[5], tex[0]});
            vertices.push_back({p[5], color, normals[5], tex[1]});
            vertices.push_back({p[1], color, normals[5], tex[2]});
            vertices.push_back({p[0], color, normals[5], tex[3]});

            std::cout << "CUBE::END_VERTEX" << std::endl;

            return vertices;
        }

        std::vector<unsigned int> generateCubeIndices() {
            std::cout << "CUBE::START_INDICES" << std::endl;
            std::vector<unsigned int> indices;
            for (int i = 0; i < 6; ++i) {
                int offset = i * 4;
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 2);
                indices.push_back(offset + 2);
                indices.push_back(offset + 3);
                indices.push_back(offset + 0);
            }
            std::cout << "CUBE::END_INDICES" << std::endl;
            return indices;
        }
};

#endif