#pragma once

#include <glad/glad.h>

#include <filesystem>

namespace aurora {

enum class TextureType { Diffuse, Specular, Normal };

class Texture {
public:
    Texture(const std::filesystem::path& path, TextureType type);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    Texture& operator=(Texture&&) noexcept;

    void bind(unsigned int unit) const;

    GLuint      id()   const { return id_; }
    TextureType type() const { return type_; }

private:
    void release() noexcept;

    GLuint      id_   = 0;
    TextureType type_ = TextureType::Diffuse;
};

}
