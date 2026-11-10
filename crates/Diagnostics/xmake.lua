target("Ivy.Diagnostics")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Diagnostics/**.cpp")
    if not is_mode("release") then
        ---@diagnostic disable-next-line: param-type-not-match
        add_packages("tracy", { public = true })
    end
