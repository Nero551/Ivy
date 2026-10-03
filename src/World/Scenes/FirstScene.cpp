#include "FirstScene.hpp"

#include "AssimpScene.hpp"

#include "Graphics/Shader/Uniforms/Vector3Uniform.hpp"
#include "Modules/Renderer/Novas/MeshInstance3D.hpp"
namespace N
{
FirstScene::FirstScene()
{
    SetRoot(C::World::Get().CreateEntity<Nova3D>());
}
} // namespace N
