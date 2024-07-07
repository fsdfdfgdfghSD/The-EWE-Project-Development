/* md -- make directories
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

/* definitions */

#define PROGRAM_NAME "md"
#define AUTHOR "netheround"

#define md_mkdir(path) (mkdir((path), \
    S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH))

#ifndef PATH_MAX
# define PATH_MAX 4096
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

static int
make_dir (const char *dirname)
{
    /* ignoring errors from creating parented directories: for instance if a and b exist, and you do `md -p a/b/c` it might throw few errors that `a` and `b` exist aleardy. */
    // TODO: Find potential fix for it ^

    if (md_mkdir(dirname) == -1 && !is_parents) {
        fprintf(stderr, "%s: cannot create directory ‘%s’: %s\n", PROGRAM_NAME, dirname, strerror(errno));
        return -1;
    }

    /* formatting for verbose */
    const char *fmt = (explicit_verbose && is_verbose) ?
                        "%s: created directory '%s' in: '%s'\n" :
                        (is_verbose) ?
                        "%s: created directory '%s'\n" :
                        NULL;

    char buf[PATH_MAX];
    const char *cwd = getcwd(buf, sizeof(buf));

    // TODO: Use chdir to change to the last directory if creating using parents.
    // TODO: For verbose mode, you gotta print just only when a new directory is created, im currently ignoring while using parents to do `md -vep a/b/c` while a and b exis.

    if (cwd == NULL) {
        fprintf(stderr, "%s: could not return the current-working-directoy: %s\n", PROGRAM_NAME, strerror(errno));
        return -1;
    }

    printf(fmt, PROGRAM_NAME, dirname, cwd);
    return 0;
}

static int
create_dir (const char *dirname)
{
    char buf[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(buf, sizeof(buf), "%s", dirname);
    len = strlen(buf);

    if (buf[len - 1] == '/') {
        buf[len - 1] = 0;
    }
    
    for (p = buf + 1; *p; ++p) {
        if (*p == '/') {
            *p = 0;
            make_dir(buf);
            *p = '/';
        }
    }

    return make_dir(buf);
}

static int
usage (int status)
{
    if (status == EXIT_FAILURE) {
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    }

    printf("Usage: %s [OPTION]... DIRECTORY...\n"
    "Create the DIRECTORY(ies), if they do not already exist.\n\n", PROGRAM_NAME);

    puts("Options:\n"
    "  -p, --parents\t\tno error if existing, make parent directories as needed\n"
    "  -v, --verbose\t\tprint a message for each created directory\n"
    "  -e, --explicit\tenables explicit information for verbose when calling `-v, --verbose`\n\n"
    
    "\t--help\t\tdisplay this help and exit\n"
    "\t--version\toutput version information and exit\n");

    printf("Examples:\n"
    "  %s test      -> creates directory 'test' if it doesn't exist.\n"
    "  %s a b c     -> creates directories 'a', 'b', 'c', if they do not exist aleardy.\n"
    "  %s -vp a/b   -> creates directories 'a' and 'b' inside of 'a' while printing a message for each created directory.\n", PROGRAM_NAME, PROGRAM_NAME, PROGRAM_NAME);
    exit(status);
}

static void
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

    /* create directories from command line arguments */
    if (optind < argc) {
        while (optind < argc) {
            /* succesfully creating directories */

            const char *dirname = argv[optind++];
            if (create_dir(dirname) == -1)
                exit(EXIT_FAILURE);
        }
    } else if (optind < argc + 1) {
        printf("%s: missing operand\n", PROGRAM_NAME);
        usage(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}