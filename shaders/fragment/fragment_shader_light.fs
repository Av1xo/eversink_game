#version 450 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 lightColor;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float colorAlpha;
uniform vec3 lightPos;
uniform vec3 viewPos;  // позиція камери для specular
uniform bool useTextures;

void main()
{
    FragColor = vec4(1.0);
}