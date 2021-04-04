#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#define commandMAX 256
#define GREEN "\033[0;32m"
#define GREENB "\033[1;32m"
#define RED "\033[0;31m"
#define REDB "\033[1;31m"
#define WHITEB "\033[1;37m"
#define clearAll() printf("\033[H\033[J")

char *take_input(){
    char* buf = malloc(sizeof(char) * commandMAX);
    int pos = 0;
    int ch;
    int overlap = 1;
    while(1){ //loop for read operation
        ch = getchar();
        if(ch == EOF || ch == 10){
            buf[pos] = '\0'; //end
            return buf;
        } 
        buf[pos] = ch;
        pos++;
        if((commandMAX * overlap) <= pos){ //realloc
            overlap++;
            buf = realloc(buf, (commandMAX * overlap));
        }
    }
}

char * trim(char *str) { //trim the inputs
    char *last;
    if(str == NULL)
        return NULL;
    while (isspace(*str)) { //leading part
        str = str + 1;
    }
    last = str + strlen(str) - 1; //tail part
    while (last > str && isspace(*last)) {
        last = last - 1;
    }
    *(last+1) = '\0';
    return str;
}

char** divide_input(char* str){
    char** spliteds = malloc(commandMAX * sizeof(char*)); 
    char* split;
    int pos = 0;
    int overlap = 1;
    do{
        split = strsep(&str, " "); //spilt arguments and add to the arr
        if(split == NULL) {
            spliteds[pos] = split;
        } else if(strcmp(split, "producer") == 0){
            spliteds[pos] = "./producer";
        } else if(strcmp(split, "consumer") == 0){
            spliteds[pos] = "./consumer";
        } else
            spliteds[pos] = split;
        pos++;
        if((commandMAX * overlap) <= pos){ //realloc
            overlap++;
            spliteds = realloc(spliteds, (commandMAX * overlap) * sizeof(char*));
        }
    } while(split != NULL);
    spliteds[pos] = NULL;
    return spliteds;
}

char** pipe_div(char* str){ // split in case of pipe
    char** half = malloc(commandMAX * sizeof(char*));
    char* split;
    char* trimedsplit;
    int pos = 0;
    int overlap = 1;

    do{
        split = strsep(&str, "|"); //split by pipe
        trimedsplit = trim(split); //trim 
        half[pos] = trimedsplit;
        pos++;
        if((commandMAX * overlap) <= pos){ //realloc
            overlap++;
            half = realloc(half, (commandMAX * overlap) * sizeof(char*));
        }
    }while(split != NULL);
    half[pos] = NULL;
    return half;
}

void initialize_shell(){ // Opening screen
    clearAll();
    printf(GREEN"\n\n\n\t##############################\n");
    printf(GREEN"\n\n\tWELCOME TO UFUK PALPAS' SHELL\n");
    printf(GREEN"\n\n\t##############################\n");
    sleep(1);
    clearAll();
}

void close_shell(){ // closing screen
    clearAll();
    printf(RED"\n\n\n\t##############################\n");
    printf(RED"\n\n\t\tTHANK YOU\n");
    printf(RED"\n\n\t##############################\n");
    sleep(1);
    clearAll();
}

