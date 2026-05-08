#version 460 core

#define MAX_POINT_LIGHTS 8

in vec3 v_world_position;
in vec3 v_world_normal;
in vec3 v_world_tangent;
in vec3 v_world_bitangent;
in vec2 v_uv;
in vec4 v_light_space_position;

out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
    int   has_normal_map;
};

struct DirectionalLight {
    vec3  direction;
    vec3  color;
    float intensity;
    int   enabled;
};

struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

uniform Material         u_material;
uniform DirectionalLight u_dir_light;
uniform PointLight       u_point_lights[MAX_POINT_LIGHTS];
uniform int              u_num_point_lights;
uniform vec3             u_view_position;
uniform float            u_ambient_factor;
uniform float            u_normal_intensity;

uniform sampler2D u_shadow_map;
uniform int       u_shadows_enabled;
uniform float     u_shadow_bias_min;
uniform float     u_shadow_bias_max;

vec3 compute_normal() {
    if (u_material.has_normal_map == 0) {
        return normalize(v_world_normal);
    }
    vec3 tangent_normal = texture(u_material.normal, v_uv).rgb * 2.0 - 1.0;
    tangent_normal.xy *= u_normal_intensity;
    mat3 TBN = mat3(normalize(v_world_tangent),
                    normalize(v_world_bitangent),
                    normalize(v_world_normal));
    return normalize(TBN * tangent_normal);
}

float compute_shadow(vec3 normal, vec3 light_dir) {
    if (u_shadows_enabled == 0) return 0.0;

    vec3 proj = v_light_space_position.xyz / v_light_space_position.w;
    proj      = proj * 0.5 + 0.5;
    if (proj.z > 1.0) return 0.0;

    float bias = max(u_shadow_bias_max * (1.0 - dot(normal, light_dir)),
                     u_shadow_bias_min);

    float shadow     = 0.0;
    vec2  texel_size = 1.0 / vec2(textureSize(u_shadow_map, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcf_depth = texture(u_shadow_map,
                                      proj.xy + vec2(x, y) * texel_size).r;
            shadow += (proj.z - bias) > pcf_depth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 phong_directional(DirectionalLight light, vec3 normal, vec3 view_dir,
                       vec3 albedo, vec3 specular_color, float shadow) {
    vec3  light_dir   = normalize(-light.direction);
    float diff        = max(dot(normal, light_dir), 0.0);
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    vec3  diffuse     = light.color * diff * albedo;
    vec3  specular    = light.color * spec * specular_color;
    return (diffuse + specular) * light.intensity * (1.0 - shadow);
}

vec3 phong_point(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir,
                 vec3 albedo, vec3 specular_color) {
    vec3  light_dir   = normalize(light.position - frag_pos);
    float diff        = max(dot(normal, light_dir), 0.0);
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
    float distance    = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant
                             + light.linear    * distance
                             + light.quadratic * distance * distance);
    vec3  diffuse  = light.color * diff * albedo;
    vec3  specular = light.color * spec * specular_color;
    return (diffuse + specular) * light.intensity * attenuation;
}

void main() {
    vec3 albedo         = texture(u_material.diffuse,  v_uv).rgb;
    vec3 specular_color = texture(u_material.specular, v_uv).rgb;
    vec3 normal         = compute_normal();
    vec3 view_dir       = normalize(u_view_position - v_world_position);

    vec3 result = albedo * u_ambient_factor;
    if (u_dir_light.enabled != 0) {
        vec3  light_dir = normalize(-u_dir_light.direction);
        float shadow    = compute_shadow(normal, light_dir);
        result += phong_directional(u_dir_light, normal, view_dir,
                                    albedo, specular_color, shadow);
    }

    int n = min(u_num_point_lights, MAX_POINT_LIGHTS);
    for (int i = 0; i < n; ++i) {
        result += phong_point(u_point_lights[i], normal, v_world_position,
                              view_dir, albedo, specular_color);
    }

    frag_color = vec4(result, 1.0);
}
