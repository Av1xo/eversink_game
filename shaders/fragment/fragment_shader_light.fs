#version 450 core

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    float alpha;
};

struct Light {
    int type;  // 0 = directional, 1 = point, 2 = spot
    
    vec3 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
    
    float cutOff;
    float outerCutOff;
};

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 viewPos;
uniform bool useTextures;
uniform Material material;
uniform Light light;

const float PI = 3.14159265359;

// NORMAL DISTRIBUTION FUNCTION (GGX/Trowbridge-Reitz)
float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

// GEOMETRY FUNCTION (Smith's method with Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / max(denom, 0.0000001);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

// FRESNEL EQUATION (Schlick's approximation)
vec3 FresnelSchlick(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}

void main()
{
    // Нормалізуємо вектори
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    vec3 L;
    float attenuation = 1.0;
    
    if (light.type == 0) {
        // DIRECTIONAL LIGHT (напрямлене - як сонце)
        L = normalize(-light.direction);  // Інвертуємо, бо це напрямок ДО світла
        attenuation = 1.0;  // Без затухання
    }
    else if (light.type == 1) {
        L = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + 
                           light.quadratic * (distance * distance));
    }
    else if (light.type == 2) {
        L = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + 
                           light.quadratic * (distance * distance));
        
        float theta = dot(L, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        attenuation *= intensity;
    }
    
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    vec3 radiance = light.diffuse * attenuation;

    // F0 для Fresnel
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metallic);

    // Fresnel ефект для прозорості
    float fresnelFactor = pow(1.0 - NdotV, 3.0);
    float finalAlpha = mix(material.alpha, 1.0, fresnelFactor * (1.0 - material.metallic));

    // Cook-Torrance BRDF
    float D = DistributionGGX(NdotH, material.roughness);
    float G = GeometrySmith(NdotV, NdotL, material.roughness);
    vec3 F = FresnelSchlick(HdotV, F0);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular = numerator / max(denominator, 0.001);

    // Енергозбереження
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;

    // Lambertian diffuse
    vec3 diffuse = kD * material.albedo / PI;

    // Підсумкове освітлення
    vec3 Lo = (diffuse + specular) * radiance * NdotL;

    // Ambient lighting
    vec3 ambient = light.ambient * material.albedo * material.ao;

    vec3 finalColor = ambient + Lo;

    // Застосування текстур
    vec4 resultColor;
    if (useTextures) {
        vec4 texColor = mix(texture(texture0, TexCoord), 
                           texture(texture1, TexCoord), 0.2);
        resultColor = texColor * vec4(finalColor, finalAlpha);
    } else {
        resultColor = vec4(finalColor, finalAlpha);
    }
    
    FragColor = resultColor;
}