#pragma once
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"

namespace Ivy::R
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
    U::CheckedPtr<C::Resource<G::Material>> Material;
    U::CheckedPtr<C::Resource<G::Mesh>> Mesh;
    U::SparseSetSoA<InstanceData> Instances;
    G::ArrayBuffer Buffer;
    G::VertexArray VAO;

    RenderBatch(
        const U::CheckedPtr<C::Resource<G::Mesh>>& mesh, const U::CheckedPtr<C::Resource<G::Material>>& mat)
        : Material(mat), Mesh(mesh)
    {
        Buffer.Usage = G::BufferUsage::DynamicDraw;
        Buffer.Generate();
    }

    void Render()
    {
        Material->Get().Use();

        unsigned int instanceCount = Instances.Size();
        Buffer.SetData(Instances.Data());

        Mesh->Get().Generate();
        Mesh->Get().VAO.SetVertexBuffer(Buffer, 1, sizeof(InstanceData));

        Mesh->Get().VAO.SetMatrix4AttribPointer(4, offsetof(InstanceData, ModelMatrix), 1);
        Mesh->Get().VAO.SetMatrix3AttribPointer(8, offsetof(InstanceData, NormalMatrix), 1);
        Mesh->Get().VAO.SetAttribDivisor(1, 1);

        Mesh->Get().DrawInstanced(instanceCount);
    }

    using BatchKey = std::pair<struct G::Material*, struct G::Mesh*>;
    struct BatchKeyHash
    {
        std::size_t operator()(const BatchKey& key) const
        {
            const std::size_t h1 = std::hash<struct G::Material*>{}(key.first);
            const std::size_t h2 = std::hash<struct G::Mesh*>{}(key.second);

            return h1 ^ (h2 << 1);
        }
    };
};
} // namespace Ivy::R
