#include <stdio.h>
#define BUFFSIZE 0x1000

int main(int argc, char* argv[])
{
	char buf[BUFFSIZE];
	int len;
	int i;
	FILE* f;

	f = fopen("/proc/self/environ","r");
	while((len = fread(buf, 1, BUFFSIZE, f)) > 0)
	{
		for(i = 0; i < len; i++)
			if(buf[i] == 0)
				buf[i] = 10;
		buf[len -1] = 10;
		printf("%s", buf);
	}
	fclose(f);
	return 0;
}
