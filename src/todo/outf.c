/* outf -- output formatted standard output
   Copyright (C) 2024 The EWE Project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <https://www.gnu.org/licenses/>. */
/* Written by netheround <myemail@email.com> */

/*
    TODO: options
    * Every single formatter for the `man 1 printf` using my custom printf implementation using `man 2 write`
    * help page
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

/* definitions */

#define PROGRAM_NAME "outf"
#define AUTHOR "netheround"

// ...

void
usage (int status)
{
    if (status != EXIT_SUCCESS) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    puts("help");
    exit(status);
}

void
version_info()
{
    printf("%s (EWE Coreutils) 0.0.1\n"
    "Copyright (C) 2024\n"
    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
    "This is free software: you are free to change and redistribute it.\n"
    "\nWritten by %s\n", PROGRAM_NAME, AUTHOR);
    exit(EXIT_SUCCESS);
}

int
output_formatted (const char *format, ...)
{
    // ...
    va_list args;
    va_start(args, format);

    va_end(args);
}

int
main (int argc, char **argv)
{
    /* options handling */
    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            usage(EXIT_SUCCESS);
        } else if (strcmp(argv[1], "--version") == 0) {
            version_info();
        }
    }

    return EXIT_SUCCESS;
}