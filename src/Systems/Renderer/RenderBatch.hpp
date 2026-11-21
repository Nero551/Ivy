#pragma once
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/Buffers/BufferUsage.hpp"
#include "Graphics/Buffers/VertexArray/VertexArray.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Systems/Asset.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <cstddef>
#include <functional>
#include <utility>

namespace Ivy
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
    U::CheckedPtr<Asset<G::Material>> Material;
    U::CheckedPtr<Asset<G::Mesh>> Mesh;
    U::SparseSetSoA<InstanceData> Instances;
    G::ArrayBuffer Buffer;
    G::VertexArray VAO;
    bool DataChanged = false;

    RenderBatch(const U::CheckedPtr<Asset<G::Mesh>>& mesh, const U::CheckedPtr<Asset<G::Material>>& mat)
        : Material(mat), Mesh(mesh)
    {
        Buffer.Usage = G::BufferUsage::DynamicDraw;
        Buffer.Generate();
    }

    void Render()
    {
        Material->Get().Use();

        Mesh->Get().VAO.SetArrayBuffer(Buffer, 1, sizeof(InstanceData));
        unsigned int instanceCount = Instances.Size();
        if (DataChanged)
        {
            DataChanged = false;
            Buffer.SetData(Instances.Data());
            Mesh->Get().VAO.SetMatrix4AttribPointer(4, offsetof(InstanceData, ModelMatrix), 1);
            Mesh->Get().VAO.SetMatrix3AttribPointer(8, offsetof(InstanceData, NormalMatrix), 1);
            Mesh->Get().VAO.SetAttribDivisor(1, 1);
        }

        Mesh->Get().DrawInstanced(instanceCount);
    }
};
} // namespace Ivy
