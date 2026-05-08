#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

namespace aurora {

class Shader {
public:
    Shader(const std::filesystem::path& vertex_path,
           const std::filesystem::path& fragment_path);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;

    void bind() const;
    void unbind() const;

    GLuint id() const { return program_; }

    void set_uniform(const std::string& name, const glm::mat4& value);
    void set_uniform(const std::string& name, const glm::vec3& value);
    void set_uniform(const std::string& name, float value);
    void set_uniform(const std::string& name, int value);

private:
    GLint uniform_location(const std::string& name);

    GLuint program_ = 0;
    std::unordered_map<std::string, GLint> uniform_cache_;
};

}
