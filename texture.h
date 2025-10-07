#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

class Texture {
    public:
        unsigned int ID;

        Texture(const char* imagePath, GLint wrap = GL_REPEAT, GLint filter = GL_LINEAR)
        {
            stbi_set_flip_vertically_on_load(true);

            glGenTextures(1, &ID);
            glBindTexture(GL_TEXTURE_2D, ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

            if (data) {
                GLenum format = (nrChannels == 1) ? GL_RED :
                                (nrChannels == 3) ? GL_RGB :
                                (nrChannels == 4) ? GL_RGBA : GL_RGB;

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                std::cout << "Loaded texture: " << imagePath 
                        << " (" << width << "x" << height << ", " 
                        << nrChannels << " channels)" << std::endl;
            } else {
                std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD " << imagePath << std::endl;
            }

            stbi_image_free(data);
            unbind();
        }

        void bind() const {
            glBindTexture(GL_TEXTURE_2D, ID);
        }

        void unbind() const {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
};

#endif