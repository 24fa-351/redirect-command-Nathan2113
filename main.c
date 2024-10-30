#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// #define READ_SIDE 0
// #define WRITE_SIDE 1

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <inputFile> <outputFile> <command> [<arg1> <arg2>...]\n", argv[0]);
        return 1;
    }

    // Open the input file for reading
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        fprintf(stderr, "Failed to open %s for reading\n", argv[1]);
        return 1;
    }

    // Open the output file for writing
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1) {
        fprintf(stderr, "Failed to open %s for writing\n", argv[2]);
        close(input_fd);
        return 1;
    }

    // Put arguments into newargv to use in execv
    char** newargv = (char **)malloc(sizeof(char*) * (argc - 2));

    // Copy command and arguments starting from argv[3]
    for (int ix = 3; ix < argc; ix++) {
        newargv[ix - 3] = argv[ix];
    }
    newargv[argc - 3] = NULL;

    int child_pid = fork();
    
    if (child_pid == 0) {
        dup2(input_fd, STDIN_FILENO); // Redirect stdin to input file
        dup2(output_fd, STDOUT_FILENO); // Redirect stdout to output file

        close(input_fd);
        close(output_fd);

        execv(newargv[0], newargv); // Use execv for absolute paths
        // If execv fails
        perror("execv failed");
        exit(1);
    }

    // Parent process
    close(input_fd);
    close(output_fd);
    wait(NULL); // Wait for child process to finish

    printf("%s pid is %d. forked %d. Parent exiting\n", argv[0], getpid(), child_pid);
    
    return 0;
}