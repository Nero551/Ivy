#pragma once
#include "Math/Matrix/Matrix4.hpp"
#include "Resources/Material/Material.hpp"
#include "Resources/Mesh/Mesh.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"

namespace N::G
{
struct InstanceData
{
    M::Matrix<4, 4> ModelMatrix;
    M::Matrix<3, 3> NormalMatrix;

    InstanceData(const M::Matrix<4, 4>& model, const M::Matrix<3, 3>& normal)
        : ModelMatrix(model), NormalMatrix(normal)
    {
    }
};

struct RenderBatch
{
    U::CheckedPtr<Material> Material;
    U::CheckedPtr<Mesh> Mesh;
    U::SparseSetSoA<InstanceData> Instances;
    ArrayBuffer Buffer;
    VertexArray VAO;

    RenderBatch(const U::CheckedPtr<struct Mesh>& mesh, const U::CheckedPtr<struct Material>& mat)
        : Material(mat), Mesh(mesh)
    {
        Buffer.Usage = BufferUsage::DynamicDraw;
        Buffer.Generate();
    }

    void Render()
    {
        Material->Use();

        unsigned int instanceCount = Instances.Size();
        Buffer.SetData(Instances.Data());

        Mesh->Generate();
        Mesh->VAO.SetVertexBuffer(Buffer, 1, sizeof(InstanceData));

        Mesh->VAO.SetMatrix4AttribPointer(4, offsetof(InstanceData, ModelMatrix), 1);
        Mesh->VAO.SetMatrix3AttribPointer(8, offsetof(InstanceData, NormalMatrix), 1);
        Mesh->VAO.SetAttribDivisor(1, 1);

        Mesh->DrawInstanced(instanceCount);
    }

    using BatchKey = std::pair<struct Material*, struct Mesh*>;
    struct BatchKeyHash
    {
        std::size_t operator()(const BatchKey& key) const
        {
            const std::size_t h1 = std::hash<struct Material*>{}(key.first);
            const std::size_t h2 = std::hash<struct Mesh*>{}(key.second);

            return h1 ^ (h2 << 1);
        }
    };
};
} // namespace N::G
