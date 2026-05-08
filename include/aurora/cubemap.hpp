#pragma once

#include <glad/glad.h>

#include <array>
#include <string>

namespace aurora {

class Cubemap {
public:
    explicit Cubemap(const std::array<std::string, 6>& face_paths);
    ~Cubemap();

    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;
    Cubemap(Cubemap&&) noexcept;
    Cubemap& operator=(Cubemap&&) noexcept;

    void bind(unsigned int unit) const;

    GLuint       id()   const { return id_; }
    unsigned int unit() const { return unit_; }

private:
    void release() noexcept;

    GLuint               id_   = 0;
    mutable unsigned int unit_ = 0;
};

}
