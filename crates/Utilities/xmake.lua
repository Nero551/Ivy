target("Ivy.Utilities")
    set_kind("static")
    set_rundir(os.projectdir())
    add_includedirs(".", { public = true })
    add_files("Utilities/**.cpp")
    ---@diagnostic disable-next-line: param-type-not-match
    add_packages("stb", { public = true })
    if is_mode("debug") then
        ---@diagnostic disable-next-line: param-type-not-match
        add_packages("tracy", { public = true })
    end
