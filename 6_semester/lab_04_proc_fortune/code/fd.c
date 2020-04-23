#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#define BUFFSIZE 0x1000

int main(int argc, char* argv)
{
	struct dirent *dirp;
	DIR *dp;

	char string[BUFFSIZE];
	char path[BUFFSIZE];
	dp = opendir("/proc/self/fd"); // open directory
	
    printf("\nfd:\n");

	while((dirp = readdir(dp)) != NULL) // read directory
	{
		if((strcmp(dirp->d_name, ".") !=0 ) &&
           (strcmp(dirp->d_name, "..") != 0))
		{
			sprintf(path, "%s%s", "/proc/self/fd/", dirp->d_name);
			readlink(path, string, BUFFSIZE);
			path[BUFFSIZE] = '\0';
			printf("%s -> %s\n", dirp->d_name, string);
		}
	}
	closedir(dp);
	return 0;
}
