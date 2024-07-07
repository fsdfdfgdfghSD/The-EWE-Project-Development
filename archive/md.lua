--[[ md -- make directories
    * Author netheround <myemail@email.com>
    * See copyright notice in LICENSE
]]

--[[

	IMPORTANT: Refactor whole code, with these requirements:
	1. -h, -v should not be mandatory options, so do not allow short options, just long.
	2. Use argparse and needed libraries listed in libraries.txt.
	3. Add -m, --mode option and -v, --verbose, more information below.

	TODOS:

	Info: `man mkdir`
	
	1. Rewrite using argparse > https://argparse.readthedocs.io/en/stable/parsers.html + try to add verbose mode with it ( mandatory options are written as "-", so make help, version not mandatory.)
	2. Use my own fs.c, and add `-m, --mode` option (https://en.wikipedia.org/wiki/Mkdir) which works with numerical permissions, similar to chmod > https://en.wikipedia.org/wiki/Chmod
]]

local lfs = require("lfs")
local split = require("string.split")
local fs = require("fs")

local getopt_long = require("libs.alt_getopt").get_opts
local errno = require("posix.errno").errno

-- Definitions
local EXIT_FAILURE, EXIT_SUCCESS = 1, 0

local PROGRAM_NAME = debug.getinfo(1, "S").source:match("@(.-)%.lua$")
local AUTHORS = { "netheround" }

-- options
local long_opts = {
	version = "v",
	help = "h",
	parents = "p",
}
-- ...

local optind, optarg
optarg, optind = getopt_long(arg, "hvp:", long_opts, PROGRAM_NAME)

local function stderr_errno(dirname)
	local err = errno()

	io.stderr:write(("%s: cannot create directory ‘%s’: %s\n"):format(PROGRAM_NAME, dirname, err))
	os.exit(EXIT_FAILURE)
end

local function create_dir(name)
	local dir = fs.mkdir(name, 0777)
	if not dir then
		stderr_errno(name)
	end
end

local options_callbacks = {
	["h"] = function()
		local formatted_msg = ([[
Usage: %s [OPTION]... DIRECTORY...
Create the DIRECTORY(ies), if they do not already exist.

Options:
  -p, --parents     no error if existing, make parent directories as needed

    -h, --help     display this help and exit
    -v, --version  output version information and exit

Examples:
  %s dir		-> creates directory "dir".
  %s -p a/b/c	-> creates "a" -> "b" -> "c" directories.
  %s -pv a/b	-> creates "a" -> "b" directories with a message for each created directory.
]]):format(PROGRAM_NAME, PROGRAM_NAME, PROGRAM_NAME, PROGRAM_NAME)

		print(formatted_msg)
		os.exit(EXIT_SUCCESS)
	end,

	["v"] = function()
		local formatted_msg = ([[
%s (EWE coreutils) 0.0.1
Copyright (C) 2024 ewe.org.
License GPLv2: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>.

Written by %s.]]):format(PROGRAM_NAME, unpack(AUTHORS))

		print(formatted_msg)
		os.exit(EXIT_SUCCESS)
	end,

	["p"] = function(argument)
		if not argument:find("/") then
			create_dir(argument)
		end

		local dirs = split(argument, "/")
		for _, dir in ipairs(dirs) do
			create_dir(dir)
			local status = lfs.chdir(dir)

			if not status then
				stderr_errno(dirs)
			end
		end
	end,
}

for arg, argument in pairs(optarg) do
	options_callbacks[arg](argument)
end

for i = optind, #arg do
	create_dir(arg[i])
end
