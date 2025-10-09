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
    // Нормалізація нормалі
    vec3 norm = normalize(Normal);
    
    // Ambient освітлення
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse освітлення
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular освітлення (Phong)
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);  // 32 - shininess
    vec3 specular = specularStrength * spec * lightColor;
    
    // Основний колір (текстура або solid color)
    vec4 mainColor;
    if (useTextures) {
        mainColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2) * vec4(ourColor, colorAlpha);
    } else {
        mainColor = vec4(ourColor, 1.0);
    }
    
    // Фінальний результат за моделлю Фонга
    vec3 result = (ambient + diffuse + specular) * mainColor.rgb;
    
    FragColor = vec4(result, colorAlpha);
}