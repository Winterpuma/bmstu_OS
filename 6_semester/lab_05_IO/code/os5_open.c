// Другой вариант программы с вызовом функции open().

#include <fcntl.h>
int main()
{
    int fd1 = open("q.txt", O_RDWR);
    int fd2 = open("q.txt", O_RDWR);
    int curr = 0;
    for(char c = 'a'; c <= 'z'; c++)
    {
        if (c % 2)
            write(fd1, &c, 1);
        else
            write(fd2, &c, 1);
    }
    close(fd1);
    close(fd2);
    return 0;
} 
 
