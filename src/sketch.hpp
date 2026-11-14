#pragma once

#include "Diagnostics/Log.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Scalar.hpp"
#include "Math/Set.hpp"
#include "Protos/MultiThreading.hpp"
#include "Utilities/stl.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Sketch
{
//TODO: If converting a general Quaternion to a rotation quaternion proves \
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion) \
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO: rethink the render pipeline (look at bevy's render graph. bevy is pretty nice)
// TODO: need particle system (cpu particle & gpu particle(compute shaders))
// TODO: first thing i should do after physics is compute shaders and SSBO, actually finish LearnOpenGL.

//TODO: all this time i been thinking how to make cpu side more automatic for graphics. \
//i should be thinking about how to make cpu -> gpu pipeline easier to MODIFY and use. not make gpu/shaders as automatic as possible.

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
            D::Log::Error("Array: Out of bounds.");
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

enum class GPUBackend
{
    OpenGL,
    Vulkan,
    DX12,
};

struct OpenGLBuffer
{
    int GL;
};
struct VulkanBuffer
{
    int Vulk;
};

struct NoBEImpl
{
    int NO_BACKEND_FOR_THIS;
};

template <GPUBackend BE, typename GL = NoBEImpl, typename VK = NoBEImpl, typename DX12 = NoBEImpl>
using SelectBE = std::conditional_t<BE == GPUBackend::OpenGL, GL,
    std::conditional_t<BE == GPUBackend::Vulkan, VK,
        std::conditional_t<BE == GPUBackend::DX12, DX12, NoBEImpl>>>;

static constexpr GPUBackend Backend = GPUBackend::OpenGL;

struct Buffer : SelectBE<Backend, OpenGLBuffer, VulkanBuffer>
{
};

// TODO: another idea could be to have free functions for each of openGL and vulkan, etc. structs (buffer) just selects based on backend. \
// and have my own custom api layer than unifies openGL and vulkan under custom api interface, convert vulkan and openGL to this api.

//TODO: if i can't bind all gpu APIs under 1 interface, i could do a mix of both ideas. \
// have 2 modes , forward+ and compatibility, where forward+ uses the implementation for vk or dx12. \
// and compatibility uses gl or dx11/10. just like godot.

void DoVulkan(unsigned int id) {}
void DoOpenGL(unsigned int id) {}

struct Shader
{
    unsigned int id = 0;
    Shader()
    {
        switch (Backend)
        {
        case GPUBackend::OpenGL:
            DoOpenGL(id);
            break;
        case GPUBackend::Vulkan:
            DoVulkan(id);
            break;
        case GPUBackend::DX12:
            break;
        }
    }
};

//TODO: define VariableCount method to math objects (vector,quaternion,matrix,complex,math tuple,etc)
// TODO: define VariableCount trait. 1 for floats,double,int,etc, then each math type specifies its number of variables. \
// this will be needed for partial derivatives specifically.
//
//TODO: concepts i might need: IsQuaternion, IsVector, IsComplex, IsMatrix, IsODE, IsFunction, IsMathTuple, IsLinearEq, IsLinearSys etc \
// define them using the IsSpec family in Utilities/IsSpecialization.hpp
//
// TODO: prevent math tuples from being `input-able to Function.
//
// TODO: setup custom clang-tidy module/ check (IvyTidy)
//TODO: new performance bottleneck: renderer.

//TODO: Utilities crate needs a nice cleaning. especially data structures. they are packed with all kinds of unnecessary edge case handling.
//TODO: i need to formally define convergence and divergence & limits (numerically ofc).
//
// TODO: remove window from graphics and make a new crate (Platform Handling / Window Management, whatever). \
// it will handle input from mouse, keyboard, joystick, wi        ndow management, basically what glfw does + some custom stuff (action mapper)
//
// TODO: automatic differentiation via dual numbers.
//
// TODO: tracy is not working, the macro which defines it when debug mode doesn't work.

static M::Set<unsigned int> N = [](unsigned int x) { return true; };
static M::Set<int> Z = [](int x) { return true; };
static M::Set<float> R = [](float x) { return true; };

inline void Test()
{
    using F = M::Function<float, float>;
    F f = [](float x) { return M::PI / 4.0f * (M::Pow(x, 3) / 3.0f - 4 * M::Pow(x, 2) + 16 * x); };

    auto start = std::chrono::high_resolution_clock::now();

    int s = 0;
    int e = 2000;
    int counter = 0;
    std::condition_variable cv;
    std::mutex mutex;

    using namespace Protos;

    Parallel::Initialize(8);

    stl::function<void(int, int)> op = [&mutex, &counter](int s, int e)
    {
        for (int i = s; i < e; ++i)
        {
            std::scoped_lock<std::mutex> lock{mutex};
            ++counter;
        }
    };

    while (true)
    {
        // Parallel::Pool.Assign(Parallel::Task{op, s, e});
        // Parallel::Pool.Assign(Parallel::Task{op, s, e});
        // Parallel::Pool.Assign(Parallel::Task{op, s, e});
        // Parallel::Pool.Assign(Parallel::Task{op, s, e});

        Parallel::ParallelFor(s, e, [](int j) { j++; });
    }

    Parallel::Pool.Wait();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    D::Log::Info(duration.count() / 1000000.0f);
    D::Log::Info(counter);
}
} // namespace Sketch
