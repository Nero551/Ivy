#pragma once
#include "Core/OuterCore/ECS/Component.hpp"
#include "Math/Common/Logarithms.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector4.hpp"
#include "Utilities/Log.hpp"

namespace Sketch
{
//TODO- the size of transform component is whats bottlenecking.
// split it. atleast split transform from the matrices (model matrix, normal matrix)

//TODO- add operator<< to all custom data structures. clean up datHa structure code.

//TODO- add vector operators. for vector.
//
//TODO- If converting a general Quaternion to a rotation quaternion proves
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion)
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO- remove matrices from transform component
// make render batches store sparse sets that map entity ids to normal/model matrices.
// and only recompute matrices when global transform.IsChanged.

//TODO- play minecraft in the redstone modpack i made for understanding logic gates.

//TODO- rework dimensional analysis system

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

inline std::string Superscript(int exponent)
{
    static constexpr std::string_view Digits[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
    if (exponent == 1)
    {
        return "";
    }

    std::string result;

    if (exponent < 0)
    {
        result += "⁻";
        exponent = -exponent;
    }

    std::string digits = std::to_string(exponent);

    for (char digit : digits)
    {
        result += Digits[digit - '0'];
    }

    return result;
};

struct IDimensional
{
};

struct IOperationDimensional : IDimensional
{
};

template <std::size_t N> struct FixedString
{
    char Data[N];

    constexpr FixedString(const char (&string)[N])
    {
        std::copy_n(string, N, Data);
    }

    constexpr operator std::string_view() const
    {
        return {Data, N - 1};
    }
};

template <template <int> typename Derived, int Exp> struct Dimensional : IDimensional
{
    static constexpr int Exponent = Exp;
    template <int E> using WithExponent = Derived<E>;
    using Normalized = Derived<Exp>;
};

//TODO- use specializations to make this less of a mess
template <typename A, typename B, FixedString Name = ""> struct OperationDimensional;

template <typename T>
concept IsOperation = std::derived_from<T, IOperationDimensional>;
template <typename T>
concept IsTerm = !IsOperation<T>;

template <typename Left, typename Right> struct OperationNormalization
{
    using Type = OperationDimensional<Left, Right>;
};

template <IsTerm LeftTerm, IsTerm RightTerm> struct OperationNormalization<LeftTerm, RightTerm>
{
    static constexpr bool Equal = std::same_as<typename LeftTerm::template WithExponent<1>,
        typename RightTerm::template WithExponent<1>>;

    using Type = std::conditional_t<Equal,
        typename LeftTerm::template WithExponent<LeftTerm::Exponent + RightTerm::Exponent>,
        OperationDimensional<LeftTerm, RightTerm>>;
};

template <IsOperation LeftOp, IsTerm RightTerm> struct OperationNormalization<LeftOp, RightTerm>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    using C1 = OperationDimensional<typename LeftOp::Left, AddTerms<typename LeftOp::Right, RightTerm>>;
    using C2 = OperationDimensional<AddTerms<typename LeftOp::Left, RightTerm>, typename LeftOp::Right>;
    using C3 = OperationDimensional<LeftOp, RightTerm>;

    using Type = std::conditional_t<Equal<typename LeftOp::Right, RightTerm>, C1,
        std::conditional_t<Equal<typename LeftOp::Left, RightTerm>, C2, C3>>;
};

template <IsTerm LeftTerm, IsOperation RightOp> struct OperationNormalization<LeftTerm, RightOp>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    using C1 = OperationDimensional<typename RightOp::Left, AddTerms<typename RightOp::Right, LeftTerm>>;
    using C2 = OperationDimensional<AddTerms<LeftTerm, typename RightOp::Left>, typename RightOp::Right>;
    using C3 = OperationDimensional<LeftTerm, RightOp>;

    using Type = std::conditional_t<Equal<typename RightOp::Right, LeftTerm>, C1,
        std::conditional_t<Equal<typename RightOp::Left, LeftTerm>, C2, C3>>;
};

