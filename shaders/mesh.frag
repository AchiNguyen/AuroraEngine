#version 460 core

in vec3 v_world_position;
in vec3 v_world_normal;
in vec2 v_uv;

out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

uniform Material u_material;
uniform DirectionalLight u_dir_light;
uniform PointLight u_point_light;
uniform vec3 u_view_position;

vec3 phong_directional(DirectionalLight light, vec3 normal, vec3 view_dir,
                       vec3 albedo, vec3 specular_color) {
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    vec3 ambient  = light.color * albedo * 0.1;
    vec3 diffuse  = light.color * diff * albedo;
    vec3 specular = light.color * spec * specular_color;
    return (ambient + diffuse + specular) * light.intensity;
}

vec3 phong_point(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir,
                 vec3 albedo, vec3 specular_color) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 ambient  = light.color * albedo * 0.1;
    vec3 diffuse  = light.color * diff * albedo;
    vec3 specular = light.color * spec * specular_color;
    return (ambient + diffuse + specular) * light.intensity * attenuation;
}

void main() {
    vec3 albedo         = texture(u_material.diffuse,  v_uv).rgb;
    vec3 specular_color = texture(u_material.specular, v_uv).rgb;
    vec3 normal   = normalize(v_world_normal);
    vec3 view_dir = normalize(u_view_position - v_world_position);

    vec3 result = phong_directional(u_dir_light,   normal,                    view_dir, albedo, specular_color)
                + phong_point(      u_point_light, normal, v_world_position, view_dir, albedo, specular_color);

    frag_color = vec4(result, 1.0);
}
