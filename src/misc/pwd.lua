local getcwd = require("fs").getcwd
local errno = require("posix.errno").errno

local PROGRAM_NAME = "pwd"

local function stderr_errno()
	io.stderr:write(("%s: %s\n"):format(PROGRAM_NAME, errno()))
	os.exit(1)
end

local ok, cwd = pcall(getcwd)
if not ok then
	stderr_errno()
end

print(cwd)
