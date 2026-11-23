#pragma once

#include "Diagnostics/Log.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Logarithms.hpp"
#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Scalar.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vectors.hpp"
#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

namespace Sketch
{
//TODO: If converting a general Quaternion to a rotation quaternion proves \
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion) \
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO: rethink the render pipeline (look at bevy's render graph. bevy is pretty nice)
// TODO: need particle system (cpu particle & gpu particle(compute shaders))
// TODO: first thing i should do after physics is compute shaders and SSBO, actually finish LearnOpenGL.
//TODO: use a DAG to represent render pipeline, dependencies between render passes just like system.

//TODO: all this time i been thinking how to make cpu side more automatic for graphics. \
//i should be thinking about how to make cpu -> gpu pipeline easier to MODIFY and use. not make gpu/shaders as automatic as possible.

// TODO: define VariableCount trait. 1 for floats,double,int,etc, then each math type specifies its number of variables. \
// this will be needed for partial derivatives specifically.
//
//TODO: concepts i might need: IsQuaternion, IsVector, IsComplex, IsMatrix, IsODE, IsFunction, IsMathTuple, IsLinearEq, IsLinearSys etc \
// define them using the IsSpec family in Utilities/IsSpecialization.hpp
//
// TODO: prevent math tuples from being `input-able to Function.
//
// TODO: setup custom clang-tidy module/ check (IvyTidy)
//TODO: new performance bottleneck: renderer. use gpu rendering, compute shaders

//TODO: Utilities crate needs a nice cleaning. especially data structures. they are packed with all kinds of unnecessary edge case handling.
//TODO: i need to formally define convergence and divergence & limits (numerically ofc).
//
// TODO: automatic differentiation via dual numbers.
//
// TODO: tracy is not working, the macro which defines it when debug mode doesn't work.
//
//? as for thread safety. i think that should be integrated into the engine/ecs itself.
//? think about deferred structural changes. for example, AddEntity() checks if any threads are reading, if yes. defer the change till after.

//TODO: cool names for my libraries. ex: "Flow" for physics. "Bloom" for graphics,

using namespace Ivy;

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
        D::Log::Assert(Real == 0, "[Dual] An Inverse Exists Only If Real Part a != 0");
        return Conjugate() / MagnitudeSquared();
    }
};

template <M::Scalar In, typename Out> Out Summation(const M::Function<Out, In>& f, In start, In end)
{
    Out result{};
    for (In i = start; i <= end; ++i)
    {
        result += f(i);
    }
    return result;
};

template <M::Scalar In, typename Out> Out Product(const M::Function<Out, In>& f, In start, In end)
{
    Out result{};
    for (In i = start; i <= end; ++i)
    {
        result *= f(i);
    }
    return result;
};

template <unsigned int K, unsigned int Component, M::Scalar T = float> struct KVector
{

  private:
    std::array<std::array<T, Component>, K> m_Data{};
};

template <unsigned int Component, M::Scalar T> struct KVector<1, Component, T>
{
};

template <unsigned int Component, M::Scalar T> using Bivector = KVector<2, Component, T>;

static M::Set<unsigned int> N = [](unsigned int x) { return true; };
static M::Set<int> Z = [](int x) { return true; };
static M::Set<float> R = [](float x) { return true; };

template <unsigned int Index, typename O, M::Scalar... Is>
M::Function<O, Is...> PartialDerivative(
    const M::Function<O, Is...>& f, std::tuple_element_t<Index, std::tuple<Is...>> dx = 0.001f)
{
    return [&f, dx](const Is&... ins)
    {
        auto args = std::tuple<Is...>{ins...};

        return [&f, &args, dx]<std::size_t... Indices>(std::index_sequence<Indices...>)
        {
            using Param = std::tuple_element_t<Index, std::tuple<Is...>>;
            return M::Derivative(
                M::Function<O, Param>{[&](const Param& x)
                    { return std::invoke(f, (Indices == Index ? x : std::get<Indices>(args))...); }},
                std::get<Index>(args), dx);
        }(std::index_sequence_for<Is...>{});
    };
}

template <M::Scalar O, M::Scalar... Is>
M::Function<M::Vector<sizeof...(Is), O>, Is...> Gradient(const M::Function<O, Is...>& f)
{
    return {[&f](const Is&... ins)
        {
            return [&f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
            { return M::Vector<sizeof...(Is), O>{PartialDerivative<Indices>(f)(ins...)...}; }(
                std::index_sequence_for<Is...>{});
        }};
}

template <M::Scalar T, typename O, M::Scalar... Is>
M::Function<O, Is...> DirectionalDerivative(
    const M::Function<O, Is...>& f, const M::Vector<sizeof...(Is), T>& v)
{
    return [&f, v](const Is&... ins)
    {
        return [&v, &f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
        { return ((v(Indices) * PartialDerivative<Indices>(f)(ins...)) + ...); }(
            std::index_sequence_for<Is...>{});
    };
}
template <typename O, M::Scalar I>
M::Function<float, I> Curvature(const M::Function<O, I>& f) requires(requires(O out) {
    out.Normalized();
    out.Magnitude();
})
{
    auto derivative = M::Differentiate(f);
    auto unitTangent = derivative.template Op<&O::Normalized>();
    auto tangentDerivative = M::Differentiate(unitTangent);
    return tangentDerivative.template Op<&O::Magnitude>() / derivative.template Op<&O::Magnitude>();
}
inline void Test()
{
    //TODO: get rid of Integral, Derivative, etc. just keep the function returning versions and call those Integral, Derivative, etc.
    //TODO: remove EvaluateInverse(), keep only Inverse().
    //TODO: rework vectors to support future k-vectors (watch intro to geometric algebra)
    //TODO: currently partial derivative only supports multi float inputs, no vector, quaternion, etc. do that.
    // M::Function<float, float, float, float> f = [](float x, float y, float z) { return x * x * M::Ln(z); };
    // D::Log::Info(PartialDerivative<0>(f)(-1, 0, M::E));
    // D::Log::Info(Gradient(f)(-1, 0, M::E));
    // D::Log::Info(DirectionalDerivative(f, M::Vector<3>{1, 0, 0})(-1, 0, M::E));

    // M::Function<M::Vector<3>, float> g = [](float t)
    // { return M::Vector<3>{std::cos(t), std::sin(t), t / 5.0f}; };
    // D::Log::Info(Curvature(g)(10));

    // D::Log::Info(N.Cartesian(Z, R).Belongs({5, -5, 0.5f}));
}

} // namespace Sketch
