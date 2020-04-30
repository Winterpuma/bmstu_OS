#include <fcntl.h>
#include <unistd.h>

int main()
{
    char c;
    int flag = 1;
    
    // have kernel open two connection to file alphabet.txt
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);
    // read a char & write it alternatingly from connections fs1 & fd2
    while(flag)
    {
        if (read(fd1,&c,1) == 1) 
        {
            write(1,&c,1);
            if (read(fd2,&c,1) == 1) 
                write(1,&c,1);
            else
                flag = 0;
        }
        else
        {
            flag = 0;
        }
    }
    return 0;
}

// Файл alphabet.txt содержит символы: abcdefghijklmnopqrstuvwxyz
