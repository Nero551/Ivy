#pragma once
#include "Dimensional.hpp"
#include "Physics/DimensionalAnalysis/Dimensional.hpp"
#include "Utilities/StringUtils.hpp"
namespace Ivy::P
{
template <int Exp> struct Mass : Dimensional<Mass, Exp, 1>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "kg" << U::Superscript(Exp);
    }
};

template <int Exp> struct Length : Dimensional<Length, Exp, 2>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "m" << U::Superscript(Exp);
    }
};

template <int Exp> struct Time : Dimensional<Time, Exp, 3>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "s" << U::Superscript(Exp);
    }
};

using Velocity = OperationDimensional<Length<1>, Time<-1>>;
using AngularSpeed = OperationDimensional<Dimensionless, Time<-1>, "ꞷ">;
using Acceleration = OperationDimensional<Velocity, Time<-1>>;
using Force = OperationDimensional<Mass<1>, Acceleration, "N">;

} // namespace Ivy::P
