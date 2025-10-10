#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec3 ourColor;
out vec3 FragPos;
out vec2 TexCoord;
out vec3 lightColor;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Передаємо колір, текстурні координати і колір світла
    ourColor = aColor;
    TexCoord = aTexCoord;
    lightColor = vec3(1.0);
    
    // Правильна трансформація нормалей (враховує неоднорідне масштабування)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}