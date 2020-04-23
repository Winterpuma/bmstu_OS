#include <stdio.h>
#include <string.h>
#define BUFFSIZE 0x1000

int main(int argc, char*argv)
{
	char buf[BUFFSIZE];
	int n = 0;
	FILE *f;
	f = fopen("/proc/self/stat","r");
	fread(buf, 1, BUFFSIZE, f);
	char* p_ch = strtok(buf, " ");

	printf("\n stat: \n");

	while(p_ch != NULL)
	{
		n++;
		printf("%d. %s \n", n, p_ch);
		p_ch = strtok(NULL, " ");
	}
	fclose(f);
	return 0;
}