int execute_command(char** args) { // single command
    pid_t n = 0;
    int status;
    if(strcmp(args[0], "exit") == 0)
        return 0; //close shell

    n = fork();
    if(n < 0){ 
        printf(REDB "Error while forking!!" WHITEB "\n");
    } else if(n == 0){ // Children
        if(execvp(args[0],args) < 0){
            printf("%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else { // Parent
        waitpid(n, &status, 0);
    } 
    return 1;
}

int execute_pipe_com_m1(char** args0, char** args1){ // Normal Mode
    pid_t n = 0;
    pid_t m = 0;
    int status;
    int fd[2];

    if(pipe(fd) < 0){
        printf("Pipe failed while creating."); 
        exit(EXIT_FAILURE);
    }

    n = fork();
    if(n != 0)
        m = fork();

    if(n < 0){ 
        printf(REDB "Error while forking!!" WHITEB "\n");
    } else if(n == 0){ //first child
        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);

        if(execvp(args0[0],args0) < 0){
            printf("%s: command not found\n", args0[0]);
            exit(EXIT_FAILURE);
        }
    } else if(m != 0){ //parent
        close(fd[1]);
    }

    if(m < 0){ // second child
        printf(REDB "Error while forking!!" WHITEB "\n");
    } else if(m == 0){
        close(fd[1]);
        dup2(fd[0],0);
        close(fd[0]);

        if(execvp(args1[0],args1) < 0){
            printf("%s: command not found\n", args1[0]);
            exit(EXIT_FAILURE);
        }
    }
    close(fd[0]); // parent
    
    waitpid(n, &status, 0);
    waitpid(m, &status, 0);
    return 1;
}

int execute_pipe_com_m2(char** args0, char** args1, int size){ //Tapped mode
    pid_t n = 0, m = 0;
    int status;
    int fd0[2];
    int fd1[2];
    char str[size];
    int lesstsize;
    int numOfRD = 0;
    int numOfWT = 0;
    int totalBytesTr = 0;

    if(pipe(fd0) < 0){
        printf("Pipe failed while creating."); 
        exit(EXIT_FAILURE);
    }
    if(pipe(fd1) < 0){
        printf("Pipe failed while creating."); 
        exit(EXIT_FAILURE);
    }

    n = fork();
    if(n != 0)
        m = fork();

    if(n < 0 || m < 0){  // in case of error
        printf(REDB "Error while forking!!" WHITEB "\n");
    } else if(n == 0){ //first child
        close(fd0[0]); // close unused pipes
        close(fd1[0]);
        close(fd1[1]);
        dup2(fd0[1], 1);
        close(fd0[1]);
        if(execvp(args0[0],args0) < 0){
            printf("%s: command not found\n", args0[0]);
            exit(EXIT_FAILURE);
        } 
    } else if(m != 0){ // parent 
        close(fd0[1]);
        close(fd1[0]);
        while((lesstsize = read(fd0[0], str, size)) > 0){ // write and read
            totalBytesTr += lesstsize;
            numOfRD++;
            if(write(fd1[1], str, lesstsize) != lesstsize){            
                printf("\nerror\n");
                break;
            } 
            numOfWT++;
        }
        close(fd0[0]);
        close(fd1[1]);
    }
    
    if(m == 0){ //second child
        close(fd0[0]); // close unused ones
        close(fd0[1]);
        close(fd1[1]);
        dup2(fd1[0],0);
        close(fd1[0]);
        if(execvp(args1[0],args1) < 0){
            printf("%s: command not found\n", args1[0]);
            exit(EXIT_FAILURE);
        }
    }
    waitpid(n, &status, 0);
    waitpid(m, &status, 0);
    printf(GREEN"\nTapped mode stats:\ncharacter-count: %d\n", totalBytesTr);
    printf(GREEN"read-call-count: %d\n", numOfRD);
    printf(GREEN"write-call-count: %d\n", numOfWT);
    return 1;
}

int main(int argc, char *argv[]) {
    char *readline;
    char *readtrim;
    char **getargs;
    char **argsfirst;
    char **argssecond;
    int cont = 1;
    int mode = 1;
    int size = 1;
    //double total = 0; //These are required for execution time calculation for experiments
    //struct timeval start_time;
	//struct timeval end_time;

    //srandom(time(NULL));
    if(argc == 3) { // This part avoids wrong inputs
        if(strcmp(argv[2], "2") == 0)
            mode = 2;
        else if(strcmp(argv[2], "1") != 0){
            printf(REDB "Mode can only be 1 (Normal Mode) or 2 (Tapped Mode)!!" WHITEB "\n");
            return 0;
        }
        if(argv[1] != NULL){
            size = atoi(argv[1]);
            if(size < 1 || size > 4096){
                printf(REDB "N need to be between 1 and 4096 bytes!!" WHITEB "\n");
                return 0;
            }
        }
    } else {
        printf(REDB "Please check your parameters. It should be in form of \"./isp <N> <mode>\"" WHITEB "\n");
        return 0;
    }
    initialize_shell();
    while(1) { //loop of the shell
        printf(REDB "ufkisp$ " WHITEB);
        readline = take_input(); //read
        readtrim = trim(readline); //trim
        getargs = pipe_div(readtrim); //check pipe
        argsfirst = divide_input(getargs[0]); //No pipe or first part of the pipe
        if(getargs[1] != NULL) { //With pipe
            argssecond = divide_input(getargs[1]);
            if(mode == 1){ //Commented codes are to calculate the execution time it is commented after experiments are completed
                //for(int i = 0; i < 100; i++){
                    //sleep(1);
                    //gettimeofday(&start_time, NULL);
                cont = execute_pipe_com_m1(argsfirst, argssecond); // normal mode
                    //gettimeofday(&end_time, NULL);
                    //total += (double) (( (double) end_time.tv_sec +  (double) end_time.tv_usec / 1000000) - ( (double) start_time.tv_sec + (double)  start_time.tv_usec / 1000000));
                    //printf(GREEN"\n i = %d  -- Normal mode execution time: %f\n", i, (double) (( (double) end_time.tv_sec +  (double) end_time.tv_usec / 1000000)-( (double) start_time.tv_sec + (double)  start_time.tv_usec / 1000000)));
                //}
                //printf("Total: %f --- Average: %f", total, total / 100);
            } else {
                //for(int i = 0; i < 100; i++){
                    //sleep(1);
                    //gettimeofday(&start_time, NULL);
                cont = execute_pipe_com_m2(argsfirst, argssecond, size); // tapped mode
                    //gettimeofday(&end_time, NULL);
                    //total += (double) (( (double) end_time.tv_sec +  (double) end_time.tv_usec / 1000000) - ( (double) start_time.tv_sec + (double)  start_time.tv_usec / 1000000));
                    //printf("\n i = %d  -- Tapped mode execution time: %f\n", i, (double) (( (double) end_time.tv_sec +  (double) end_time.tv_usec / 1000000)-( (double) start_time.tv_sec +  (double) start_time.tv_usec / 1000000)));
                //}
                //printf("Total: %f --- Average: %f", total, total / 100);
            }
        } else { //No pipe
            cont = execute_command(argsfirst);
        }
        free(argsfirst); //free allocations
        if(getargs[1] != NULL)
            free(argssecond);
        free(readline);
        free(getargs);
        if(cont == 0){  //exit
            close_shell();
            break; //exit from shell
        }
    }
}