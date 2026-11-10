#pragma once

#ifdef DEBUG

#include <tracy/Tracy.hpp>

#define TRACY_OPENGL_AUTO_CALIBRATION
#include <glad/glad.h>
#include <tracy/TracyOpenGL.hpp>

#define IVY_PROFILE_ZONE(Name) ZoneScopedN(Name)
#define IVY_PROFILE_GPU_ZONE(Name) TracyGpuZone(Name)
#define IVY_PROFILE_FRAME_MARK FrameMark
#define IVY_PROFILE_GPU_CONTEXT TracyGpuContext
#define IVY_PROFILE_GPU_COLLECT TracyGpuCollect
#else

#define IVY_PROFILE_ZONE(Name) ((void)0)
#define IVY_PROFILE_GPU_ZONE(Name) ((void)0)
#define IVY_PROFILE_FRAME_MARK ((void)0)
#define IVY_PROFILE_GPU_CONTEXT ((void)0)
#define IVY_PROFILE_GPU_COLLECT ((void)0)

#endif
