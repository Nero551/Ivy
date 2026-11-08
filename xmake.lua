set_languages("c++26")
set_toolchains("clang")

add_rules("plugin.compile_commands.autoupdate", { lsp = "clang" })
add_rules("mode.debug", "mode.release")

add_requires("tracy v0.13.1", "glfw 3.4", "glad 0.1.36", "stb", "gtest", "benchmark 1.9.5")
add_requireconfs("**", { override = true, configs = { shared = false, system = false } })

includes("crates/Utilities")
includes("crates/Math")
includes("crates/Physics")
includes("crates/Graphics")
includes("crates/ECS")
includes("crates/Core")

target("Ivy")
    set_kind("binary")
    set_rundir(os.projectdir())
    add_includedirs("src")
    set_pcxxheader("src/pch.hpp")
    add_files("src/**.cpp")
    add_deps("IvyUtilities", "IvyMath", "IvyPhysics", "IvyGraphics", "IvyECS", "IvyCore")

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

target("IvyTests")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("tests/**.cpp")
    add_deps("IvyMath", "IvyPhysics", "IvyUtilities")
    add_packages("gtest")

target("IvyBenchmarks")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("benchmarks/**.cpp")
    add_deps("IvyUtilities", "IvyMath", "IvyPhysics", "IvyECS", "IvyGraphics", "IvyCore")
    set_pcxxheader("src/pch.hpp")

    add_packages("benchmark")
