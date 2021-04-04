#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Please try again. Format is : consumer M");
        return 0;
    }
    int M  = atoi(argv[1]);
    char str[M + 1];
    for(int i = 0; i < M; i++)
        scanf(" %c", &str[i]);
    str[M] = 0;
    //printf(" %s", str);
    return 1;
}
