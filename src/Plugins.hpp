#pragma once
#include "Core/Engine.hpp"
#include "Core/Plugin.hpp"
#include "Core/SystemManager.hpp"
#include "Sandbox/Sandbox2D.hpp"
#include "Sandbox/Sandbox3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields2D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/Renderer/Systems/LightingSystem.hpp"
#include "Systems/Renderer/Systems/RendererSystem.hpp"

namespace Ivy
{

struct Sandbox3DPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<Sandbox::VectorFields3DState, Sandbox::Controller3DState,
            Sandbox::StressTester3DState>();

        engine.SM.AddSystems<Ivy::C::Phase::Start, Sandbox::Sandbox3D::Start_Sandbox3D,
            Sandbox::VectorFields3D::Start_VectorFields3D>();

        engine.SM.AddSystems<Ivy::C::Phase::FixedUpdate, Sandbox::Sandbox3D::FixedUpdate_Sandbox3D>();

        engine.SM.AddSystems<Ivy::C::Phase::Update, Sandbox::Controller3D::Update_Controller3D>();

        engine.SM.Chain<Ivy::C::Phase::Start, Sandbox::VectorFields3D::Start_VectorFields3D,
            Sandbox::Sandbox3D::Start_Sandbox3D>();
    }
};

struct Sandbox2DPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<Sandbox::VectorFields2DState, Sandbox::Controller2DState,
            Sandbox::StressTester2DState>();

        engine.SM.AddSystems<Ivy::C::Phase::Start, Sandbox::Sandbox2D::Start_Sandbox2D,
            Sandbox::VectorFields2D::Start_VectorFields2D>();

        engine.SM.AddSystems<Ivy::C::Phase::FixedUpdate, Sandbox::Sandbox2D::FixedUpdate_Sandbox2D>();

        engine.SM.AddSystems<Ivy::C::Phase::Update, Sandbox::Controller2D::Update_Controller2D>();

        engine.SM.Chain<Ivy::C::Phase::Start, Sandbox::VectorFields2D::Start_VectorFields2D,
            Sandbox::Sandbox2D::Start_Sandbox2D>();
    }
};

struct RendererPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<RendererState, LightingSystemState>();

        engine.SM.AddSystems<C::Phase::Start, Start_Renderer, Start_Lighting>();
        engine.SM.AddSystems<Ivy::C::Phase::BeginFrame, BeginFrame_Renderer>();
        engine.SM.AddSystems<Ivy::C::Phase::Render, Render_Renderer, Render_Lighting>();
        engine.SM.AddSystems<Ivy::C::Phase::Stop, Stop_Renderer>();

        engine.SM.Chain<Ivy::C::Phase::Render, Render_Lighting, Render_Renderer>();
    }
};
} // namespace Ivy
