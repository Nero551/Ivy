#pragma once
#include "Math/Common/Logarithms.hpp"
#include "Math/DimensionalAnalysis/DerivedDimensionals.hpp"
#include "Math/DimensionalAnalysis/Dimension.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Modules/Physics/Units.hpp"
#include "Utilities/Log.hpp"

namespace Sketch
{
//TODO- the size of transform component is whats bottlenecking.
// split it. atleast split transform from the matrices (model matrix, normal matrix)

//TODO- add operator<< to all custom data structures. clean up datHa structure code.

//TODO- If converting a general Quaternion to a rotation quaternion proves
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion)
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO- remove matrices from transform component
// make render batches store sparse sets that map entity ids to normal/model matrices.
// and only recompute matrices when global transform.IsChanged.

//TODO- play minecraft in the redstone modpack i made for understanding logic gates.

//TODO- redo Entity completely, make it use handles, add ability to search by entity object not just id/handle.

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
            N::U::Log::Fatal("Array: Out of bounds.");
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
Output Summation(const int start, const int end, const N::M::Function<Input, Output>& f)
    requires(std::is_arithmetic_v<Input>)
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
            N::M::Function<int, unsigned int>{[&](const int i) { return indexArray[i] * Strides[i]; }});

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
    //TODO- found bug, if exponent is 0 in an operation, it doesn't remove it, ex: lien 166-172.
    // as u can see, they should be addable.

    N::M::Dimension<float, N::M::Length<-1>> l1;
    N::M::Dimension<float, N::M::OperationDimensional<N::M::Mass<1>, N::M::Length<-1>>> m1;
    N::M::Dimension<float, N::M::Mass<-1>> m2;

    auto m3 = m1 * m2;

    N::U::Log::Info(l1 + m3);

    N::M::Dimension<float, N::M::Mass<1>> m = 0.30f * N::Units::Kilogram;
    N::M::Dimension<N::M::Vector<2>, N::M::Newton> f1 = N::M::Vector<2>::FromPolar({N::M::Rad(-20), 5});
    N::M::Dimension<N::M::Vector<2>, N::M::Newton> f2 = N::M::Vector<2>::FromPolar({N::M::Rad(60), 8});

    auto netF = f1 + f2;

    auto accel = netF / m;

    // N::U::Log::Info(netF / m);
    // N::U::Log::Info((netF / m)().ToPolar());
}
} // namespace Sketch