#pragma once
#include "FundamentalDimensionals.hpp"
#include "OperationDimensional.hpp"
namespace N::P
{
using Velocity = OperationDimensional<Length<1>, Time<-1>>;
using Acceleration = OperationDimensional<OperationDimensional<Length<1>, Time<-1>>, Time<-1>>;
using Newton = OperationDimensional<Mass<1>, Acceleration, "N">;

} // namespace N::P