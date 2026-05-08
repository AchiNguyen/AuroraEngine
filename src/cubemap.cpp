#include "aurora/cubemap.hpp"

#include "stb/stb_image.h"

#include <spdlog/spdlog.h>

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace aurora {

Cubemap::Cubemap(const std::array<std::string, 6>& face_paths) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

    stbi_set_flip_vertically_on_load(false);

    for (std::size_t i = 0; i < face_paths.size(); ++i) {
        int w = 0, h = 0, ch = 0;
        unsigned char* data = stbi_load(face_paths[i].c_str(), &w, &h, &ch, 4);
        if (!data) {
            spdlog::error("Cubemap face {} load failed '{}': {}",
                          i, face_paths[i], stbi_failure_reason());
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            release();
            throw std::runtime_error("Failed to load cubemap face: " + face_paths[i]);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
                     0, GL_RGBA8, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
        spdlog::info("Cubemap face {} '{}' {}x{} src_channels={}",
                     i, face_paths[i], w, h, ch);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap() {
    release();
}

Cubemap::Cubemap(Cubemap&& other) noexcept
    : id_  (std::exchange(other.id_,   0)),
      unit_(other.unit_) {}

Cubemap& Cubemap::operator=(Cubemap&& other) noexcept {
    if (this != &other) {
        release();
        id_   = std::exchange(other.id_, 0);
        unit_ = other.unit_;
    }
    return *this;
}

void Cubemap::release() noexcept {
    if (id_ != 0) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

void Cubemap::bind(unsigned int unit) const {
    unit_ = unit;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
}

}
