#include "aurora/texture.hpp"

#include "stb/stb_image.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

namespace aurora {

namespace {

const char* type_name(TextureType type) {
    switch (type) {
        case TextureType::Diffuse:  return "diffuse";
        case TextureType::Specular: return "specular";
        case TextureType::Normal:   return "normal";
    }
    return "unknown";
}

}

Texture::Texture(const std::filesystem::path& path, TextureType type)
    : type_(type) {
    stbi_set_flip_vertically_on_load(true);

    int width = 0, height = 0, channels = 0;
    const std::string path_str = path.string();
    unsigned char* data = stbi_load(path_str.c_str(), &width, &height, &channels, 4);
    if (!data) {
        spdlog::error("Texture load failed [{}] '{}': {}",
                      type_name(type), path_str, stbi_failure_reason());
        throw std::runtime_error("Failed to load texture: " + path_str);
    }

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    spdlog::debug("Texture loaded [{}] '{}' {}x{} src_channels={} id={}",
                  type_name(type), path_str, width, height, channels, id_);
}

Texture::~Texture() {
    release();
}

Texture::Texture(Texture&& other) noexcept
    : id_(std::exchange(other.id_, 0)),
      type_(other.type_) {}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        release();
        id_   = std::exchange(other.id_, 0);
        type_ = other.type_;
    }
    return *this;
}

void Texture::release() noexcept {
    if (id_ != 0) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id_);
}

}
