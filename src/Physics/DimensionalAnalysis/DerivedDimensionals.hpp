#pragma once
#include "FundamentalDimensionals.hpp"
#include "OperationDimensional.hpp"
namespace Ivy::P
{
using Velocity = OperationDimensional<Length<1>, Time<-1>>;
using Acceleration = OperationDimensional<Velocity, Time<-1>>;
using Force = OperationDimensional<Mass<1>, Acceleration, "N">;
using Density = OperationDimensional<Mass<1>, Length<-3>>;

} // namespace Ivy::P