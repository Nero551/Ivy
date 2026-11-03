#pragma once

#include "Core/ECS/Component.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Math/Common/Constants.hpp"
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
#include "Math/Functions/Inverse.hpp"
#include "Math/Math.hpp"
#include "Math/MathTuple.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vector.hpp"
#include "Physics/DimensionalAnalysis/Dimension.hpp"
#include "Physics/DimensionalAnalysis/Dimensional.hpp"
#include "Physics/DimensionalAnalysis/Dimensionals.hpp"
#include "Physics/Units.hpp"
#include "Protos/ECS.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Inspector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/IsSpecialization.hpp"
#include "Utilities/Track.hpp"
#include "tracy/Tracy.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <csignal>
#include <cstdlib>
#include <dlfcn.h>
#include <functional>
#include <ios>
#include <memory>
#include <ostream>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

namespace Sketch
{
//TODO: If converting a general Quaternion to a rotation quaternion proves \
// expensive in a hot path, introduce a specialized RotQuaternion (RQuaternion) \
// type and explicit conversion between the two. it will just be a unit quaternion with half angle representation.

//TODO: rethink the render pipeline (look at bevy's render graph. bevy is pretty nice)

//TODO: make transform system cache its own vector of the root's children that's updated on events \
// that will make it way less expensive performance-wise.
//TODO: not running renderer module sends fps from 15 to 34 (on 1.7m entities). \
//not running renderer + transform system sends fps from 34 to 6500 (on 1.7m entities). \
// its the ForEachDescendant thing.
//
// TODO: use a GetSmallestPool in ComponentPoolQuery instead of using the first pool as the driver.
//
// TODO: need particle system (cpu particle & gpu particle(compute shaders))
// TODO: first thing i should do after physics is compute shaders and SSBO, actually finish LearnOpenGL.

//TODO: all this time i been thinking how to make cpu side easier to use and more optimized & automatic, even for graphics. \
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

struct A : Ivy::C::Component
{
    int Number = 10;
};
struct B : Ivy::C::Component
{
    int Number = 10;
};
struct C : Ivy::C::Component
{
    int Number = 10;
};

inline void BenchECS()
{
    Ivy::C::EntityComponentSystem ecs;
    auto& aPool = ecs.Query.Pool<A>();
    auto& bPool = ecs.Query.Pool<B>();
    auto& cPool = ecs.Query.Pool<C>();
    unsigned int entities = 1000000;

    for (unsigned int i = 0; i < entities; ++i)
    {
        ZoneScopedN("Creation");
        Ivy::C::Entity& e = ecs.CreateEntity<Ivy::C::Entity>();
        aPool.Add(e);
        bPool.Add(e);
        cPool.Add(e);
    }

    double elapsed = 0;
    while (true)
    {
        if (elapsed >= 10)
        {
            ZoneScopedN("Iteration");
            elapsed = 0;
            ecs.Query.ForEach<A, B, C>(
                [](const unsigned int id, A& a, B& b, C& c)
                {
                    a.Number += b.Number + c.Number;
                    b.Number += a.Number + c.Number;
                    c.Number += a.Number + b.Number;
                });
        }
        elapsed += 0.016;
    }
}

//TODO: define VariableCount method to math objects (vector,quaternion,matrix,complex,math tuple,etc)
// TODO: define VariableCount trait. 1 for floats,double,int,etc, then each math type specifies its number of variables. \
// this will be needed for partial derivatives specifically.
//
//TODO: concepts i might need: IsQuaternion, IsVector, IsComplex, IsMatrix, IsODE, IsFunction, IsMathTuple, IsLinearEq, IsLinearSys etc \
// define them using the IsSpec family in Utilities/IsSpecialization.hpp
//
// TODO: prevent math tuples from being `input-able to Function.
//
// TODO: if i really want to optimize my ECS. i gotta take it out of the project, and optimize it on its own. so take ECS folder + Event.hpp
//
// TODO: wrap all used std stuff in stl wrapper. basically to avoid backward compatibility bs in std. \
// and also as a declaration and limitation that these are all the std stuff used in the codebase.
// TODO: setup custom clang-tidy module/ check (IvyTidy)
//
//
// TODO: make ArchetypeAoS & ArchetypeSoA data structures
// TODO: instead of checking

struct D : ECS::Component
{
    int Number = 10;
};
struct E : ECS::Component
{
    int Number = 10;
};
struct F : ECS::Component
{
    int Number = 10;
};

struct G : ECS::Component
{
    int Number = 10;
};

void BenchArchetypeECS()
{
    ECS::Query q;
    unsigned int entities = 10000000;

    for (unsigned int i = 0; i < entities; ++i)
    {
        if (i % 2 == 0)
        {
            q.AddComponent<D, E, G>({
                .Id = {i, 0}
            });
        }
        else
        {
            q.AddComponent<D, F, G>({
                .Id = {i, 0}
            });
        }
        ZoneScopedN("Creation");
    }

    double elapsed = 0;
    while (true)
    {
        if (elapsed >= 10)
        {
            ZoneScopedN("Iteration");
            elapsed = 0;
            q.ForEach<D, G>([](unsigned int id, D& d, G& g) { d.Number += id; });
        }
        elapsed += 0.016;
    }
}

//TODO: next step is figuring out entity creation, component adding, removing, and syncing the cache.
// TODO: gonna need to make Archetype dynamic. meaning no templates :( so i can dynamically move entities between archetypes. \
// meaning i have to somehow create a dynamic storage that doesn't use tuples. or has any info about type on compile time. \
// gotta pass some sort of signature containing the type at runtime.

inline void Test()
{
    BenchArchetypeECS();
}
} // namespace Sketch
