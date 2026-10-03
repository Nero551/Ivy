#pragma once
#include "Core/World/ECS/Entity.hpp"
namespace N {
struct Entity : C::Entity {
   void Initialize() override {
    Entity::Initialize();
   }
};
}