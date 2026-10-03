#pragma once

#include "Core/Module.hpp"
namespace N {
struct Module : C::Module {
protected:
   void OnStart() override;
   void OnUpdate(double dt) override;
};
}