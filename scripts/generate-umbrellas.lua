local RootDir = path.join(os.projectdir(), "src")

function GenerateUmbrellaHeader(dir, subsystem)
    local subsystemDir = path.join(RootDir, dir)
    local output = path.join(subsystemDir, subsystem .. ".hpp")

    local headers = os.files(path.join(subsystemDir, "**.hpp"))

    -- Don't include the umbrella header itself.
    local filtered = {}

    for _, header in ipairs(headers) do
        if path.filename(header) ~= subsystem .. ".hpp" then
            table.insert(filtered, header)
        end
    end

    table.sort(filtered)

    local content = "#pragma once\n\n"

    for _, header in ipairs(filtered) do
        local relative = path.relative(header, subsystemDir)

        content = content .. '#include "' .. relative .. '"\n'
    end

    io.writefile(output, content)
end

function GenerateEngineUmbrella(name)
    local output = path.join(RootDir, name .. ".hpp")

    local subsystemDirs = os.dirs(path.join(RootDir, "*"))
    local moduleHeaders = os.files(path.join(RootDir, "Modules/*/Header.hpp"))

    table.sort(subsystemDirs)
    table.sort(moduleHeaders)

    local content = "#pragma once\n\n"

    for _, dir in ipairs(subsystemDirs) do
        local subsystem = path.filename(dir)
        local umbrella = path.join(dir, subsystem .. ".hpp")

        if os.isfile(umbrella) then
            content = content ..
                    '#include "' .. subsystem .. '/' .. subsystem .. '.hpp"\n'
        end
    end

    for _, header in ipairs(moduleHeaders) do
        local moduleDir = path.directory(header)
        local module = path.filename(moduleDir)

        content = content ..
                '#include "Modules/' .. module .. '/Header.hpp"\n'
    end

    io.writefile(output, content)
end