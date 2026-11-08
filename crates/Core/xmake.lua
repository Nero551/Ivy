
target("IvyCore")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Core/**.cpp")
    add_deps("IvyUtilities", "IvyGraphics", "IvyECS")
