local diriter = require("posix.dirent").dir
local errno = require("posix.errno").errno

local PROGRAM_NAME = "dir"

local function stderr_errno()
	io.stdout:write(("%s: %s\n"):format(PROGRAM_NAME, errno()))
	os.exit(1)
end

local ok, files = pcall(diriter, ".")
if not ok then
	stderr_errno()
end

for _, file in ipairs(files) do
	if file == "." or file == ".." then
		-- skip "." and ".." directories
	else
		print(file)
	end
end
