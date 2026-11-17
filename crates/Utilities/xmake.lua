target("Ivy.Utilities")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Utilities/**.cpp")
    add_deps("Ivy.Diagnostics")
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("stb", { public = true })
