#include <stdio.h>
#define FILENAME "os5_3_res.txt"

int main()
{
    FILE *fs1 = fopen(FILENAME, "w");
    FILE *fs2 = fopen(FILENAME, "w");
    
    for (char c = 'a'; c <= 'z'; c++)
    {
        if (c % 2)
            fprintf(fs1, "%c", c);
        else
            fprintf(fs2, "%c", c);
    }
    
    fclose(fs2);
    fclose(fs1);
    
    return 0;
}
