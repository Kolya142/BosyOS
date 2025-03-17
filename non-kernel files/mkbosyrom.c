#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s <OUTPUT FILE> <INPUT DIRECTORY>\n", argv[0]);
        return 1;
    }
    FILE *out = fopen(argv[1], "wb");

    DIR *dp;
    struct dirent *dep;

    fwrite("-romfs-", 1, 7, out);
    {
        dp = opendir(argv[2]);
        int fc = 0;
        
        if (!dp) {
            printf("ERROR: invalid input directory\n");
            return 1;
        }
        
        while (dep = readdir(dp)) {
            if (!strcmp(dep->d_name, ".") || !strcmp(dep->d_name, "..")) {
                continue;
            }
            ++fc;
        }

        fwrite(&fc, 4, 1, out);

        closedir(dp);
    }
    {
        chdir(argv[2]);
        dp = opendir(".");
        
        while (dep = readdir(dp)) {
            if (!strcmp(dep->d_name, ".") || !strcmp(dep->d_name, "..")) {
                continue;
            }
            char buf[12] = {0};
            int s = strlen(dep->d_name);
            memcpy(buf, dep->d_name, s < 11 ? s : 11);
            fwrite(buf, 1, 12, out);
            printf("%s\n", dep->d_name);
            struct stat st;
            stat(dep->d_name, &st);
            
            int size = (int)st.st_size;
            fwrite(&size, 4, 1, out);

            FILE *f = fopen(dep->d_name, "rb");
            
            unsigned char buf_data[512];
            int count;
            while (count = fread(buf_data, 1, 512, f)) {
                fwrite(buf_data, count, 1, out);
            }

            fclose(f);
        }

        closedir(dp);
    }

    fclose(out);
}