# Intercepting_Shell_Program
A simple shell program, a command line interpreter, called intercepting shell program and 2 simple programs to make timing experiments.
Codes that you can make timing experiments are commented. 

# Modes

"normal mode": a single unnamed Linux pipe is used for communication, and it is
used directly by the child processes. That means, the output of one child is fed directly to the pipe
from where the second child gets the data. it is enabled by I/O re-direction.

"tapped mode": two Linux unnamed pipes are used and data will flow
indirectly between child processes. The main process is on the data flow path. The main process
creates two pipes. The output of the first child process is directed to the first pipe, from where
the main process reads the incoming stream of bytes (characters). The main process writes those
characters to the second pipe from where the second child takes the input. The main process
reads from first pipe and writes to second pipe N characters at a time using read and write system calls.
N is a value that is given as an argument to the shell program when it is started. N can be between 1
and 4096.

# Shell Parameters

  isp <N> <mode>

where <N> is the number of bytes to read/write in one system call and <mode> is the mode of the
communication to use. If mode value is 1, then normal communication mode is used. If mode value is
2, tapped communication mode is used.

An example invocation can be: “./isp 1 2”. That means, we want to read/write 1 byte at a time and
tapped mode is used.

# Experiments
“producer M” and “consumer M” as two programs to be compounded.
When separately executed, the producer just prints M random alphanumeric characters to screen
one-by-one (one character at a time), and consumer just reads M characters from standard input
(keyboard) one-by-one.
Timing experiments can be done to compare different modes "Normal mode", "Tapped mode" respectively, and to see the effect of change in M value.
