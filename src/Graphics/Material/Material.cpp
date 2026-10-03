#include "Material.hpp"

#include "../../Modules/Renderer/Primitives/Primitives.hpp"
#include "../Shader/Uniforms/FloatUniform.hpp"
#include "../Shader/Uniforms/IntUniform.hpp"
#include "../Shader/Uniforms/Vector3Uniform.hpp"
#include "../Shader/Uniforms/Vector4Uniform.hpp"
#include "../Texture/Texture2D.hpp"
#include "Utilities/Log.hpp"

namespace N::G
{
Material::Material(const std::string& name) : m_Name(name)
{
    auto& whiteTexture = R::Primitives::CreateWhiteTexture();

    DiffuseMap = &whiteTexture();
    SpecularMap = &whiteTexture();
    EmissionMap = &whiteTexture();
}

const std::string& Material::GetName() const
{
    return m_Name;
}

void Material::AssignTexture(Texture& texture, const unsigned int slot)
{
    if (slot >= MaxCustomTextures)
    {
        U::Log::Error("Material: ", GetName(), " Texture slot: ", slot, " out of bounds: " + texture.GetId());
        return;
    }
    m_CustomTextures[slot] = &texture;
}

void Material::Use()
{
    SetProperties();

    for (int slot = 0; slot < MaxCustomTextures; ++slot)
    {
        if (m_CustomTextures[slot])
        {
            Shader->SetUniform(IntUniform(m_CustomTextures[slot]->GetName(), slot));
            m_CustomTextures[slot]->Bind(slot);
        }
    }

    Depth.Apply();
    Stencil.Apply();
    Blend.Apply();

    Shader->Use();
}

void Material::SetProperties() const
{
    Shader->SetUniform(Vector3Uniform("MATERIAL.Ambient", Ambient));
    Shader->SetUniform(Vector3Uniform("MATERIAL.Diffuse", Diffuse));
    Shader->SetUniform(Vector3Uniform("MATERIAL.Specular", Specular));
    Shader->SetUniform(Vector3Uniform("MATERIAL.Emission", Emission));
    Shader->SetUniform(FloatUniform("MATERIAL.Shininess", Shininess));
    Shader->SetUniform(Vector4Uniform("MATERIAL.Color", Color));

    Shader->SetUniform(IntUniform("MATERIAL.DiffuseMap", 16));
    DiffuseMap->Bind(16);

    Shader->SetUniform(IntUniform("MATERIAL.SpecularMap", 15));
    SpecularMap->Bind(15);

    Shader->SetUniform(IntUniform("MATERIAL.EmissionMap", 14));
    EmissionMap->Bind(14);
}
} // namespace N::G