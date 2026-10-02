#pragma once
#include "Math/Functions/Function.hpp"
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

using namespace N;

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

inline Bits<2> HalfAdder(const Bit bit, const Bit bit2)
{
    Bit sum = XOR(bit, bit2);
    Bit carry = AND(bit, bit2);
    return {sum, carry};
}

inline Bits<2> FullAdder(const Bit bit1, const Bit bit2, const Bit carry)
{
    Bits<2> halfAdded = HalfAdder(bit1, bit2);
    Bits<2> halfAdded2 = HalfAdder(halfAdded(0), carry);

    Bit sum = halfAdded2(0);
    Bit carry2 = OR(halfAdded(1), halfAdded2(1));
    return {sum, carry2};
}

struct ALU
{
    using Flag = Bit;
    Flag Zero = true;
    Flag Overflow = false;

    template <int Amount> Bits<Amount> Add(const Bits<Amount>& bits1, const Bits<Amount>& bits2)
    {
        Bit carry = false;
        Bits<Amount> result;
        Zero = true;
        Overflow = false;

        for (int i = 0; i < Amount; ++i)
        {
            Bits<2> fullAdded = FULLADDER(bits1(i), bits2(i), carry);
            Bit sum = fullAdded(0);
            result(i) = sum;
            carry = fullAdded(1);

            if (sum == 1)
            {
                Zero = false;
            }
        }

        if (carry == 1)
        {
            Overflow = true;
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
    constexpr uInt8(uint8_t value)
    {
        for (int i = 0; i < 8; ++i)
        {
            (*this)(i) = value % 2;
            value /= 2;
        }
    }
};

template <typename T> struct Set
{
    template <typename F>
    requires std::same_as<std::invoke_result_t<F, T>, bool> && (!std::same_as<std::remove_cvref_t<F>, Set>)
    Set(F&& predicate) : m_Predicate(std::forward<F>(predicate))
    {
    }

    bool Belongs(const T& x) const
    {
        return m_Predicate(x);
    }

    Set Intersection(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) && right.Belongs(x); }};
    }

    Set Union(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) || right.Belongs(x); }};
    }

    Set Difference(const Set& other) const
    {
        return Set{
            [left = *this, right = other](const T& x) { return left.Belongs(x) && !right.Belongs(x); }};
    }

    Set SymmetricDifference(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) != right.Belongs(x); }};
    }

    Set Complement() const
    {
        return Set{[set = *this](const T& x) { return !set.Belongs(x); }};
    }

    static Set Empty()
    {
        return Set{[](const T&) { return false; }};
    }

    static Set Universal()
    {
        return Set{[](const T&) { return true; }};
    }

  private:
    std::function<bool(const T&)> m_Predicate;
};

template <typename T> struct FiniteSet
{
    void Add(const T& value)
    {
        m_Elements.emplace(value);
    }

    void Remove(const T& value)
    {
        m_Elements.erase(value);
    }

    bool Belongs(const T& value) const
    {
        return m_Elements.contains(value);
    }

    FiniteSet Union(const FiniteSet& other) const
    {
        FiniteSet result = *this;
        for (const auto& elem : other.m_Elements)
        {
            result.Add(elem);
        }
        return result;
    }

    FiniteSet Intersection(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    FiniteSet Difference(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    FiniteSet SymmetricDifference(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        for (const auto& elem : other.m_Elements)
        {
            if (!m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    bool IsSubSetOf(const FiniteSet& other) const
    {
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                return false;
            }
        }
        return true;
    }

    bool IsSuperSetOf(const FiniteSet& other) const
    {
        return other.IsSubSetOf(*this);
    }

    bool operator==(const FiniteSet& other) const
    {
        if (m_Elements.size() == other.m_Elements.size())
        {
            for (const auto& elem : m_Elements)
            {
                if (!other.m_Elements.contains(elem))
                {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

  private:
    std::unordered_set<T> m_Elements{};
};

inline void Test()
{
    ALU alu;

    uInt8 c = {34};
    uInt8 d = {33};
    // N::U::Log::Info(alu.Add(c, d));
    // N::U::Log::Info(alu.Zero);
    // N::U::Log::Info(alu.Overflow);

    constexpr float theta = 90;
    P::Dimension<M::Vector<2>, P::Acceleration> g = M::Vector<2>{0, -9.8f};
    P::Dimension<M::Vector<2>, P::Acceleration> a = M::Vector<2>{9.8f * M::DSin(theta), 0};

    M::Matrix<2, 2> mat2 = M::Matrix<2, 2>::Identity();
    mat2 = mat2.Rotate(M::Rad(theta));
    U::Log::Info(a);
    U::Log::Info(mat2.Transpose() * a);

    M::Function<float, M::Vector<2>> f = [](const float x) { return M::Vector<2>{x, x * x}; };
    M::Function<float, float> p = [](const float x) { return x * x; };
    U::Log::Info((f + p)(5));
}
} // namespace Sketch