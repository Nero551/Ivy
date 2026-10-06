#pragma once
#include "Math/Common/Exponentials.hpp"
#include "Math/Equations/LinearEquation.hpp"
#include "Math/Equations/LinearSystem.hpp"
#include "Math/Functions/Function.hpp"
#include "Physics/DimensionalAnalysis/DerivedDimensionals.hpp"
#include "Physics/DimensionalAnalysis/Dimension.hpp"
#include "Physics/DimensionalAnalysis/FundamentalDimensionals.hpp"
#include "Protos/LogicPrototype.hpp"
#include "Utilities/Log.hpp"

namespace Sketch
{
//TODO - If converting a general Quaternion to a rotation quaternion proves
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion)
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO - play minecraft in the redstone modpack i made for understanding logic gates.

//TODO - redo Entity completely, make it use handles, add ability to search by entity object not just id/handle.
//
//TODO - try to make dependency injection to avoid global accessors like Engine::Get().
// try to decouple as much as possible before attempting to implement DI(dependency injection)

//TODO - probably wanna rethink my entire architecture (Core + Modules + World).
// the other stuff can be extracted out of this project and still work.
// so they dont count as "part of the architecture".

//TODO - it appears that clion is not seeing my pch?

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

template <typename Input, typename Output>
Output Summation(const int start, const int end, const M::Function<Input, Output>& f)
    requires(std::convertible_to<int, Input> && M::Additive<Output, Output>)
{
    Output result{};

    for (int i = start; i <= end; ++i)
    {
        result += f(static_cast<Input>(i));
    }

    return result;
}

template <unsigned int... Dimensions> struct Tensor
{
    static constexpr unsigned int Order = sizeof...(Dimensions);
    static constexpr unsigned int Size = (Dimensions * ...);

    constexpr Tensor() {}
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
        unsigned int flatIndex = Summation(0, Order - 1,
            Ivy::M::Function<int, unsigned int>{[&](const int i) { return indexArray[i] * Strides[i]; }});

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
        const std::array<unsigned int, Order> dimensions, unsigned int dimension, unsigned int flatIndex,
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

inline void Test()
{
    ALU alu;

    uInt8 c = {34};
    uInt8 d = {33};
    // Ivy::U::Log::Info(alu.Add(c, d));
    // Ivy::U::Log::Info(alu.Zero);
    // Ivy::U::Log::Info(alu.Overflow);

    P::Dimension<float, P::Velocity> speed{20};
    P::Dimension<float, P::Length<1>> dist{115};

    P::Dimension<float, P::Acceleration> accel = (speed * speed) / (2 * dist);
    ;
    P::Dimension<float, P::Acceleration> g{9.8};

    float coefficient = accel / g;
    //
    U::Log::Info(coefficient);
}
} // namespace Sketch
