#pragma once
#include <type_traits>
namespace Protos::MultiGpuBackend
{
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
} // namespace Protos::MultiGpuBackend
