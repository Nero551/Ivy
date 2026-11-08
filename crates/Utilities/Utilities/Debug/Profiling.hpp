#pragma once

#ifdef DEBUG
#include <tracy/Tracy.hpp>
#define TRACY_OPENGL_AUTO_CALIBRATION
#include "tracy/TracyOpenGL.hpp"

#define IVY_PROFILE_ZONE ZoneScoped
#define IVY_PROFILE_GPU_ZONE TracyGpuZone
#define IVY_PROFILE_FRAME_MARK FrameMark
#define IVY_PROFILE_GPU_CONTEXT TracyGpuContext
#define IVY_PROFILE_GPU_COLLECT TracyGpuCollect

#else
#define IVY_PROFILE_ZONE
#define IVY_PROFILE_FRAME
#define IVY_PROFILE_GPU_ZONE
#define IVY_PROFILE_FRAME_MARK
#define IVY_PROFILE_GPU_CONTEXT
#define IVY_PROFILE_GPU_COLLECT
#endif
