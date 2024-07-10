/* cwd -- peint the name of the current working directory
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
#include <unistd.h>

#include <limits.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

/*
TODO: Portability.

On windows there's no such environment variable `PWD` so just throw an error when trying to print it with logical.
On windows theres _getcwd, readmore: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd?view=msvc-170

*/

// TODO: -L and -P options.

/* definitions */
#define PROGRAM_NAME "cwd"
#define AUTHOR "netheround"

/* PATH_MAX definitions for limits.h */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATH_MAX */

// ...

/* options */

/* flag set by '--help, --version'. */
static int verbose_flag;

/* logical, physical modes, read more: https://en.wikipedia.org/wiki/Pwd */
static bool is_logical = false;
static bool is_physical = false;

static struct option long_options[] = {
    /* these options set a flag. */
    {"physical", no_argument, 0, 'P'},
    {"logical", no_argument, 0, 'L'},

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
print_cwd()
{
   char *buf = (char *)malloc(PATH_MAX * sizeof(char));
   if (buf == NULL)
      return -1;
   
   char *cwd = getcwd(buf, PATH_MAX);
   if (cwd == NULL) {
      free(buf);
      return -1;
   }

   puts(cwd);
   free(buf);

   return EXIT_SUCCESS;
}

int
main (int argc, char **argv)
{
   int c;
   while (true) {
      int option_ind = 0;
      c = getopt_long(argc, argv, "LP", long_options, &option_ind);

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
         
         case 'L':
            is_logical = true;
            break;

         case 'P':
            is_physical = true;
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

   /* main logic */

   int status = print_cwd();
   if (status == -1) {
      fprintf(stderr, "%s: error fetching current working directory: %s", PROGRAM_NAME, strerror(errno));
      return EXIT_FAILURE;
   }

   return status;
   // ...
}