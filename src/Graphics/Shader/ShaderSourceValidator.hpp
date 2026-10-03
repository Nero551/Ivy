#pragma once

#include <glslang/Public/ShaderLang.h>
#include <string>

#include "ShaderSource.hpp"
#include "ShaderStage.hpp"

namespace N::G
{
/**
 * @brief Result produced by shader source validation.
 */
struct ShaderValidationResult
{
    /**
     * @brief Whether the shader passed validation.
     */
    bool Success;

    /**
     * @brief Validation diagnostics produced by glslang.
     *
     * This contains compiler errors and additional diagnostic information
     * when validation fails.
     */
    std::string Log;
};

/**
 * @brief Validates GLSL shader source using glslang.
 *
 * ShaderSourceValidator performs offline GLSL validation before the shader
 * is passed to the OpenGL driver for compilation. The validator uses the
 * shader stage and GLSL version provided by the ShaderSource.
 *
 * This validation is intended as an additional development-time check;
 * OpenGL shader compilation should still be performed because the graphics
 * driver remains the final authority for whether a shader can be compiled.
 */
struct ShaderSourceValidator
{
    /**
     * @brief Converts a shader stage to glslang's shader stage.
     *
     * @param stage The shader stage used by the engine.
     * @return The corresponding glslang shader language.
     */
    static EShLanguage ToEShLanguage(const ShaderStage& stage);

    /**
     * @brief Validates a shader source.
     *
     * The source should contain GLSL code.
     * The GLSL version and shader stage are obtained from the ShaderSource.
     *
     * @param source Shader source to validate.
     * @return A ShaderValidationResult containing the validation status
     *         and any diagnostics produced by glslang.
     */
    static ShaderValidationResult Validate(const ShaderSource& source);
};
} // namespace N::G
