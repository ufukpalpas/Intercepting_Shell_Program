#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Please try again. Format is : producer M");
        return 0;
    }
    const char alphanumerics[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int M  = atoi(argv[1]);
    srandom(time(NULL));
    for(int i = 0; i < M; i++)
        printf("%c", alphanumerics[random() % (sizeof(alphanumerics) - 1)]);
    return 1;
}