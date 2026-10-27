#pragma once

#include "Core/ECS/Component.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Logarithms.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Concepts.hpp"
#include "Math/Equations/LinearEquation.hpp"
#include "Math/Equations/LinearSystem.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Functions/Integration.hpp"
#include "Math/Math.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vector.hpp"
#include "Physics/DimensionalAnalysis/Dimension.hpp"
#include "Physics/DimensionalAnalysis/Dimensional.hpp"
#include "Physics/DimensionalAnalysis/Dimensionals.hpp"
#include "Physics/Units.hpp"
#include "Utilities/Debug/Inspector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/Track.hpp"
#include <array>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <dlfcn.h>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace Sketch
{
//TODO: If converting a general Quaternion to a rotation quaternion proves
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion)
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO: redo Entity completely, make it use handles, add ability to search by entity object not just id/handle.

//TODO: rethink the render pipeline (look at bevy's render graph. bevy is pretty nice)

//TODO: make transform system cache its own vector of the root's children that's updated on events
// that will make it way less expensive performance-wise.
//TODO: not running renderer module sends fps from 15 to 34 (on 1.7m entities).
//TODO: not running renderer + transform system sends fps from 34 to 6500 (on 1.7m entities).
//TODO: its the ForEachDescendant thing.
//
// TODO: use a GetSmallestPool in ComponentPoolQuery instead of using the first pool as the driver.
//
// TODO: need particle system (cpu particle & gpu particle(compute shaders))
// TODO: first thing i should do after physics is compute shaders and SSBO, actually finish LearnOpenGL.

//TODO: all this time i been thinking how to make cpu side easier to use and more optimized & automatic, even for graphics.
//TODO: i should be thinking about how to make cpu -> gpu pipeline easier to MODIFY and use. not make gpu/shaders as automatic as possible.

using namespace Ivy;

using Index = unsigned int;
template <typename T, Index Size> struct Array
{
    static bool Contains(const Index index)
    {
        return index < Size;
    }

    T& At(Index index)
    {
        if (!Contains(index))
        {
            U::Log::Fatal("Array: Out of bounds.");
        }
        return m_Data[index];
    }

    T& operator[](Index index)
    {
        return m_Data[index];
    }

  private:
    T m_Data[Size];
};

template <unsigned int... Dimensions> struct Tensor
{
    static constexpr unsigned int Order = sizeof...(Dimensions);
    static constexpr unsigned int Size = (Dimensions * ...);

    constexpr Tensor() = default;
    constexpr explicit Tensor(float all)
    {
        m_Data.fill(all);
    }

    template <typename... Numbers>
    requires(sizeof...(Numbers) == Size && (std::convertible_to<Numbers, float> && ...))
    constexpr Tensor(Numbers... numbers) : m_Data{static_cast<float>(numbers)...}
    {
    }

    template <typename... Indices>
    constexpr float& operator()(Indices... indices)
        requires(sizeof...(Indices) == Order && (std::integral<Indices> && ...))
    {
        std::array<unsigned int, Order> indexArray{static_cast<unsigned int>(indices)...};
        unsigned int flatIndex = 0;
        for (unsigned int i = 0; i < Order; ++i)
        {
            flatIndex += indexArray[i] * Strides[i];
        }

        return m_Data[flatIndex];
    }

    friend std::ostream& operator<<(std::ostream& os, const Tensor& tensor)
    {
        Print(os, tensor, {Dimensions...}, 0, 0, 0);
        return os;
    }

  private:
    std::array<float, Size> m_Data{0};

    static constexpr std::array<unsigned int, Order> GetStrides()
    {
        constexpr std::array<unsigned int, Order> dimensions = {Dimensions...};
        std::array<unsigned int, Order> strides{};
        unsigned int stride = 1;

        for (int i = Order - 1; i >= 0; --i)
        {
            strides[i] = stride;
            stride *= dimensions[i];
        }

        return strides;
    }

    static constexpr std::array<unsigned int, Order> Strides = GetStrides();

    static void Print(std::ostream& os, const Tensor& tensor,
        const std::array<unsigned int, Order>& dimensions, unsigned int dimension, unsigned int flatIndex,
        const unsigned int indent)
    {
        os << std::string(indent, ' ') << "[\n";

        if (dimension == Order - 1)
        {
            for (unsigned int i = 0; i < dimensions[dimension]; ++i)
            {
                os << std::string(indent + 4, ' ') << tensor.m_Data[flatIndex + i];

                if (i + 1 < dimensions[dimension])
                {
                    os << ' ';
                }
            }

            os << '\n';
        }
        else
        {
            for (unsigned int i = 0; i < dimensions[dimension]; ++i)
            {
                Print(os, tensor, dimensions, dimension + 1, flatIndex + i * tensor.Strides[dimension],
                    indent + 4);
            }
        }

        os << std::string(indent, ' ') << ']';

        if (dimension != 0)
        {
            os << '\n';
        }
    }
};

template <M::Scalar T> struct Dual
{
    T Real{0};
    T Epsilon{0};

    constexpr Dual() = default;

    constexpr explicit Dual(T all) : Real(all), Epsilon(all) {}

    constexpr Dual(T real, T epsilon) : Real(real), Epsilon(epsilon) {}

    constexpr Dual operator+(const Dual& other)
    {
        return {Real + other.Real, Epsilon + other.Epsilon};
    }

    constexpr Dual operator-(const Dual& other)
    {
        return {Real - other.Real, Epsilon - other.Epsilon};
    }

    constexpr Dual operator*(const Dual& other)
    {
        return {Real * other.Real, (Real * other.Epsilon) + (Epsilon * other.Real)};
    }

    constexpr Dual Conjugate()
    {
        return {Real, -Epsilon};
    }

    constexpr T MagnitudeSquared()
    {
        return Real * Real;
    }

    constexpr T Magnitude()
    {
        return std::abs(Real);
    }

    constexpr Dual Inverse()
    {
        U::Log::Assert(Real == 0, "[Dual] An Inverse Exists Only If Real Part a != 0");
        return Conjugate() / MagnitudeSquared();
    }
};

//TODO: add NumberOfDimensions method to all math types
//
//TODO: shaders only have a finite amount of sources(vertex, fragment, geometry, tessellation)
//TODO: drop the sources map in shader and just use members(Vertex, Fragment, Geometry, etc).

inline void Test()
{
    P::Dimension<float, P::Acceleration> g = 9.8;

    P::Dimension<M::Vector<2>, P::Length<1>> deltaR = {2, 3};
    P::Dimension<M::Vector<2>, P::Force> F = {5, 2};

    P::Dimension<float, P::Joule> work = F().Dot(deltaR());

    U::Log::Info(work);

    using StateVector = M::Vector<3, double>;
    using Equation = M::ODE<double, StateVector>;

    const Equation equation{[](const Equation::State& state) -> StateVector
        {
            const double t = state.x;
            const double x = state.y.x;
            const double y = state.y.y;
            const double z = state.y.z;

            return {y + 0.3 * std::sin(3.0 * t) - 0.1 * x * z,
                -x + 0.2 * y * (1.0 - z * z) + 0.15 * std::cos(5.0 * t),
                0.5 * (x * y - z) + 0.1 * std::sin(x * y)};
        }};

    const Equation::State initial{
        0.0, {1.0, 0.0, 0.5}
    };

    U::Log::Info(equation.Solve(initial, 10, 0.0000005));
}
} // namespace Sketch
