/* mkdir -- make directories
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
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <getopt.h>

#define PROGRAM_NAME "md"
#define AUTHOR "netheround"

// TODO: Fix argument parsing for options required arguments. `md -p=0` or `md -p a/b` and so on.
// TODO Once parents option is done, try it out doing stuff like `md -pv a/b` and so on.

/* options */

/* Flag set by '--help, --version'. */
static int verbose_flag;

/* verbose option, see: `man 1 mkdir` */
static bool is_verbose = false;

/* mode option, see: `man 1 mkdir` and unix file permissions. */
static mode_t mode = S_IRWXU; // read, write, execute/search by owner

static struct option long_options[] = {
    /* These options set a flag. */
    {"verbose", no_argument, 0, 'v'},
    {"parents", no_argument, 0, 'p'},
    {"mode", required_argument, 0, 'm'},

    /* These options don't set a flag. */
    {"help", no_argument, &verbose_flag, 1},
    {"version", no_argument, &verbose_flag, 2},

    // terminating...
    {0, 0, 0, 0}
};

// ...

int
create_dir (const char *dirname, mode_t mode)
{
    if (mkdir(dirname, mode) == -1) {
        fprintf(stderr, "%s: cannot create directory ‘%s’: %s\n", PROGRAM_NAME, dirname, strerror(errno));
        return -1;
    }

    if (is_verbose)
        printf("%s: created directory '%s'\n", PROGRAM_NAME, dirname);
}

int
create_parents (const char *dirname, mode_t mode)
{
    // todo
}

int
usage (int status)
{
    if (status == EXIT_FAILURE) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    printf("Usage: %s [OPTION]... DIRECTORY...\n"
    "Create the DIRECTORY(ies), if they do not already exist.\n\n", PROGRAM_NAME);

    puts("Options:\n"
    "  -m, --mode=MODE\tset file mode (as in cm)\n"
    "  -p, --parents\t\tno error if existing, make parent directories as needed\n"
    "  -v, --verbose\t\tprint a message for each created directory\n\n"
    
    "\t--help\t\tdisplay this help and exit\n"
    "\t--version\toutput version information and exit\n");

    printf("Examples:\n"
    "  %s test      -> creates directory 'test' if it doesn't exist.\n"
    "  %s a b c     -> creates directories 'a', 'b', 'c', if they do not exist aleardy.\n", PROGRAM_NAME, PROGRAM_NAME);
    exit(status);
}

int
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
    while (1) {
        int option_ind = 0;
        c = getopt_long(argc, argv, "vpm:",
            long_options, &option_ind);

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
            
            case 'v':
                is_verbose = true;
                break;
            
            case 'p':
                // todo:
                break;

            case 'm':
                // todo:
                break;

            case '?':
                /* getopt_long aleardy printed an error message. */

                usage(EXIT_FAILURE);
                break;

            default:
                abort();
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
        while (optind < argc) {
            /* succesfully creating directories */

            const char *dirname = argv[optind++];
            if (create_dir(dirname, mode) == -1)
                exit(EXIT_FAILURE);
        }
    } else if (optind < argc + 1) {
        printf("%s: missing operand\n", PROGRAM_NAME);
        usage(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}