/* true -- do nothing, successfully
   Copyright (C) 2024 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* false.c overrides this, acts like "true" by default. */
#ifndef EXIT_STATUS
# define EXIT_STATUS EXIT_SUCCESS
#endif /* EXIT_STATUS */

#if EXIT_STATUS == EXIT_SUCCESS
# define PROGRAM_NAME "true"
#else
# define PROGRAM_NAME "false"
#endif /* EXIT_STATUS == EXIT_SUCCESS */

#define AUTHOR "netheround"

/* verbose_mode for making a call once exiting. */
bool verbose_mode = false;

void
usage (int status)
{
    printf("Usage: %s [ignored command line arguments]\n"
    "or: %s OPTION\n\n", PROGRAM_NAME, PROGRAM_NAME);

    printf("%s\n\n", (EXIT_STATUS == EXIT_SUCCESS) ? "Exit with a status code indicating success."
    : "Exit with a status code indicating failure.");

    puts("Options:\n"
    "  --help\tdisplay this help and exit\n"
    "  --version\toutput version information and exit\n");

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
main (int argc, char **argv)
{
    /* options handling */
    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            usage(EXIT_STATUS);
        } else if (strcmp(argv[1], "--version") == 0) {
            version_info();
        } else if (strcmp(argv[1], "--verbose") == 0
            || strcmp(argv[1], "-v") == 0) {
            
            verbose_mode = true;
        }
    }

    if (verbose_mode)
        printf("%s: Exited with status code %d (%s)\n", PROGRAM_NAME, EXIT_STATUS,
           (EXIT_STATUS == EXIT_SUCCESS) ? "success" : "failure");

    /* exits with the fixed exit status. */
    return EXIT_STATUS;
}