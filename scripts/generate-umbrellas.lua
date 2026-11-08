local RootDir = path.join(os.projectdir(), "")

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
