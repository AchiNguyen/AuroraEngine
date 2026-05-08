#pragma once

#include "aurora/material.hpp"
#include "aurora/mesh.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace aurora {

class Shader;

class Model {
public:
    explicit Model(const std::filesystem::path& path);
    Model(Mesh mesh, Material material);

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;

    void draw(Shader& shader) const;

    std::size_t mesh_count()     const { return entries_.size(); }
    std::size_t vertex_count()   const { return total_vertices_; }
    std::size_t triangle_count() const { return total_triangles_; }

    Material& primary_material() {
        return entries_.empty() ? fallback_material_ : entries_.front().material;
    }

private:
    struct MeshEntry {
        Mesh     mesh;
        Material material;
    };

    std::vector<MeshEntry> entries_;
    std::filesystem::path  directory_;
    std::size_t            total_vertices_  = 0;
    std::size_t            total_triangles_ = 0;
    Material               fallback_material_{};
};

}
