#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <errno.h>

int create_dir(const char *pathname)
{
    mode_t mode = 0777; // READ-WRITE-EXECUTE (rwxrwxrwx)
    if (mkdir(pathname, mode) == -1) {
        fprintf(stderr, "Error: %s", strerror(errno));
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    return create_dir(argv[1]);
}