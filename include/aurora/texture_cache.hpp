#pragma once

#include "aurora/texture.hpp"

#include <filesystem>
#include <memory>

namespace aurora {

// Process-wide dedup for textures keyed by absolute path. Multiple meshes inside
// a single model often reference the same image file, so loading it once and
// sharing the GL handle saves memory and load time.
class TextureCache {
public:
    static std::shared_ptr<Texture> get(const std::filesystem::path& path, TextureType type);
};

}
