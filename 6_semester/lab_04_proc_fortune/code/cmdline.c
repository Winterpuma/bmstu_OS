#include <stdio.h>
#include <unistd.h>
#define BUFFSIZE 0x1000

int main(int argc, char* argv[])
{
	char buf[BUFFSIZE];
	int len;
	FILE* f = fopen("/proc/self/cmdline","r");
    
    len = fread(buf, 1, BUFFSIZE, f);
    buf[len-1] = 0;
    
    printf("pid: %d\ncmdline:%s\n", getpid(), buf);
    
	fclose(f);
	return 0;
}
