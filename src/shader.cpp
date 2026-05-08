#include "aurora/shader.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace aurora {

namespace {

std::string read_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open shader file: " + path.string());
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

const char* stage_name(GLenum stage) {
    switch (stage) {
        case GL_VERTEX_SHADER:   return "vertex";
        case GL_FRAGMENT_SHADER: return "fragment";
        case GL_GEOMETRY_SHADER: return "geometry";
        case GL_COMPUTE_SHADER:  return "compute";
        default:                 return "unknown";
    }
}

GLuint compile_stage(GLenum stage, const std::string& source, const std::filesystem::path& path) {
    GLuint shader = glCreateShader(stage);
    const char* src = source.c_str();
    const GLint length = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);

    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(static_cast<size_t>(log_length > 0 ? log_length : 1), '\0');
        if (log_length > 0) {
            glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        }
        spdlog::error("Shader compile failed [{}] '{}':\n{}",
                      stage_name(stage), path.string(), log.c_str());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed: " + path.string());
    }
    return shader;
}

}

Shader::Shader(const std::filesystem::path& vertex_path,
               const std::filesystem::path& fragment_path) {
    const std::string vert_source = read_file(vertex_path);
    const std::string frag_source = read_file(fragment_path);

    GLuint vert = compile_stage(GL_VERTEX_SHADER, vert_source, vertex_path);
    GLuint frag = 0;
    try {
        frag = compile_stage(GL_FRAGMENT_SHADER, frag_source, fragment_path);
    } catch (...) {
        glDeleteShader(vert);
        throw;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vert);
    glAttachShader(program_, frag);
    glLinkProgram(program_);

    glDetachShader(program_, vert);
    glDetachShader(program_, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLint link_status = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        GLint log_length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(static_cast<size_t>(log_length > 0 ? log_length : 1), '\0');
        if (log_length > 0) {
            glGetProgramInfoLog(program_, log_length, nullptr, log.data());
        }
        spdlog::error("Shader link failed (vert='{}', frag='{}'):\n{}",
                      vertex_path.string(), fragment_path.string(), log.c_str());
        glDeleteProgram(program_);
        program_ = 0;
        throw std::runtime_error("Shader program link failed");
    }

    spdlog::debug("Shader linked: vert='{}' frag='{}' program={}",
                  vertex_path.string(), fragment_path.string(), program_);
}

Shader::~Shader() {
    if (program_ != 0) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

Shader::Shader(Shader&& other) noexcept
    : program_(std::exchange(other.program_, 0)),
      uniform_cache_(std::move(other.uniform_cache_)) {}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (program_ != 0) {
            glDeleteProgram(program_);
        }
        program_       = std::exchange(other.program_, 0);
        uniform_cache_ = std::move(other.uniform_cache_);
    }
    return *this;
}

void Shader::bind() const {
    glUseProgram(program_);
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLint Shader::uniform_location(const std::string& name) {
    if (auto it = uniform_cache_.find(name); it != uniform_cache_.end()) {
        return it->second;
    }
    const GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc < 0) {
        spdlog::debug("Uniform '{}' not found / inactive in program {}", name, program_);
    }
    uniform_cache_.emplace(name, loc);
    return loc;
}

void Shader::set_uniform(const std::string& name, const glm::mat4& value) {
    const GLint loc = uniform_location(name);
    if (loc < 0) return;
    glProgramUniformMatrix4fv(program_, loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_uniform(const std::string& name, const glm::vec3& value) {
    const GLint loc = uniform_location(name);
    if (loc < 0) return;
    glProgramUniform3fv(program_, loc, 1, glm::value_ptr(value));
}

void Shader::set_uniform(const std::string& name, float value) {
    const GLint loc = uniform_location(name);
    if (loc < 0) return;
    glProgramUniform1f(program_, loc, value);
}

void Shader::set_uniform(const std::string& name, int value) {
    const GLint loc = uniform_location(name);
    if (loc < 0) return;
    glProgramUniform1i(program_, loc, value);
}

}
