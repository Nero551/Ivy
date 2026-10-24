#pragma once
#include "Dimensional.hpp"
#include "Math/Common/Constants.hpp"
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
using Acceleration = OperationDimensional<Velocity, Time<-1>>;
using Force = OperationDimensional<Mass<1>, Acceleration, "N">;

using AngularSpeed = OperationDimensional<Dimensionless, Time<-1>, M::Greek::OMEGA>;

using Area = Length<2>;
using Volume = Length<3>;
using Density = OperationDimensional<Mass<1>, Volume::WithExponent<-1>, M::Greek::RHO>;
using SurfaceDensity = OperationDimensional<Mass<1>, Area::WithExponent<-1>, M::Greek::SIGMA>;

using Joule = OperationDimensional<Force, Length<1>, "J">;

} // namespace Ivy::P
