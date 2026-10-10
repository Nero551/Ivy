#include "FirstScene.hpp"

#include "World/Nodes/Node3D.hpp"
namespace Ivy
{
FirstScene::FirstScene()
{
    SetRoot(C::World::Get().CreateEntity<Node3D>());
}
} // namespace Ivy
