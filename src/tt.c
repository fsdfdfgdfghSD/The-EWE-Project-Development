/* tt -- print the file name of the terminal connected to standard input
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

/* definintions */
#define PROGRAM_NAME "tt"
#define AUTHOR "netheround"

// ...

/* options */

/* flag set by '--help, --version'. */
static int verbose_flag;

/* silent_mode, read more: https://en.wikipedia.org/wiki/Tty_(Unix) */
static bool is_silent = false;

static struct option long_options[] = {
    /* these options set a flag. */
    {"silent", no_argument, 0, 's'},
    {"quiet", no_argument, 0, 's'},

    /* these options don't set a flag. */
    {"help", no_argument, &verbose_flag, 1},
    {"version", no_argument, &verbose_flag, 2},

    // terminating...
    {0, 0, 0, 0}
};

// ...

void
usage (int status)
{
    if (status != EXIT_SUCCESS) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    printf("Usage %s [OPTION]...\n"
    "Print the file name of the terminal connected to standard input.\n\n", PROGRAM_NAME);

    puts("Options\n"
    "  -s, --silent, --quiet\tprint nothing, only return an exit status\n"
    "      --help\t\tdisplay this help and exit\n"
    "      --version\toutput version information and exit\n");
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
    int c;
    while (true) {
        int option_ind = 0;
        c = getopt_long(argc, argv, "s", long_options, &option_ind);
        
        if (c == -1)
            break;
        
        switch (c)
        {
            case 0:
                if (long_options[option_ind].flag != 0)
                    break;
                printf("option %s", long_options[option_ind].name);
                if (optarg)
                    printf(" with arg %s\n", optarg);
                break;
            
            case 's':
                is_silent = true;
                break;
            
            case '?':
                /* getopt_long aleardy printed an error message. */

                usage(EXIT_FAILURE);
                break;
            
            default:
                usage(2);
        }
    };

    switch (verbose_flag)
    {
        case 1:
            usage(EXIT_SUCCESS);
        case 2:
            version_info();
    }

    if (optind < argc) {
        printf("%s: extra operand '%s'\n", PROGRAM_NAME, argv[optind]);
        usage(EXIT_FAILURE);
    }

    if (is_silent)
        return isatty(STDIN_FILENO) ? EXIT_SUCCESS : EXIT_FAILURE;
    
    int status = EXIT_SUCCESS;
    char const *tt = ttyname(STDIN_FILENO);

    /* incase the tty hasnt been found. */
    if (!tt) {
        fprintf(stderr, "%s: standard input is not connected to a terminal (not a tty)\n", PROGRAM_NAME);
        status = EXIT_FAILURE;
    }

    puts(tt);
    return status;
}