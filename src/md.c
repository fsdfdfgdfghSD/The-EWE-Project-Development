/* md/mkdir -- make directories
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
#include <limits.h> /* PATH_MAX */

#include "include/stripslash.c"

/* definitions */

#ifdef _WIN32
# include <direct.h>

# define md_mkdir(path) (_mkdir(path))
# define PATH_SEP "\\"
#elif defined(__linux__)
# define md_mkdir(path) (mkdir((path), \
    S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH))
# define PATH_SEP "/"
#endif /* _WIN32 */

/* mkdir.c overrides this, acts like "md" by default. */
#ifndef PROGRAM_NAME
# define PROGRAM_NAME "md"
#endif /* PROGRAM_NAME */

#define AUTHOR "netheround"

/* PATH_MAX definitions for limits.h */
#ifndef PATH_MAX
# define PATH_MAX PATH_MAX
#endif /* PATH_MAX */

// ...

/* options */

/* flag set by '--help, --version'. */
static int verbose_flag;

/* parents option, see: `man 1 mkdir` */
static bool is_parents = false;

/* verbose option, see: `man 1 mkdir` */
static bool is_verbose = false;

/* explicit verbose option expands the verbose message by displaying the current-working-path where it created it. */
static bool explicit_verbose = false;

static struct option long_options[] = {
    /* these options set a flag. */
    {"verbose", no_argument, 0, 'v'},
    {"explicit", no_argument, 0, 'e'},
    {"parents", no_argument, 0, 'p'},

    /* these options don't set a flag. */
    {"help", no_argument, &verbose_flag, 1},
    {"version", no_argument, &verbose_flag, 2},

    // terminating...
    {0, 0, 0, 0}
};

// ...

int
make_dir (const char *dirname)
{
    if (md_mkdir(dirname) == -1) {
        if (errno == EEXIST) {
            /* if EEXIST error, skip it */

            return 0;
        } else {
            fprintf(stderr, "%s: cannot create directory '%s': %s\n", PROGRAM_NAME, dirname, strerror(errno));
            return -1;
        }
    }

    if (explicit_verbose) {
        char *cwd = NULL;
        size_t size = PATH_MAX * sizeof(char);

        cwd = (char *)malloc(size);
        if (cwd == NULL) {
            fprintf(stderr, "%s: memory allocation failed\n", PROGRAM_NAME);
            return -1;
        }

        while (getcwd(cwd, size) == NULL) {
            if (errno == ERANGE) {
                /* Path is too long for allocated buffer, realloc with larger size */
                size *= 2;
                cwd = (char *)realloc(cwd, size);
                if (cwd == NULL) {
                    fprintf(stderr, "%s: memory allocation failed\n", PROGRAM_NAME);
                    return -1;
                }
            } else {
                fprintf(stderr, "%s: could not get current working directory: %s\n", PROGRAM_NAME, strerror(errno));
                free(cwd);
                return -1;
            }
        }

        printf("%s: created directory '%s' in: '%s%s%s'\n", PROGRAM_NAME, dirname, cwd, PATH_SEP, dirname);
        free(cwd);
    } else if (is_verbose) {
        printf("%s: created directory '%s'\n", PROGRAM_NAME, dirname);
    }

    return 0;
}

int
create_dir (const char *dirname)
{
    /* handling parented directories */
    if (is_parents) {
        char *dir_cpy = strdup(dirname);
        if (dir_cpy == NULL) {
            fprintf(stderr, "%s: memory allocation failed\n", PROGRAM_NAME);
            return -1;
        }

        /* Normalize directory path */
        strip_trailing_slashes(dir_cpy);

        char *slash = dir_cpy;
        while ((slash = strchr(slash + 1, '/')) != NULL) {
            /* Null terminate at the last '/ */
            *slash = '\0';

            make_dir(dir_cpy);
            *slash = '/';
        }
        free(dir_cpy);
    }
    return make_dir(dirname);
}

void
usage (int status)
{
    if (status != EXIT_SUCCESS) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    printf("Usage: %s [OPTION]... DIRECTORY...\n"
    "Create the DIRECTORY(ies), if they do not already exist.\n\n", PROGRAM_NAME);

    puts("Options:\n"
    "  -p, --parents\t\tno error if existing, make parent directories as needed\n"
    "  -v, --verbose\t\tprint a message for each created directory\n"
    "  -e, --explicit\tsimilar to '-v, --verbose', prints a message with more explicit information.\n\n"
    
    "      --help\t\tdisplay this help and exit\n"
    "      --version\toutput version information and exit\n");

    printf("Examples:\n"
    "  %s test      -> creates directory 'test' if it doesn't exist.\n"
    "  %s a b c     -> creates directories 'a', 'b', 'c', if they do not exist aleardy.\n"
    "  %s -ep a/b   -> creates directories 'a' and 'b' inside of 'a' while printing a message for each created directory.\n", PROGRAM_NAME, PROGRAM_NAME, PROGRAM_NAME);
    exit(status);
}

void
version_info()
{
    printf("%s (EWE Coreutils) 0.0.2\n"
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
        c = getopt_long(argc, argv, "vep", long_options, &option_ind);

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
            
            case 'e':
                explicit_verbose = true;
                break;

            case 'p':
                is_parents = true;
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

    /* create directories from command line arguments */
    if (optind < argc) {
        while (optind < argc) {
            /* succesfully creating directories */

            const char *dirname = argv[optind++];
            if (create_dir(dirname) == -1)
                exit(EXIT_FAILURE);
        }
    } else if (argc == 1) {
        printf("%s: missing operand\n", PROGRAM_NAME);
        usage(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}