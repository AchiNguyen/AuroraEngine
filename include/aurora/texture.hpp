#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <memory>

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

    // Lazy 1x1 fallbacks. Created on first call after a GL context exists,
    // shared across all materials that need them.
    static std::shared_ptr<Texture> white_fallback();
    static std::shared_ptr<Texture> black_fallback();

private:
    Texture(int width, int height, const unsigned char* rgba_pixels, TextureType type);
    void release() noexcept;

    GLuint      id_   = 0;
    TextureType type_ = TextureType::Diffuse;
};

}
