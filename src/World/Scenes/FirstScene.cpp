#include "FirstScene.hpp"

#include "AssimpScene.hpp"

#include "Graphics/Shader/Uniforms/Vector3Uniform.hpp"
#include "Modules/Renderer/Nodes/MeshInstance3D.hpp"
namespace Ivy
{
FirstScene::FirstScene()
{
    SetRoot(C::World::Get().CreateEntity<Node3D>());
}
} // namespace Ivy
