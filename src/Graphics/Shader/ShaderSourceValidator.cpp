#include "ShaderSourceValidator.hpp"
#include "ShaderSource.hpp"
#include <glslang/Public/ResourceLimits.h>

namespace N::G
{
EShLanguage ShaderSourceValidator::ToEShLanguage(const ShaderStage& stage)
{
    switch (stage)
    {
    case ShaderStage::Vertex:
        return EShLangVertex;

    case ShaderStage::Fragment:
        return EShLangFragment;

    case ShaderStage::Geometry:
        return EShLangGeometry;
    }

    return EShLangVertex;
}

ShaderValidationResult ShaderSourceValidator::Validate(const ShaderSource& source)
{
    const int version = std::stoi(source.Version.substr(8));
    const EShLanguage language = ToEShLanguage(source.GetStage());

    glslang::TShader shader(language);

    const char* sourceString = source.GeneratedCode.c_str();
    shader.setStrings(&sourceString, 1);

    const bool success = shader.parse(GetDefaultResources(), version, true, EShMsgDefault);

    ShaderValidationResult result{.Success = success, .Log = {}};

    if (!success)
    {
        result.Log = shader.getInfoLog();

        if (const char* debugLog = shader.getInfoDebugLog(); debugLog && *debugLog)
        {
            result.Log += '\n';
            result.Log += debugLog;
        }
    }

    return result;
}
} // namespace N::G
