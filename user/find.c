#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/syscall.h"

// Recursive function to find files with a specific name in a directory tree
void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de; // Structure for directory entry
    struct stat st;    // Structure for file status

    // Open the directory at the given path
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Get the status of the directory
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Ensure the path is a directory
    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    // Read the directory entries
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        // Skip empty entries
        if (de.inum == 0)
            continue;

        // Construct the full path for the entry
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // Null-terminate the string

        // Skip the current (.) and parent (..) directories
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // Get the status of the entry
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // Check if the entry matches the given filename
        if (strcmp(de.name, filename) == 0) {
            printf("%s\n", buf); // Print the full path if it matches
        }

        // Recurse into subdirectories
        if (st.type == T_DIR) {
            find(buf, filename); // Call find recursively
        }
    }

    // Close the directory
    close(fd);
}

// Main function to handle command line arguments and initiate the search
int main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc < 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1); // Exit if not enough arguments
    }

    // Start the search process
    find(argv[1], argv[2]);
    exit(0); // Exit successfully
}
