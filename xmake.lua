set_languages("c++26")
set_toolchains("clang")

---@diagnostic disable-next-line: param-type-not-match
add_rules("plugin.compile_commands.autoupdate", { lsp = "clang" })
add_rules("mode.debug", "mode.release")

add_requires("tracy v0.13.1", "glfw 3.4", "glad 0.1.36", "stb", "gtest", "benchmark 1.9.5", "nlohmann_json")
add_requireconfs("**", { override = true, configs = { shared = false, system = false } })

if is_mode("release") then
    set_policy("build.optimization.lto", true)
    add_defines("RELEASE")

else
    -- set_policy("build.sanitizer.address", true)
    -- set_policy("build.sanitizer.undefined", true)
    -- set_policy("build.sanitizer.thread", true)
    -- set_policy("build.sanitizer.memory", true)
    add_defines("DEBUG", "TRACY_ENABLE", "TRACY_PROFILE_MEMORY")
end

-- set_warnings("all", "extra", "pedantic")

includes(
"crates/Utilities",
"crates/Math",
"crates/Graphics",
"crates/ECS",
"crates/Reflect",
"crates/Diagnostics",
"crates/Physics",
"crates/Core"
)

target("Ivy")
    set_kind("binary")
    set_rundir(os.projectdir())
    add_includedirs("src")
    set_pcxxheader("src/pch.hpp")
    add_files("src/**.cpp")
    add_deps("Ivy.Utilities", "Ivy.Math", "Ivy.Physics", "Ivy.Graphics", "Ivy.ECS", "Ivy.Core", "Ivy.Reflect", "Ivy.Diagnostics")
    add_packages("nlohmann_json")

    if is_plat("linux") then
        ---@diagnostic disable-next-line: param-type-not-match
        add_ldflags("-static-libstdc++", "-static-libgcc", { force = true })
    elseif is_plat("windows") then
        set_runtimes("MT")
    end

    if is_mode("release") then
        remove_files("src/TracyMemory.cpp")
    else
        add_links("TracyClient")
    end

target("Ivy.Tests")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("tests/**.cpp")
    add_deps("Ivy.Math", "Ivy.Physics", "Ivy.Utilities", "Ivy.Reflect", "Ivy.Diagnostics", "Ivy.Graphics")
    add_packages("gtest")

target("Ivy.Benchmarks")
    set_kind("binary")
    set_rundir(os.projectdir())

    add_files("benchmarks/**.cpp")
    add_deps("Ivy.Utilities", "Ivy.Math", "Ivy.Physics", "Ivy.ECS", "Ivy.Graphics", "Ivy.Core")
    set_pcxxheader("src/pch.hpp")

    add_packages("benchmark")
