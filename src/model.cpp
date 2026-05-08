#include "aurora/model.hpp"

#include "aurora/shader.hpp"
#include "aurora/texture_cache.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <spdlog/spdlog.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace aurora {

namespace {

glm::vec3 to_glm(const aiVector3D& v) { return {v.x, v.y, v.z}; }

std::shared_ptr<Texture> load_first_texture(const aiMaterial* mat,
                                            aiTextureType ai_type,
                                            TextureType type,
                                            const std::filesystem::path& dir) {
    if (mat->GetTextureCount(ai_type) == 0) return nullptr;
    aiString name;
    if (mat->GetTexture(ai_type, 0, &name) != AI_SUCCESS) return nullptr;

    const std::filesystem::path full = dir / name.C_Str();
    try {
        return TextureCache::get(full, type);
    } catch (const std::exception& e) {
        spdlog::warn("Texture missing or unreadable, using fallback: '{}' ({})",
                     full.string(), e.what());
        return nullptr;
    }
}

void process_mesh(const aiMesh* ai_mesh, const aiScene* scene,
                  const std::filesystem::path& dir,
                  std::vector<Vertex>& vertices,
                  std::vector<std::uint32_t>& indices,
                  Material& material) {
    vertices.clear();
    vertices.reserve(ai_mesh->mNumVertices);

    const bool has_uv = ai_mesh->HasTextureCoords(0);
    for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
        Vertex v{};
        v.position = to_glm(ai_mesh->mVertices[i]);
        v.normal   = ai_mesh->HasNormals() ? to_glm(ai_mesh->mNormals[i])
                                           : glm::vec3(0.0f, 1.0f, 0.0f);
        v.color    = glm::vec3(1.0f);
        v.uv       = has_uv ? glm::vec2(ai_mesh->mTextureCoords[0][i].x,
                                        ai_mesh->mTextureCoords[0][i].y)
                            : glm::vec2(0.0f);
        vertices.push_back(v);
    }

    indices.clear();
    indices.reserve(static_cast<std::size_t>(ai_mesh->mNumFaces) * 3);
    for (unsigned int f = 0; f < ai_mesh->mNumFaces; ++f) {
        const aiFace& face = ai_mesh->mFaces[f];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    material = {};
    if (ai_mesh->mMaterialIndex < scene->mNumMaterials) {
        const aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];
        material.diffuse_map  = load_first_texture(mat, aiTextureType_DIFFUSE,  TextureType::Diffuse,  dir);
        material.specular_map = load_first_texture(mat, aiTextureType_SPECULAR, TextureType::Specular, dir);
        float shininess = 0.0f;
        if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess > 0.0f) {
            material.shininess = shininess;
        } else {
            material.shininess = 32.0f;
        }
    }
}

}

Model::Model(const std::filesystem::path& path)
    : directory_(path.parent_path()) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path.string().c_str(),
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace
        | aiProcess_OptimizeMeshes);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        spdlog::error("Model load failed '{}': {}",
                      path.string(), importer.GetErrorString());
        throw std::runtime_error("Assimp failed to load: " + path.string());
    }

    std::vector<const aiNode*> stack;
    stack.push_back(scene->mRootNode);

    std::vector<Vertex>        verts;
    std::vector<std::uint32_t> idxs;
    Material                   mat;

    while (!stack.empty()) {
        const aiNode* node = stack.back();
        stack.pop_back();

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            const aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
            if (ai_mesh->mNumVertices == 0 || ai_mesh->mNumFaces == 0) continue;

            process_mesh(ai_mesh, scene, directory_, verts, idxs, mat);
            total_vertices_  += verts.size();
            total_triangles_ += idxs.size() / 3;
            entries_.push_back(MeshEntry{ Mesh(verts, idxs), mat });
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            stack.push_back(node->mChildren[i]);
        }
    }

    std::unordered_set<const Texture*> distinct;
    for (const auto& e : entries_) {
        if (e.material.diffuse_map)  distinct.insert(e.material.diffuse_map.get());
        if (e.material.specular_map) distinct.insert(e.material.specular_map.get());
    }

    spdlog::info("Model '{}' loaded: meshes={} vertices={} triangles={} textures={}",
                 path.string(), entries_.size(), total_vertices_, total_triangles_, distinct.size());
}

void Model::draw(Shader& shader) const {
    for (const auto& entry : entries_) {
        upload(shader, "u_material", entry.material);
        entry.mesh.draw();
    }
}

}
