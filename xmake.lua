set_languages("c++26")
set_toolchains("clang")

add_rules("plugin.compile_commands.autoupdate", { lsp = "clang" })
add_rules("mode.debug", "mode.release")

add_requires("tracy v0.13.1", "glfw 3.4", "assimp v6.0.5", "glad 0.1.36", "stb", "gtest", "benchmark 1.9.5")
add_requireconfs("**", { override = true, configs = { shared = false, system = false } })

target("IvyUtilities")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    add_files("src/Utilities/**.cpp")
    add_packages("stb", { public = true })
    on_load( function (target)
        local umbrella = import("scripts/generate-umbrellas")
        umbrella.GenerateUmbrellaHeader("Utilities", "Include")
    end)

target("IvyMath")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    add_files("src/Math/**.cpp")
    add_deps("IvyUtilities")
    on_load( function (target)
        local umbrella = import("scripts/generate-umbrellas")
        umbrella.GenerateUmbrellaHeader("Math", "Include")
    end)

target("IvyGraphics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    add_files("src/Graphics/**.cpp")
    add_deps("IvyUtilities", "IvyMath")
    add_packages("glad", { public = true })
    add_packages("glfw", { public = true })
    on_load( function (target)
        local umbrella = import("scripts/generate-umbrellas")
        umbrella.GenerateUmbrellaHeader("Graphics", "Include")
    end)

target("IvyPhysics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    add_files("src/Physics/**.cpp")
    add_deps("IvyUtilities", "IvyMath")
    on_load( function (target)
        local umbrella = import("scripts/generate-umbrellas")
        umbrella.GenerateUmbrellaHeader("Physics", "Include")
    end)

target("IvyCore")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    add_files("src/Core/**.cpp")
    add_deps("IvyUtilities", "IvyGraphics")
    add_packages("tracy", { public = true })
    on_load( function (target)
        local umbrella = import("scripts/generate-umbrellas")
        umbrella.GenerateUmbrellaHeader("Core", "Include")
    end)

target("Ivy")
    set_kind("binary")
    set_rundir(os.projectdir())
    add_includedirs("src", { public = true })
    set_pcxxheader("src/pch.hpp")
    add_files("src/**.cpp")

    remove_files("src/Core/**.cpp")
    remove_files("src/Physics/**.cpp")
    remove_files("src/Utilities/**.cpp")
    remove_files("src/Math/**.cpp")
    remove_files("src/Graphics/**.cpp")

    add_deps("IvyUtilities", "IvyMath", "IvyPhysics", "IvyGraphics", "IvyCore")

    if is_plat("linux") then
        add_ldflags("-static-libstdc++", "-static-libgcc", { force = true })
    elseif is_plat("windows") then
        set_runtimes("MT")
    end

    if is_mode("release") then
        remove_files("src/TracyMemory.cpp")
        set_policy("build.optimization.lto", true)
    else
        add_links("TracyClient")
        add_defines("TRACY_ENABLE")
        add_defines("TRACY_PROFILE_MEMORY")
    end

    on_load( function (target)

        local umbrella = import("scripts/generate-umbrellas")

        umbrella.GenerateUmbrellaHeader("Modules/Renderer", "Include")
        umbrella.GenerateUmbrellaHeader("Modules/Input", "Include")

        umbrella.GenerateEngineUmbrella("Ivy")
    end)

target("IvyTests")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("Tests/**.cpp")
    add_deps("IvyMath", "IvyPhysics")
    add_packages("gtest")

target("IvyBenchmarks")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("Benchmarks/**.cpp")
    add_deps("IvyMath", "IvyPhysics", "IvyCore", "IvyGraphics")
    set_pcxxheader("src/pch.hpp")

    add_packages("benchmark")
