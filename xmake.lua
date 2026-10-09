set_languages("c++26")
set_toolchains("clang")

add_rules("plugin.compile_commands.autoupdate", { lsp = "clang" })

add_rules("mode.debug", "mode.release")
add_requires("catch2", "glfw", "assimp", "glslang", "stb", "glad 0.1.36", "nlohmann_json", "magic_enum")

target("Ivy")
    if is_mode("release") then
        set_policy("build.optimization.lto", true)
    end

    set_kind("binary")
    set_rundir(os.projectdir())
    add_files("src/**.cpp")
    add_includedirs("src")

    set_pcxxheader("src/pch.hpp")

    add_files("External/tracy/public/TracyClient.cpp")
    add_includedirs("External/tracy/public")
    add_defines("TRACY_ENABLE")

    on_load( function (target)

        local umbrella = import("scripts/GenerateUmbrellas")

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

    add_packages("glfw", "assimp", "glslang", "stb", "glad", "nlohmann_json", "magic_enum")


target("IvyTests")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("Tests/**.cpp")

    add_includedirs("src")
    set_pcxxheader("src/pch.hpp")

    add_packages("catch2")
