target("Ivy.Graphics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Graphics/**.cpp")
    add_deps("Ivy.Utilities", "Ivy.Math")
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("glad", { public = true })
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("glfw", { public = true })
