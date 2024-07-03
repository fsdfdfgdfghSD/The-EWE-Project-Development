/* tree -- recursive directory and file listing
    * Author netheround <myemail@email.com>
    * See copyright notice in LICENSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>

#ifndef PATH_MAX
/* Must define PATH_MAX, either IDE error. */

# define PATH_MAX 260
#endif /* PATH_MAX */

#define HELP "/?"
#define DEFAULT "."

int list_files(const char *path);

int help()
{
    puts("Help!");
    return EXIT_SUCCESS;
}

int read_dir(DIR* dir, const char *path)
{
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue; /* skip "." and ".." directories "*/

        char buffer[PATH_MAX];
        snprintf(buffer, PATH_MAX, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (list_files(buffer) == -1)
                return -1;
        }
        else printf("%s\n", entry->d_name);
    }

    return EXIT_SUCCESS;
}

int list_files(const char *path)
{
    DIR* dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }

    if (read_dir(dir, path) == -1) {
        closedir(dir);
        return -1;
    }
    closedir(dir);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    char *path = argv[1] ? argv[1] : DEFAULT;
    list_files(path);
}