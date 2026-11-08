#pragma once
#include "OpenGL.hpp"

namespace Ivy::G
{
/**
 * @brief Specifies the data type of texture pixel components.
 *
 * Determines how the individual components of source pixel data
 * are represented in memory when transferred to OpenGL.
 */
enum class DataType
{
    /** Unsigned 8-bit integer. */
    UnsignedByte = GL_UNSIGNED_BYTE,

    /** Signed 8-bit integer. */
    Byte = GL_BYTE,

    /** Unsigned 16-bit integer. */
    UnsignedShort = GL_UNSIGNED_SHORT,

    /** Signed 16-bit integer. */
    Short = GL_SHORT,

    /** Unsigned 32-bit integer. */
    UnsignedInt = GL_UNSIGNED_INT,

    /** Signed 32-bit integer. */
    Int = GL_INT,

    /** 16-bit floating-point value. */
    HalfFloat = GL_HALF_FLOAT,

    /** 32-bit floating-point value. */
    Float = GL_FLOAT,

    /** Packed unsigned 16-bit RGB/RGBA data. */
    UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,

    /** Packed unsigned 16-bit RGBA data. */
    UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,

    /** Packed unsigned 16-bit RGBA data. */
    UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,

    /** Packed unsigned 32-bit depth/stencil data. */
    UnsignedInt248 = GL_UNSIGNED_INT_24_8,

    /** Packed unsigned 32-bit depth/stencil data. */
    Float32UnsignedInt248Rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
};

constexpr unsigned int GetBytesPerComponent(const DataType type)
{
    switch (type)
    {
    case DataType::UnsignedByte:
    case DataType::Byte:
        return 1;

    case DataType::UnsignedShort:
    case DataType::Short:
    case DataType::HalfFloat:
        return 2;

    case DataType::UnsignedInt:
    case DataType::Int:
    case DataType::Float:
    case DataType::UnsignedInt248:
        return 4;

    case DataType::UnsignedShort565:
    case DataType::UnsignedShort4444:
    case DataType::UnsignedShort5551:
        return 2;

    case DataType::Float32UnsignedInt248Rev:
        return 8;
    }

    return 0;
}
} // namespace Ivy::G
