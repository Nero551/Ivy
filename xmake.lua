set_languages("c++26")
set_toolchains("clang")

add_rules("plugin.compile_commands.autoupdate", { lsp = "clang" })
add_rules("mode.debug", "mode.release")

add_requires("tracy v0.13.1", { system = false })
add_requires("catch2", { system = false })
add_requires("glfw", { system = false })
add_requires("assimp", { system = false })
add_requires("glslang", { system = false })
add_requires("stb", { system = false })
add_requires("glad 0.1.36", { system = false })

target("Ivy")
    set_kind("binary")
    set_rundir(os.projectdir())
    add_includedirs("src")
    set_pcxxheader("src/pch.hpp")

    if is_mode("release") then
        add_files("src/**.cpp|src/TracyMemory.cpp")
        set_policy("build.optimization.lto", true)
    else
        add_defines("TRACY_ENABLE")
        add_defines("TRACY_PROFILE_MEMORY")
        add_files("src/**.cpp")
    end

    if is_mode("debug") then
        add_links("TracyClient")
        add_defines("TRACY_ENABLE")
        add_defines("TRACY_PROFILE_MEMORY")
    end

    on_load( function (target)

        local umbrella = import("scripts/generate-umbrellas")

        umbrella.GenerateUmbrellaHeader("Core", "Core")
        umbrella.GenerateUmbrellaHeader("World", "World")
        umbrella.GenerateUmbrellaHeader("Modules/Renderer", "Header")
        umbrella.GenerateUmbrellaHeader("Modules/Input", "Header")

        umbrella.GenerateUmbrellaHeader("Graphics", "Graphics")
        umbrella.GenerateUmbrellaHeader("Math", "Math")
        umbrella.GenerateUmbrellaHeader("Physics", "Physics")
        umbrella.GenerateUmbrellaHeader("Utilities", "Utilities")
        umbrella.GenerateEngineUmbrella("Ivy")
    end)

    add_links("glslang-default-resource-limits")
    add_packages("tracy", "glfw", "assimp", "glslang", "stb", "glad")


target("IvyTests")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("Tests/**.cpp")

    add_includedirs("src")
    set_pcxxheader("src/pch.hpp")

    add_packages("catch2")
