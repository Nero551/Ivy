#include "AssimpScene.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <filesystem>
#include <string>
#include <vector>

#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Core/World/World.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Graphics/Texture/Texture2D.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"
#include "Modules/Renderer/Components/MaterialComponent.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Utilities/Debug/Log.hpp"
#include "World/Nodes/Node3D.hpp"

namespace Ivy
{

static void ProcessVertices(std::vector<G::Vertex>& vertices, const aiMesh* mesh)
{
    for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
    {
        M::Vector<4> pos = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z, 1};
        M::Vector<3> normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
        M::Vector<2> uv = {0, 0};

        if (mesh->mTextureCoords[0])
        {
            uv = {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y};
        }

        vertices.emplace_back(pos, M::Vector<4>(1), uv, normal);
    }
}

static void ProcessFaces(std::vector<unsigned int>& indices, const aiMesh* mesh)
{
    for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
    {
        aiFace face = mesh->mFaces[f];

        for (unsigned int i = 0; i < face.mNumIndices; ++i)
        {
            indices.push_back(face.mIndices[i]);
        }
    }
}

static C::Resource<G::Material>& ProcessMaterial(
    const aiScene* scene, const aiMesh* mesh, const std::string& directory)
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    auto& material = resourceManager.Load<G::Material>("material_" + std::to_string(mesh->mMaterialIndex));

    material().Shader = &resourceManager.Load<G::Shader>("s")();

    material().Shader->AssignSource(
        resourceManager.Load<G::ShaderSource>("s", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));

    material().Shader->AssignSource(
        resourceManager.Load<G::ShaderSource>("s", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));

    aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

    for (unsigned int t = 0; t < aiMat->GetTextureCount(aiTextureType_DIFFUSE); ++t)
    {
        aiString str;
        aiMat->GetTexture(aiTextureType_DIFFUSE, t, &str);

        auto& diffuseMap = resourceManager.Load<G::Texture2D>("diffuse" + std::to_string(t));
        diffuseMap().UseImage(U::Image{std::filesystem::path(directory) / str.C_Str(), true});

        material().DiffuseMap = &diffuseMap();
    }

    for (unsigned int t = 0; t < aiMat->GetTextureCount(aiTextureType_SPECULAR); ++t)
    {
        aiString str;
        aiMat->GetTexture(aiTextureType_SPECULAR, t, &str);

        auto& specularMap = resourceManager.Load<G::Texture2D>("specular" + std::to_string(t));
        specularMap().UseImage(U::Image{std::filesystem::path(directory) / str.C_Str(), true});

        material().SpecularMap = &specularMap();
    }

    return material;
}

static void ProcessNode(
    const aiNode* node, const aiScene* scene, const std::string& directory, C::Entity& parent)
{
    auto& world = C::World::Get();
    auto& query = world.Query;
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    auto& entity = world.CreateEntity<Node3D>();

    auto& meshPool = query.Pool<R::MeshComponent>();
    auto& materialPool = query.Pool<R::MaterialComponent>();

    for (unsigned int m = 0; m < node->mNumMeshes; ++m)
    {
        std::vector<G::Vertex> vertices;
        std::vector<unsigned int> indices;

        aiMesh* mesh = scene->mMeshes[node->mMeshes[m]];

        ProcessVertices(vertices, mesh);
        ProcessFaces(indices, mesh);

        auto& material = ProcessMaterial(scene, mesh, directory);

        auto& meshResource = resourceManager.Load<G::Mesh>("mesh_" + std::to_string(node->mMeshes[m]));
        meshResource().Vertices = vertices;
        meshResource().Indices = indices;

        meshPool.Add(entity.GetId()).Mesh = &meshResource;
        materialPool.Add(entity.GetId()).Material = &material;
    }

    parent.AttachChild(entity);

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene, directory, entity);
    }
}

AssimpScene::AssimpScene(const std::string& filepath)
{
    Assimp::Importer importer;
    SetRoot(C::World::Get().CreateEntity<Node3D>());

    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        U::Log::Error("[ASSIMP] Failed To Load Scene: ", importer.GetErrorString());

        return;
    }

    std::string directory = filepath.substr(0, filepath.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene, directory, GetRoot());
}
} // namespace Ivy
