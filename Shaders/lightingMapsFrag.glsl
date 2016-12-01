#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main ()
{
    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    // Diffuse
    vec3 norm = normalize (Normal);
    vec3 lightDir = normalize (light.position - FragPos);
    float diff = max (dot (norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3 (texture (material.diffuse, TexCoords));
    
    // Specular
    vec3 viewDir = normalize (viewPos - FragPos);
    vec3 reflectDir = reflect (-lightDir, norm);
    float spec = pow (max (dot (viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (vec3(1.0f) - vec3(texture(material.specular, TexCoords)));
    
    // Emission
    vec3 emission = vec3(texture(material.emission, TexCoords));
    
    vec3 result = ambient + diffuse + specular + emission;
    color = vec4(result, 1.0f);
}