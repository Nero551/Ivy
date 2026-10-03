#pragma once
#include "Core/World/ECS/System.hpp"

namespace N {
struct System : C::System {
    void Start() override;
};
}