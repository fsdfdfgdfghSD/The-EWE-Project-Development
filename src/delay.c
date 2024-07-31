/* delay -- delay for a specified amount of time
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
/* gcc delay.c -o delay ./include/xnanosleep.c */

/*

TODO:

don't allow `delay -0s`
add verbose option that prints every second
Make it cross platform by using Sleep on windows.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "include/xnanosleep.h"

#define PROGRAM_NAME "delay"
#define AUTHOR "netheround"

void
usage (int status)
{
    if (status != EXIT_SUCCESS) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    printf("Usage: %s NUMBER[SUFFIX]...\n"
    "  or:  %s OPTION\n", PROGRAM_NAME, PROGRAM_NAME);

    puts("Pause for NUMBER seconds.  SUFFIX may be 's' for seconds (the default),\n"
    "'m' for minutes, 'h' for hours or 'd' for days.  NUMBER need not be an\n"
    "integer.  Given two or more arguments, pause for the amount of time\n"
    "specified by the sum of their values.\n\n"
    "      --help\t\tdisplay this help and exit\n"
    "      --version\t\toutput version information and exit\n");

    printf("Examples:\n"
    "  %s 1h 30m    -> pauses for 1 hour and 30 minutes.\n"
    "  %s 10        -> pauses for 10 seconds.\n", PROGRAM_NAME, PROGRAM_NAME);
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

static bool
apply_suffix (double *x, char suffix)
{
    int multiplier;
    switch (suffix)
    {
        /* no suffix */
        case 0:
        
        case 's':
            multiplier = 1;
            break;
        case 'm':
            multiplier = 60;
            break;
        case 'h':
            multiplier = 3600;
            break;
        case 'd':
            multiplier = 86400;
            break;
        
        default:
            return false;
    }

    *x *= multiplier;
    return true;
}

static void
parse_cli_args (int argc, char **argv)
{
    if (argc == 1) {
        printf("%s: missing operand\n", PROGRAM_NAME);
        usage(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "--help") == 0) {
        usage(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "--version") == 0) {
        version_info();
    }
}

int
main (int argc, char **argv)
{
    double seconds = 0;
    bool ok = true;

    /* initializing */
    setlocale(LC_ALL, "");
    parse_cli_args(argc, argv);

    for (int i = optind; i < argc; i++) {
        char *endptr;
        double s = strtod(argv[i], &endptr);

        if (endptr == argv[i] || errno == ERANGE
            /* nonnegative interval */
            || !(0 <= s)
            /* no extra chars after the number and an optional s,m,h,d char. */
            || (*endptr && *(endptr + 1))
            /* check any sufix char and update 's' based on sufix. */
            || !apply_suffix(&s, *endptr))
        {
            fprintf(stderr, "%s: invalid time interval '%s'\n", PROGRAM_NAME, argv[i]);
            ok = false;
        }

        seconds += s;
    }

    if (!ok)
        usage(EXIT_FAILURE);

    /* wait the specified amount of seconds */
    if (xnanosleep(seconds)) {
        fprintf(stderr, "%s: cannot read realtime clock.\n", PROGRAM_NAME);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}