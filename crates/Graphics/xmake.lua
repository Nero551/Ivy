target("IvyGraphics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Graphics/**.cpp")
    add_deps("IvyUtilities", "IvyMath")
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("glad", { public = true })
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("glfw", { public = true })
