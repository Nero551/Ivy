#pragma once
#include "Math/Functions/Function.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Physics/DimensionalAnalysis/DerivedDimensionals.hpp"
#include "Physics/DimensionalAnalysis/Dimension.hpp"
#include "Physics/DimensionalAnalysis/FundamentalDimensionals.hpp"
#include "Physics/Units.hpp"
#include "Utilities/Log.hpp"

#include <bitset>

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

using Bit = bool;

inline Bit NOT(const Bit bit)
{
    return !bit;
}
inline Bit OR(const Bit bit, const Bit bit2)
{
    return bit || bit2;
}
inline Bit AND(const Bit bit, const Bit bit2)
{
    return bit && bit2;
}
inline Bit XOR(const Bit bit, const Bit bit2)
{
    return bit != bit2;
}

template <int Amount> struct Bits
{
    Bits() {}
    template <typename... Args>
    Bits(Args... args) requires(sizeof...(Args) == Amount && (std::convertible_to<Args, Bit> && ...))
        : m_Bits{static_cast<Bit>(args)...}
    {
    }

    Bit& operator()(unsigned int index)
    {
        return m_Bits[index];
    }

    const Bit& operator()(unsigned int index) const
    {
        return m_Bits[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const Bits& bits)
    {
        for (auto it = bits.m_Bits.rbegin(); it != bits.m_Bits.rend(); ++it)
        {
            os << *it;
        }

        return os;
    }

  private:
    std::array<Bit, Amount> m_Bits{0};
};
using Byte = Bits<8>;

inline Bits<2> HALFADDER(const Bit bit, const Bit bit2)
{
    Bit sum = XOR(bit, bit2);
    Bit carry = AND(bit, bit2);
    return {sum, carry};
}

inline Bits<2> FULLADDER(const Bit bit1, const Bit bit2, const Bit carry)
{
    Bits<2> halfAdded = HALFADDER(bit1, bit2);
    Bits<2> halfAdded2 = HALFADDER(halfAdded(0), carry);

    Bit sum = halfAdded2(0);
    Bit carry2 = OR(halfAdded(1), halfAdded2(1));
    return {sum, carry2};
}

inline Bits<3> FULLADDER2(const Bits<2> bits1, const Bits<2> bits2, const Bit carry)
{
    Bits<2> fullAdded1 = FULLADDER(bits1(0), bits2(0), carry);
    Bits<2> fullAdded2 = FULLADDER(bits1(1), bits2(1), fullAdded1(1));

    Bit sum1 = fullAdded1(0);
    Bit sum2 = fullAdded2(0);
    Bit carry2 = fullAdded2(1);

    return {sum1, sum2, carry2};
}

inline Bits<5> FULLADDER4(const Bits<4>& bits1, const Bits<4>& bits2, const Bit carry)
{
    Bits<3> full2Added1 = FULLADDER2({bits1(0), bits1(1)}, {bits2(0), bits2(1)}, carry);
    Bits<3> full2Added2 = FULLADDER2({bits1(2), bits1(3)}, {bits2(2), bits2(3)}, full2Added1(2));

    Bits<2> sum1 = {full2Added1(0), full2Added1(1)};
    Bits<2> sum2 = {full2Added2(0), full2Added2(1)};
    Bit carry2 = full2Added2(2);

    return {sum1(0), sum1(1), sum2(0), sum2(1), carry2};
}

struct ALU
{
    using Flag = Bit;
    Flag Zero = 1;
    Flag Overflow = 0;

    template <int Amount> Bits<Amount> Add(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bit carry = 0;
        Bits<Amount> result;
        Zero = 1;
        Overflow = 0;

        for (int i = 0; i < Amount; ++i)
        {
            Bits<2> fullAdded = FULLADDER(bits1(i), bits2(i), carry);
            Bit sum = fullAdded(0);
            result(i) = sum;
            carry = fullAdded(1);

            if (sum == 1)
            {
                Zero = 0;
            }
        }

        if (carry == 1)
        {
            Overflow = 1;
        }

        return result;
    }

    template <int Amount> Bits<Amount> And(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = AND(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Or(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = OR(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Xor(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = XOR(bits1(i), bits2(i));
        }
        return result;
    }
    template <int Amount> Bits<Amount> Not(const Bits<Amount>& bits)
    {
        Bits<Amount> result;

        for (int i = 0; i < Amount; ++i)
        {
            result(i) = NOT(bits(i));
        }
        return result;
    }
};

struct uInt8 : Bits<8>
{
    using Bits::Bits;
    uInt8(uint8_t uint8)
    {
        for (int i = 0; i < 8; ++i)
        {
            (*this)(i) = uint8 % 2;
            uint8 /= 2;
        }
    }
};

inline void Test()
{
    ALU alu;

    uInt8 c = {34};
    uInt8 d = {33};

    N::U::Log::Info(alu.Add(c, d));
    N::U::Log::Info(alu.Zero);
    N::U::Log::Info(alu.Overflow);
    // N::U::Log::Info(ADD(a, b));

    N::P::Dimension<float, N::P::Acceleration> g = 9.8f;
    N::P::Dimension<float, N::P::Force> FT = 122;
    // N::P::Dimension<float, N::P::Mass<1>> MT = FT / g;

    N::U::Log::Info((FT / g));
}
} // namespace Sketch