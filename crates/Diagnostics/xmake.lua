target("Ivy.Diagnostics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Diagnostics/**.cpp")
    if not is_mode("release") then
        add_packages("tracy", { public = true })
    end
