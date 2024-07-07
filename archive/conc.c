/* conc -- displays the contents of a text file or files.
    * Author netheround <myemail@email.com>
    * See copyright notice in LICENSE
*/

//TODO: Add the following options:
/*
 * -e, --show-ends          display $ at end of each line
 * -n, --number             number all output lines
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>

#include <getopt.h>
#include <string.h>
#include <errno.h>

#define PROGRAM_NAME "conc"
#define AUTHOR "netheround"

/* We're reading files using chunks, that's the most "efficient" way i could think of. */
#define CHUNK_SIZE 1024

/* options */
static struct option const longopts[] =
{
  {"help", no_argument, 0, 'h'},
  {"no-header", no_argument, 0, 'p'},
  {"version", no_argument, 0, 'v'},

  // terminating
  {0, 0, 0, 0}
};

static bool no_header = false;
// ...

void
usage (int status)
{
    if (status == EXIT_FAILURE)
    {
        fprintf(stderr, "Try '%s -h, --help' for more information.\n", PROGRAM_NAME);
        exit(status);
    } else {
        printf("Displays the contents of a text file or files.\n\n"
        "%s [path:]<filename>\n", PROGRAM_NAME);

        puts("Options:\n"
        "  -p, --no-header\thides the header file\n"
        "  -h, --help\t\tdisplay this help and exit\n"
        "  -v, --version\t\toutput version information and exit\n");

        printf("Examples:\n"
        "  %s info.txt           -> concatenating a single file\n"
        "  %s info.txt info2.txt -> concatenating multiple files\n", PROGRAM_NAME, PROGRAM_NAME);
        exit(status);
    }
}

void
version_page ()
{
    printf("%s (EWE coreutils) 0.0.1\n"
    "Copyright (C) 2024 ewe.org.\n"
    "License GPLv2: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>.\n\n"
    "Written by %s.\n", PROGRAM_NAME, AUTHOR);
    exit(EXIT_SUCCESS);
}

static void
concat_file (const char *filename, int is_last)
{
    int fd = open(filename, O_RDONLY);
    char buffer[CHUNK_SIZE];
    ssize_t nread;

    if (fd == -1)
        {
            fprintf(stderr, "%s: %s\n", filename, strerror(errno));
            return;
        }
    if (!no_header)
        {
            printf("[%s]\n\n", filename);
        }

    while ((nread = read(fd, buffer, sizeof(buffer))) > 0)
        {
            if (write(STDOUT_FILENO, buffer, nread) != nread)
            {
                perror("Failed to write to stdout\n");
                close(fd);
                return;
            }
        }

    if (nread == -1)
        {
            perror("Error reading file\n");
        }

    close(fd);

    if (!is_last){putchar('\n');}
}

int
main (int argc, char *argv[])
{
    int opt;
    while ((opt = getopt_long(argc, argv, "hpv", longopts, NULL)) != -1)
       {
        switch (opt)
            {
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case 'p':
                no_header = true;
                break;
            case 'v':
                version_page();
                break;
            default:
                usage(EXIT_FAILURE);
            }
        }

    if (argc < optind + 1)
        {
            fprintf(stderr, "%s: missing operand\n", PROGRAM_NAME);
            usage(EXIT_FAILURE);
        }

    for (; optind < argc; ++optind)
        {
            const char *fp = argv[optind];
            int res = optind == argc - 1;

            concat_file(fp, res);
        }

    return EXIT_SUCCESS;
}