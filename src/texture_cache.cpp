#include "aurora/texture_cache.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>

namespace aurora {

namespace {

std::unordered_map<std::string, std::weak_ptr<Texture>>& cache() {
    static std::unordered_map<std::string, std::weak_ptr<Texture>> m;
    return m;
}

}

std::shared_ptr<Texture> TextureCache::get(const std::filesystem::path& path, TextureType type) {
    std::error_code ec;
    auto canonical = std::filesystem::weakly_canonical(path, ec);
    const std::string key = (ec ? path : canonical).string();

    auto& m = cache();
    if (auto it = m.find(key); it != m.end()) {
        if (auto live = it->second.lock()) {
            return live;
        }
        m.erase(it);
    }

    auto tex = std::make_shared<Texture>(canonical.empty() ? path : canonical, type);
    m.emplace(key, tex);
    return tex;
}

}
