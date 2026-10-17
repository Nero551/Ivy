#include "Material.hpp"

#include <string>
#include <utility>

#include "../../Utilities/Debug/Log.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Texture/Texture.hpp"

namespace Ivy::G
{

const std::string& Material::GetName() const
{
    return m_Name;
}

void Material::AssignTexture(Texture& texture, const unsigned int slot)
{
    if (slot >= MaxCustomTextures)
    {
        U::Log::Error("Material: ", GetName(), " Texture slot: ", slot, " out of bounds: ", texture.GetId());
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
            Shader->SetUniform(m_CustomTextures[slot]->GetName(), slot);
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
    Shader->SetUniform("MATERIAL.Ambient", Ambient);
    Shader->SetUniform("MATERIAL.Diffuse", Diffuse);
    Shader->SetUniform("MATERIAL.Specular", Specular);
    Shader->SetUniform("MATERIAL.Emission", Emission);
    Shader->SetUniform("MATERIAL.Shininess", Shininess);
    Shader->SetUniform("MATERIAL.Color", Color);

    // Shader->SetUniform(IntUniform("MATERIAL.DiffuseMap", 16));
    // DiffuseMap->Bind(16);

    // Shader->SetUniform(IntUniform("MATERIAL.SpecularMap", 15));
    // SpecularMap->Bind(15);

    // Shader->SetUniform(IntUniform("MATERIAL.EmissionMap", 14));
    // EmissionMap->Bind(14);
}
} // namespace Ivy::G
