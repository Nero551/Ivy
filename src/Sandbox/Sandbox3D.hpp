#pragma once

#include "Core/Engine.hpp"
#include "Core/TimeHandler.hpp"
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Constraints.hpp"
#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Platform/CursorMode.hpp"
#include "Platform/Global.hpp"
#include "Platform/Key.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Systems/AssetServer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include <cmath>
#include <vector>

namespace Ivy::Sandbox
{
namespace Sandbox3D
{

inline void Start_Sandbox3D(C::Engine& eng)
{
    Platform::Input.SetMouseMode(Platform::CursorMode::Disabled);

    auto& query = eng.ECS.Query;
    auto& camera = Camera3D::Create(eng.ECS);
    query.GetComponent<CMP::Transform3D>(camera).Position = {0, 0, 10};
    eng.World.GetRoot().AttachChild(camera);
    eng.World.SetCamera(camera);

    eng.World.GetRoot().AttachChild(CoordinateAxes3DScene(eng));

    auto& resources = eng.GetRes<AssetServer>();
    G::Mesh m = G::Primitives::CreateArrow3D("mvPoint");
    m.CullMode = G::CullMode::None;
    auto& mesh = resources.LoadAsset<G::Mesh>(m);

    auto& material = resources.LoadAsset<G::Material>("mvMat");

    auto& shader = resources.LoadAsset<G::Shader>("mvShader");
    shader().AssignSource(
        resources.LoadAsset<G::ShaderSource>("mvVert", "assets/Shaders/arrow.vert", G::ShaderStage::Vertex));
    shader().AssignSource(resources.LoadAsset<G::ShaderSource>(
        "mvFrag", "assets/Shaders/arrow.frag", G::ShaderStage::Fragment));

    material().Shader = &shader();

    std::vector<float> magnitudes{};
    float max = 5.0f;
    float min = -5.0f;
    M::Function<M::Vector<2>, float, float> f = [](float x, float y)
    { return M::Vector<2>{x * y, y * y - x * x}; };
    auto dvdx = M::PartialDerivative<0>(f);
    auto dvdy = M::PartialDerivative<1>(f);
    for (float x = min; x < max; x += 0.5)
    {
        for (float y = min; y < max; y += 0.5)
        {
            M::Vector<3> pos = {x, y, 0};
            M::Vector<3> dir = {f(x, y).x, f(x, y).y, 0};
            magnitudes.push_back(f(x, y).Magnitude() / 5.);

            auto& point = MeshInstance3D::Create(eng.ECS);
            query.GetComponent<CMP::Mesh>(point).Mesh = &mesh;
            query.GetComponent<CMP::Material>(point).Material = &material;

            auto& transform = query.GetComponent<CMP::Transform3D>(point);
            transform.Scale = M::Vector<3>{0.4f};
            transform.Position = pos;
            transform.Rotation = M::Quaternion<>::FromTo({1, 0, 0}, M::Vector<3>{dir.x, dir.y, 0});
            eng.World.GetRoot().AttachChild(point);
        }
    }

    G::ArrayBuffer buffer{};
    buffer.SetData(magnitudes);

    mesh.Get().VAO.SetArrayBuffer(buffer, 2, sizeof(float));
    mesh.Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
    mesh.Get().VAO.SetAttribDivisor(2, 1);

    // StressTester3D::Spawn(eng, 4, 3);
    // VectorFields3D::LorenzSystem(eng);

    // auto& square = MeshInstance3D::Create(eng.ECS);
    // query.GetComponent<CMP::Mesh>(square).Mesh =
    //     &eng.GetRes<AssetServer>().LoadAsset<G::Mesh>(G::Primitives::CreateQuad("TerrainQuad", 20, 20));

    // auto& vertSource = eng.GetRes<AssetServer>().LoadAsset<G::ShaderSource>(
    //     "TerrainShaderVert", "assets/Shaders/TerrainShader.vert", G::ShaderStage::Vertex);

    // auto& fragSource = eng.GetRes<AssetServer>().LoadAsset<G::ShaderSource>(

    //     "TerrainShaderFrag", "assets/Shaders/TerrainShader.frag", G::ShaderStage::Fragment);
    // auto& shader = eng.GetRes<AssetServer>().LoadAsset<G::Shader>("TerrainShader", vertSource, fragSource);
    // auto& mat = eng.GetRes<AssetServer>().LoadAsset<G::Material>("TerrainMat");
    // mat().Shader = &shader();
    // mat().Color = {1, 1, 1, 1};
    // shader().HotReload = true;

    // query.GetComponent<CMP::Material>(square).Material = &mat;
    // auto& transform = query.GetComponent<CMP::Transform3D>(square);
    // transform.Rotation = M::Quaternion<>::FromEulerXYZ({M::Rad(90.0f), 0, 0});
    // transform.Scale = {5, 5, 1};
    // eng.World.GetRoot().AttachChild(square);
}
inline void FixedUpdate_Sandbox3D(C::Engine& eng, const C::TimeHandler& time)
{
    if (Platform::Input.IsKeyHeld(Platform::Key::Escape))
    {
        eng.Shutdown();
    }

    // StressTester3D::Update_StressTester3D(eng, time);
}
} // namespace Sandbox3D
} // namespace Ivy::Sandbox
