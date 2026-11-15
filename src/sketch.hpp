#pragma once

#include "Diagnostics/Log.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Scalar.hpp"
#include "Math/Set.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>
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

// enum class Execution : unsigned int
// {
//     Start,
//     BeginFrame,
//     FixedUpdate,
//     Update,
//     Render,
//     EndFrame,
//     Stop,
//     Count,
// };

// struct Sys
// {
//     virtual void Upd() {}
// };

// struct Eng
// {

//     /** @brief Returns a registered system by type. */
//     template <typename T> T& GetSystems()
//     {
//         auto system = m_Systems.Find<T>();
//         if (system == m_Systems.end())
//         {
//             D::Log::Error(std::format("System {} not found", typeid(T).name()));
//         }
//         return static_cast<T&>(**system);
//     }

//     const U::TypedVector<std::unique_ptr<Sys>>& GetAllSystems() const
//     {
//         return m_Systems;
//     }

//     /** @brief Creates and registers a system. */
//     template <typename T> T& AddSystems()
//     {
//         m_Systems.Emplace<T>(std::make_unique<T>());
//         // for (int i = 0; i < static_cast<unsigned int>(Execution::Count); ++i)
//         // {
//         //     Add<T>(static_cast<Execution>(i));
//         // }
//         return static_cast<T&>(*m_Systems.Get<T>());
//     }

//     template <typename... Args> std::tuple<Args&...> AddSystems() requires(sizeof...(Args) != 1)
//     {
//         return {AddSystems<Args>()...};
//     }

//     template <typename Tuple> decltype(auto) AddSystems() requires(U::IsTSpec<Tuple, std::tuple>)
//     {
//         return U::UnpackAndRun<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{},
//             [this]<typename... Args>() { return AddSystems<Args...>(); });
//     }

//     using ExecutionOrder = std::vector<unsigned int>;

//     std::array<ExecutionOrder, static_cast<unsigned int>(Execution::Count)> ExecutionScheduler;

//     template <typename... Args> void Add(Execution execution)
//     {
//         (ExecutionScheduler[static_cast<unsigned int>(execution)].push_back(m_Systems.GetIndex<Args>()), ...);
//     }

//     /** @brief Registered engine systems. */
//     U::TypedVector<std::unique_ptr<Sys>> m_Systems;
// };

// struct SysA : Sys
// {
//     void Upd() override
//     {
//         D::Log::Info("A");
//     }
// };
// struct SysB : Sys
// {
//     void Upd() override
//     {
//         D::Log::Info("B");
//     }
// };

template <std::unsigned_integral T> struct DAG
{
    static constexpr T Invalid = std::numeric_limits<T>::max();
    struct Node
    {
        std::vector<T> Predecessors;
        std::vector<T> Successors;
    };

    void Ensure(T index)
    {
        if (index == Invalid)
        {
            return;
        }

        std::size_t oldSize = m_Nodes.size();
        std::size_t newSize = static_cast<std::size_t>(index) + 1;

        if (newSize <= oldSize)
        {
            return;
        }

        m_Nodes.resize(newSize);

        for (std::size_t i = oldSize; i < newSize; ++i)
        {
            T node = static_cast<T>(i);
            m_Sources.Push(node, node);
        }
    }

    bool Exists(T node)
    {
        return node < m_Nodes.size();
    }

    void Attach(T pred, T suc)
    {
        if (!Exists(pred) || !Exists(suc))
        {
            return;
        }
        m_Nodes[pred].Successors.push_back(suc);
        m_Nodes[suc].Predecessors.push_back(pred);
        m_Sources.Erase(suc);
    }

    bool Detach(T pred, T suc)
    {
        if (!Exists(pred) || !Exists(suc))
        {
            return false;
        }

        Node& predNode = m_Nodes[pred];
        Node& sucNode = m_Nodes[suc];

        auto edge = std::ranges::find(predNode.Successors, suc);

        if (edge == predNode.Successors.end())
        {
            return false;
        }

        std::erase(predNode.Successors, suc);
        std::erase(sucNode.Predecessors, pred);

        if (sucNode.Predecessors.empty())
        {
            m_Sources.Push(suc, suc);
        }

        return true;
    }

    std::vector<Node> m_Nodes;
    U::SparseSetSoA<T> m_Sources;
};

inline void Test()
{

    // TODO: some sort of ordering mechanic for systems(which system runs first on each part of the loop). \
    // system execution order. after that, figure out how to parallelize them via my new MT prototype. \
    // if i can do that. i will be able to run each system(or set of systems) on a different thread. more speed.
    //
    // TODO: idea, all systems run on different threads, but option to run some systems together sequentially on 1 thread.
    //
    //
    //TODO: download and setup bevy, i would like to know how they do things.
    //
    // TODO: i need a way to describe system dependencies, via graphs or chains or smth. \
    //  directed acyclic graphs
    //

    // Eng eng{};
    // eng.AddSystems<SysA>();
    // eng.AddSystems<SysB>();
    // eng.Add<SysB, SysA>(Execution::Update);

    // for (unsigned int index : eng.ExecutionScheduler[static_cast<unsigned int>(Execution::Update)])
    // {
    //     eng.m_Systems[index]->Upd();
    // }
}
} // namespace Sketch