template <IsOperation LeftOp, IsOperation RightOp> struct OperationNormalization<LeftOp, RightOp>
{
    template <typename C, typename D>
    static constexpr bool Equal =
        std::same_as<typename C::template WithExponent<1>, typename D::template WithExponent<1>>;
    template <typename C, typename D> using AddTerms = C::template WithExponent<C::Exponent + D::Exponent>;

    static constexpr bool E1 = Equal<typename LeftOp::Left, typename RightOp::Left> &&
        Equal<typename LeftOp::Right, typename RightOp::Right> &&
        !Equal<typename LeftOp::Left, typename LeftOp::Right>;

    static constexpr bool E2 = Equal<typename LeftOp::Left, typename RightOp::Right> &&
        Equal<typename LeftOp::Right, typename RightOp::Left> &&
        !Equal<typename LeftOp::Left, typename LeftOp::Right>;

    static constexpr bool E3 = Equal<typename LeftOp::Left, typename RightOp::Left>;
    static constexpr bool E4 = Equal<typename LeftOp::Right, typename RightOp::Right>;
    static constexpr bool E5 = Equal<typename LeftOp::Left, typename RightOp::Right>;
    static constexpr bool E6 = Equal<typename LeftOp::Right, typename RightOp::Left>;

    using C1 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Left>,
        AddTerms<typename LeftOp::Right, typename RightOp::Right>>;

    using C2 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Right>,
        AddTerms<typename LeftOp::Right, typename RightOp::Left>>;

    using C3 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Left>,
        OperationDimensional<typename LeftOp::Right, typename RightOp::Right>>;

    using C4 = OperationDimensional<OperationDimensional<typename LeftOp::Left, typename RightOp::Left>,
        AddTerms<typename LeftOp::Right, typename RightOp::Right>>;

    using C5 = OperationDimensional<AddTerms<typename LeftOp::Left, typename RightOp::Right>,
        OperationDimensional<typename LeftOp::Right, typename RightOp::Left>>;

    using C6 = OperationDimensional<OperationDimensional<typename LeftOp::Left, typename RightOp::Right>,
        AddTerms<typename LeftOp::Right, typename RightOp::Left>>;

    using C7 = OperationDimensional<LeftOp, RightOp>;

    using Type = std::conditional_t<E1, C1,
        std::conditional_t<E2, C2,
            std::conditional_t<E3, C3,
                std::conditional_t<E4, C4, std::conditional_t<E5, C5, std::conditional_t<E6, C6, C7>>>>>>;
};

template <typename A, typename B, FixedString Name> struct OperationDimensional : IOperationDimensional
{
    using Left = A::Normalized;
    using Right = B::Normalized;

    using Normalized = OperationNormalization<Left, Right>::Type;

    static std::ostream& Print(std::ostream& os)
    {
        if constexpr (Name.Data[0] != '\0')
        {
            os << std::string_view(Name);
            return os;
        }

        if (Normalized::Left::Exponent < 0 && Normalized::Right::Exponent < 0)
        {
            os << "1/(";
            Normalized::Left::template WithExponent<-Normalized::Left::Exponent>::Print(os);
            Normalized::Right::template WithExponent<-Normalized::Right::Exponent>::Print(os);
            os << ")";
        }

        if (Normalized::Left::Exponent < 0 && Normalized::Right::Exponent > 0)
        {
            Normalized::Right::Print(os);
            os << "/";
            Normalized::Left::template WithExponent<-Normalized::Left::Exponent>::Print(os);
        }

        if (Normalized::Left::Exponent > 0 && Normalized::Right::Exponent < 0)
        {
            Normalized::Left::Print(os);
            os << "/";
            Normalized::Right::template WithExponent<-Normalized::Right::Exponent>::Print(os);
        }

        if (Normalized::Left::Exponent > 0 && Normalized::Right::Exponent > 0)
        {
            Normalized::Left::Print(os);
            Normalized::Right::Print(os);
        }

        return os;
    }

    template <int E> using WithExponent = OperationDimensional;
    static constexpr int Exponent = 1;
};

template <int Exp> struct Time : Dimensional<Time, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "s" << Superscript(Exp);
    }
};
template <int Exp> struct Length : Dimensional<Length, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "m" << Superscript(Exp);
    }
};
template <int Exp> struct Mass : Dimensional<Mass, Exp>
{
    static std::ostream& Print(std::ostream& os)
    {
        return os << "kg" << Superscript(Exp);
    }
};

template <typename T, typename D> requires(std::derived_from<D, IDimensional>)
struct Dimension
{
    T Value{0};

    constexpr Dimension() {}
    constexpr Dimension(const T& value) : Value(value) {}

    template <typename O>
    constexpr Dimension operator+(const Dimension<T, O>& other)
        requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value + other.Value};
    }

    template <typename O>
    constexpr Dimension operator-(const Dimension<T, O>& other)
        requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value - other.Value};
    }

    template <typename O, int E>
    constexpr Dimension<T, typename D::template WithExponent<D::Exponent + E>> operator*(
        const Dimension<T, O>& other) requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value * other.Value};
    }

    template <typename O, int E>
    constexpr Dimension<T, typename D::template WithExponent<D::Exponent - E>> operator/(
        const Dimension<T, O>& other) requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value / other.Value};
    }

    template <typename O> constexpr Dimension<T, OperationDimensional<D, O>> operator*(Dimension<T, O>& other)
    {
        return {Value * other.Value};
    }

    template <typename O>
    constexpr Dimension<T, OperationDimensional<D, typename O::template WithExponent<-O::Exponent>>>
    operator/(Dimension<T, O>& other)
    {
        return {Value / other.Value};
    }

    constexpr T& operator()()
    {
        return Value;
    }
    constexpr const T& operator()() const
    {
        return Value;
    }

    friend std::ostream& operator<<(std::ostream& os, Dimension dimension)
    {
        os << dimension.Value << ' ';
        return D::Print(os);
    }
};

inline void Test()
{

    using Acceleration = OperationDimensional<OperationDimensional<Length<1>, Time<-1>>, Time<-1>>;
    using Newton = OperationDimensional<Mass<1>, Acceleration>;
    N::U::Log::Info(Dimension<float, Newton>{5});
}
} // namespace Sketch