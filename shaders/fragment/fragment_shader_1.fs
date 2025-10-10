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
uniform Light lights[32];
uniform int numLights;
uniform vec3 viewPos;
uniform bool useTextures;
uniform Material material;

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

vec3 calculateLight(Light light, vec3 N, vec3 V, vec3 F0, float viewNdotV)
{
    // === 1. Обчислення L (напрямку до світла) та Attenuation ===
    vec3 L;
    float attenuation = 1.0;

    if (light.type == 0) { // DIRECTIONAL LIGHT
        L = normalize(-light.direction);
        attenuation = 1.0;
    }
    else { // POINT (1) або SPOT (2)
        float distanceLight = length(light.position - FragPos);
        L = normalize(light.position - FragPos);
        
        // Attenuation (затухання)
        attenuation = 1.0 / (light.constant + light.linear * distanceLight + 
                             light.quadratic * (distanceLight * distanceLight));

        if (light.type == 2) { // SPOT LIGHT (додатковий множник)
            float theta = dot(L, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }
    }
    
    // Якщо світло вимкнене або поза межами, виходимо
    if (attenuation < 0.001) return vec3(0.0);

    // Radiance - інтенсивність світла з урахуванням атенуації
    vec3 radiance = light.diffuse * attenuation;
    
    // === 2. Cook-Torrance BRDF та Обчислення Lo ===
    
    vec3 H = normalize(V + L); // Halfway vector
    
    // Dot products
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    // D, G, F
    float D = DistributionGGX(NdotH, material.roughness);
    float G = GeometrySmith(viewNdotV, NdotL, material.roughness);
    vec3 F = FresnelSchlick(HdotV, F0);

    // Specular
    vec3 numerator = D * G * F;
    float denominator = 4.0 * viewNdotV * NdotL;
    vec3 specular = numerator / max(denominator, 0.001);

    // Енергозбереження та Diffuse
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic; 

    // Lambertian diffuse
    vec3 diffuse = kD * material.albedo / PI;

    // Підсумкове освітлення від цього джерела (Lo)
    vec3 Lo = (diffuse + specular) * radiance * NdotL;
    
    return Lo;
}


void main()
{
    // Нормалізуємо вектори
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    float NdotV = max(dot(N, V), 0.0);
    
    // F0 для Fresnel
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metallic);

    // 1. Ініціалізація сумматорів
    vec3 totalAmbient = vec3(0.0); 
    vec3 finalLo = vec3(0.0);
    
    // 2. Цикл по ВСІХ джерелах світла
    for(int i = 0; i < numLights; ++i)
    {
        // Ambient беремо від кожного джерела, але з меншою вагою
        totalAmbient += lights[i].ambient * material.albedo * material.ao * 0.3;
        
        // Додаємо внесок кожного світла
        finalLo += calculateLight(lights[i], N, V, F0, NdotV);
    }
    
    // 3. Фінальний колір
    vec3 finalColor = totalAmbient + finalLo;

    // Fresnel ефект для прозорості
    float fresnelFactor = pow(1.0 - NdotV, 5.0);
    float finalAlpha = mix(material.alpha, 1.0, fresnelFactor * (1.0 - material.metallic) * 0.7);

    // Tone mapping (ACES)
    vec3 x = finalColor * 0.6;
    finalColor = (x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14);
    
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

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