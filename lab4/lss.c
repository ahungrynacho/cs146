#include <stdio.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>
#define SIZE 4096
#define MAX_DIRS 128

typedef struct Entry{
    char * path;    // absolute path
    char * file;    // file name
    int size;       // file size
} Entry;


char * format_time(const time_t * timep) {
    /* Prints the most recent modification time of a file in military time. */
    struct tm * time = localtime(timep);
    char * result = (char *) malloc(sizeof(char) * 128);

    strncpy(result, ctime(timep)+4, 12);
    result[12] = '\0';
    return result;
}
void print_info(char * path, char * file, int padding) {
    /* Prints the file permissions, size, time stamp, and file name. */
    struct stat info;
    stat(path, &info);

    
    switch(info.st_mode & S_IFMT) {
        case S_IFDIR:
            printf("d");
            break;
        case S_IFLNK:
            printf("l");
            break;
        default:
            printf("-");
            break;
    }
    printf((info.st_mode & S_IRUSR) ? "r" : "-");
    printf((info.st_mode & S_IWUSR) ? "w" : "-");
    printf((info.st_mode & S_IXUSR) ? "x" : "-");
    printf((info.st_mode & S_IRGRP) ? "r" : "-");
    printf((info.st_mode & S_IWGRP) ? "w" : "-");
    printf((info.st_mode & S_IXGRP) ? "x" : "-");
    printf((info.st_mode & S_IROTH) ? "r" : "-");
    printf((info.st_mode & S_IWOTH) ? "w" : "-");
    printf((info.st_mode & S_IXOTH) ? "x" : "-");
   

    struct passwd * user;
    struct group * grp;

    user = getpwuid(info.st_uid);
    grp = getgrgid(info.st_gid);
    
    printf(" %llu %s %s %*d %s %s\n", 
            info.st_nlink, 
            user->pw_name, 
            grp->gr_name, 
            padding,
            info.st_size,
            format_time(&info.st_mtime),
            file);

}

int comparator(const void * lhs, const void * rhs) {
    /* Used to sort Entry objects by size, resolving ties by lexicographic order. */
    Entry * _lhs = (Entry *) lhs;
    Entry * _rhs = (Entry *) rhs;

    if (_lhs->size > _rhs->size)
        return -1;
    else if (_lhs->size < _rhs->size)
        return 1;
    else
        return strcmp(_lhs->file, _rhs->file);
}

int places(int num) {
    /* Counts the number of places of an integer */
    int count = 1;

    while (num > 10) {
        num = num / 10;
        ++count;
    }
    return count;
}

void lss(char * path) {
    /* Reads files in the current directory */
    int file_count = 0;
    DIR * dirp = opendir(path);

    while (1) {
        struct dirent * dp = readdir(dirp);
        if (dp == NULL)
            break;
        ++file_count;
    }
    closedir(dirp);

    Entry * entries = (Entry *) malloc(sizeof(Entry) * file_count);
    dirp = opendir(path);
    for (int i = 0; i < file_count; ++i) {
        struct dirent * dp = readdir(dirp);
        if (dp == NULL)     // no more files to be read in the directory
            break;
       
        char * absolute_path = malloc(sizeof(char) * (strlen(path) + strlen(dp->d_name)));
        sprintf(absolute_path, "%s/%s", path, dp->d_name);

        struct stat info;
        stat(absolute_path, &info);

        if (errno == ELOOP) {   // If a symlink points to nowhere
            perror("Error");
            exit(ELOOP);
        }

        (entries[i]).path = absolute_path;
        (entries[i]).file = dp->d_name;
        (entries[i]).size = info.st_size;

    }

    qsort(entries, file_count, sizeof(Entry), comparator);
    int padding = places((entries[0]).size);    // pads diplayed file sizes according
                                                // to the largest file
    for (int i = 0; i < file_count; ++i) {
        print_info((entries[i]).path, (entries[i]).file, padding);
    }
    
    closedir(dirp);
}


int main(int argc, char ** args) {
    char * dir = ".";

    lss(dir);
    return 0;
}
