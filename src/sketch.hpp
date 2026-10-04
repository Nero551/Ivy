#pragma once
#include "Math/Functions/Function.hpp"
#include "Protos/LogicPrototype.hpp"
#include "Utilities/Log.hpp"

namespace Sketch
{
//TODO- If converting a general Quaternion to a rotation quaternion proves
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion)
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO- play minecraft in the redstone modpack i made for understanding logic gates.

//TODO- redo Entity completely, make it use handles, add ability to search by entity object not just id/handle.
//
//TODO- try to make dependency injection to avoid global accessors like Engine::Get().
// try to decouple as much as possible before attempting to implement DI(dependency injection)

//TODO- probably wanna rethink my entire architecture (Core + Modules + World).
// the other stuff can be extracted out of this project and still work.
// so they dont count as "part of the architecture".

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

template <int Variables, M::Scalar T = float> struct LinearEquation
{

    template <typename... Args>
    constexpr LinearEquation(const T& constant, Args... coefficients)
        requires(sizeof...(Args) == Variables && (std::convertible_to<Args, T> && ...))
        : m_Coefficients{static_cast<T>(coefficients)...}, Constant(constant)
    {
    }

    constexpr const T& operator()(unsigned int index) const
    {
        return m_Coefficients[index];
    }

    constexpr T& operator()(unsigned int index)
    {
        return m_Coefficients[index];
    }

    T Constant;

    constexpr const std::array<T, Variables>& Data() const
    {
        return m_Coefficients;
    }

  private:
    std::array<T, Variables> m_Coefficients;
};

template <int Variables, M::Scalar T = float> struct LinearSystem
{
    template <typename... Args>
    constexpr LinearSystem(Args... equations)
        requires(sizeof...(Args) == Variables && (std::same_as<Args, LinearEquation<Variables, T>> && ...))
        : m_Equations{equations...}
    {
    }

    constexpr M::Vector<Variables> Solve() const
    {
        M::Matrix<Variables, Variables> A;
        M::Vector<Variables> b;

        for (int i = 0; i < Variables; ++i)
        {
            b(i) = m_Equations[i].Constant;
            for (int j = 0; j < Variables; ++j)
            {
                A(i, j) = m_Equations[i](j);
            }
        }

        return A.Inverse() * b;
    }

    constexpr LinearEquation<Variables, T>& operator()(unsigned int index)
    {
        return m_Equations[index];
    }

    constexpr const LinearEquation<Variables, T>& operator()(unsigned int index) const
    {
        return m_Equations[index];
    }

    constexpr const std::array<LinearEquation<Variables, T>, Variables>& Data() const
    {
        return m_Equations;
    }

  private:
    std::array<LinearEquation<Variables, T>, Variables> m_Equations;
};

inline void Test()
{
    ALU alu;

    uInt8 c = {34};
    uInt8 d = {33};
    // Ivy::U::Log::Info(alu.Add(c, d));
    // Ivy::U::Log::Info(alu.Zero);
    // Ivy::U::Log::Info(alu.Overflow);

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

    LinearEquation<2> eq1{3 * 9.81, -3, 1};
    LinearEquation<2> eq2{7 * 9.81, 7, 1};

    LinearSystem<2> sys{eq1, eq2};

    U::Log::Info(sys.Solve());
}
} // namespace Sketch