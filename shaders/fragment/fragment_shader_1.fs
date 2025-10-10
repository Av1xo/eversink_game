#version 450 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 lightColor;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float colorAlpha;
uniform vec3 viewPos; 
uniform bool useTextures;
uniform Material material;
uniform Light light;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Attenuation
    float distanceLight = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceLight + light.quadratic * (distanceLight * distanceLight));
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // Specular 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec4 mainColor;

    ambient = ambient * attenuation;
    diffuse = diffuse * attenuation;
    specular = specular * attenuation;

    // Phong
    vec3 result = ambient + diffuse + specular;

    if (useTextures) {
        mainColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2) * vec4(result, 1.0);
    } else {
        mainColor = vec4(result, 1.0);
    }
    
    FragColor = mainColor;
}