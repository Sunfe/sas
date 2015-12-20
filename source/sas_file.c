#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>   
#include <sys/types.h>   
#include <sys/stat.h>   

int getFiles(char *path, char**files, int *filenum)   
{   
    struct dirent *pDirEntry = NULL;
    DIR *pDir = NULL;
    int count = 0;   

    if( (pDir = opendir(path)) == NULL )
    {
        printf("opendir failed!\n");
        return 1;
    }
    else
    {
        while( pDirEntry = readdir(pDir) )
        {
            strcpy(files[count], path);
            strcat(files[count], pDirEntry->d_name);
            count++;
        }
        closedir(pDir);
        *filenum = count;
        return 0;    
    }       
}
