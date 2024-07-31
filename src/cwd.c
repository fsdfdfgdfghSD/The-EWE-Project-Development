/* cwd -- print the name of the current working directory
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

#include <limits.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

/* definitions */
#define PROGRAM_NAME "cwd"
#define AUTHOR "netheround"

#ifdef _WIN32
# include <direct.h>

# define cwd_getpwd(buf, size) (_getcwd(buf, size))
#elif defined (__linux__)
# define cwd_getpwd(buf, size) (getcwd(buf, size))
#endif /* _WIN32 */

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

   printf("Usage %s [OPTION]...\n"
   "Print the name of the current working directory.\n\n", PROGRAM_NAME);

   puts("Options\n"
   "  -L, --logical\t\tuse PWD from environment, only if your system has it.\n"
   "  -P, --physical\tavoid all symlinks\n"
   "      --help\t\tdisplay this help and exit\n"
   "      --version\t\toutput version information and exit\n");
   
   printf("By default, '%s' behaves as if '-L' were specified.\n", PROGRAM_NAME);

   printf("Examples:\n"
   "  %s -L    -> print the current working directory using PWD.\n"
   "  %s       -> print the current working directory.\n", PROGRAM_NAME, PROGRAM_NAME);
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

char*
get_curent_directory()
{
   char *buf = (char *)malloc(PATH_MAX * sizeof(char));
   if (buf == NULL) {
      fprintf(stderr, "%s: memory allocation failed\n", PROGRAM_NAME);
      return NULL;
   }
   
   char *cwd;
   if (is_logical) {
      cwd = getenv("PWD");
      if (cwd == NULL) {
         fprintf(stderr, "%s: 'PWD' environment variable not found.\n", PROGRAM_NAME);
         exit(EXIT_FAILURE);

         free(buf);
         return NULL;
      }

      /* Duplicate the string to ensure it's writable */
      cwd = strdup(cwd);
      if (cwd == NULL) {
         return NULL;
         free(buf);
      }
   } else {
      cwd = getcwd(buf, PATH_MAX);
      if (cwd == NULL) {
         free(buf);
         return NULL;
      }
   }

   return cwd;
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
            is_logical = false;
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

   char *cwd = get_curent_directory();
   if (cwd == NULL) {
      fprintf(stderr, "%s: error fetching current working directory: %s", PROGRAM_NAME, strerror(errno));
      return EXIT_FAILURE;
   }
   
   printf("%s\n", cwd);
   free(cwd);

   return EXIT_SUCCESS;
   // ...
}