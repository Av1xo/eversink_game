#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW  = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
    public:
        // атрибути камери
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUP;

        // кути Ейлера
        float Yaw;
        float Pitch;

        // Налаштування камери
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        // Векторний конструктор
        Camera(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = YAW,
            float pitch = PITCH
        ) 
        : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Fov(FOV)
        {
            // =========== VECTORS ================
            Position = position;
            WorldUP = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
            std::cout << "CAMERA::CREATED" << std::endl;
        }

        Camera(
            float posX, float posY, float posZ,
            float upX, float upY, float upZ,
            float yaw, float pitch
        )
        :
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Fov(FOV)
        {
            // ========= SCALARS ==============
            Position = glm::vec3(posX, posY, posZ);
            WorldUP = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
            std::cout << "CAMERA::CREATED" << std::endl;
        }

        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(
                    Position, 
                    Position + Front,
                    Up
                );
        }

        void ProcessKeyboard(Camera_Movement direction, float deltatime)
        {
            float velocity = MovementSpeed * deltatime;
            switch (direction)
            {
            case FORWARD:
                Position += Front * velocity;
                break;
            case BACKWARD:
                Position -= Front * velocity;
                break;
            case LEFT:
                Position -= Right * velocity;
                break;
            case RIGHT:
                Position += Right * velocity;
                break;
            default:
                break;
            }
        }

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw  += xoffset;
            Pitch += yoffset;

            if (constrainPitch)
            {
                if(Pitch > 89.0f)
                    Pitch = 89.0f;
                if(Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            updateCameraVectors();
        }

        void ProcessMouseScroll(float yoffset)
        {
            if (Fov >= 1.0f && Fov <= 45.0f)
                Fov -= yoffset;
            if (Fov <= 1.0f)
                Fov = 1.0f;
            if (Fov >= 45.0f)
                Fov = 45.0f;
        }

    private:
        void updateCameraVectors()
        {
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);

            Right = glm::normalize(glm::cross(Front, WorldUP));
            Up = glm::normalize(glm::cross(Right, Front));
        }
};

#endif