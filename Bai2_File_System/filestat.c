// filestat.c
// Build: gcc -Wall -Wextra -o filestat filestat.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>

void print_file_type(mode_t mode) {
    if (S_ISREG(mode))       printf("Regular File\n");
    else if (S_ISDIR(mode))  printf("Directory\n");
    else if (S_ISLNK(mode))  printf("Symbolic Link\n");
    else if (S_ISCHR(mode))  printf("Character Device\n");
    else if (S_ISBLK(mode))  printf("Block Device\n");
    else if (S_ISFIFO(mode)) printf("FIFO (pipe)\n");
    else if (S_ISSOCK(mode)) printf("Socket\n");
    else                    printf("Unknown\n");
}

void print_permissions(mode_t mode) {
    char perms[11] = "----------";
    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';

    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';

    // handle suid/sgid/sticky bits
    if (mode & S_ISUID) perms[3] = (perms[3]=='x') ? 's' : 'S';
    if (mode & S_ISGID) perms[6] = (perms[6]=='x') ? 's' : 'S';
    if (mode & S_ISVTX) perms[9] = (perms[9]=='x') ? 't' : 'T';

    printf("%s\n", perms);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    struct stat st;

    if (lstat(path, &st) == -1) {
        fprintf(stderr, "lstat('%s') failed: %s\n", path, strerror(errno));
        return 2;
    }

    printf("File Path: %s\n", path);

    printf("File Type: ");
    print_file_type(st.st_mode);

    // Size
    printf("Size: %lld bytes\n", (long long) st.st_size);

    // Last Modified
    char timestr[64];
    struct tm *tm = localtime(&st.st_mtime);
    if (tm) {
        if (strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm) > 0) {
            printf("Last Modified: %s\n", timestr);
        } else {
            printf("Last Modified: (format error)\n");
        }
    } else {
        printf("Last Modified: (invalid time)\n");
    }

    // Optional helpful info
    printf("Hard Link Count: %lu\n", (unsigned long) st.st_nlink);

    // Owner (username) and group (group name) if possible
    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    printf("Owner: %s (uid=%u)\n", pw ? pw->pw_name : "UNKNOWN", (unsigned) st.st_uid);
    printf("Group: %s (gid=%u)\n", gr ? gr->gr_name : "UNKNOWN", (unsigned) st.st_gid);

    // Permissions (rwx)
    printf("Permissions: ");
    print_permissions(st.st_mode);

    // If it's a symlink, read the link target
    if (S_ISLNK(st.st_mode)) {
        ssize_t len;
        char target[PATH_MAX + 1];
        len = readlink(path, target, sizeof(target) - 1);
        if (len != -1) {
            target[len] = '\0';
            printf("Symbolic Link Target: %s\n", target);
        } else {
            printf("Symbolic Link Target: (readlink failed: %s)\n", strerror(errno));
        }
    }

    return 0;
}
