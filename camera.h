#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
    public:
        float cameraSpeed = 0.05f;
        float sensitivity = 0.1f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 45.0f;
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        void moveForward() 
        {
            cameraPos += cameraSpeed * cameraFront;
        }

        void moveBackward() 
        {
            cameraPos -= cameraSpeed * cameraFront;
        }

        void moveLeft() 
        {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        void moveRight() 
        {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        void setDirection() 
        {
            cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraDirection.y = sin(glm::radians(pitch));
            cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(cameraDirection);
        }

        void setVertAngle(float angleUp = 80.0f, float angleDown = -80.0f)
        {
            // Set vertical angle default UP=80 DOWN=-80
            if(pitch > angleUp)
                pitch = angleUp;
            if(pitch < angleDown)
                pitch = angleDown;
        }

        void setFOV(double xoffset, double yoffset)
        {
            if(fov >= 1.0f && fov <= 45.0f)
            {
                fov -= yoffset;
            }
            else if(fov < 1.0f)
            {
                fov = 1.0f;
            }
            else if(fov > 45.0f)
            {
                fov = 45.0f;
            }
        }
};

#endif