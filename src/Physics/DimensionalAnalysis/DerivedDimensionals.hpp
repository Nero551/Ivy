#pragma once
#include "Dimensional.hpp"
namespace Ivy::P
{
using Velocity = OperationDimensional<Length<1>, Time<-1>>;
using Acceleration = OperationDimensional<Velocity, Time<-1>>;
using Force = OperationDimensional<Mass<1>, Acceleration, "N">;

} // namespace Ivy::P
