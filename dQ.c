#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

// Function to count digits in a file
int count_digits(int fd) {
    int count = 0;
    char buffer;
    // Reset file offset to the beginning
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek failed in count_digits");
        exit(EXIT_FAILURE);
    }
    while (read(fd, &buffer, 1) == 1) {
        if (isdigit(buffer)) {
            count++;
        }
    }
    return count;
}

// Function to count alphabets in a file
int count_alphabets(int fd) {
    int count = 0;
    char buffer;
    // Reset file offset to the beginning
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek failed in count_alphabets");
        exit(EXIT_FAILURE);
    }
    while (read(fd, &buffer, 1) == 1) {
        if (isalpha(buffer)) {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <first_file> <second_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the first file for reading
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        fprintf(stderr, "Error opening first file '%s': %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Open/Create the second file for writing (truncate if exists)
    int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        fprintf(stderr, "Error opening/creating second file '%s': %s\n", argv[2], strerror(errno));
        close(fd1);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        close(fd1);
        close(fd2);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process: Count digits
        int digit_count = count_digits(fd1);

        // Wait for child to finish
        if (wait(NULL) == -1) {
            fprintf(stderr, "Wait failed: %s\n", strerror(errno));
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // Prepare the message
        char message[100];
        int len = snprintf(message, sizeof(message), "Parent PID %d counted %d digits.\n", getpid(), digit_count);
        if (len < 0) {
            fprintf(stderr, "snprintf failed in parent.\n");
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // Write to the second file
        if (write(fd2, message, len) != len) {
            fprintf(stderr, "Error writing to second file: %s\n", strerror(errno));
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

    } else {
        // Child process: Count alphabets
        int alpha_count = count_alphabets(fd1);

        // Prepare the message
        char message[100];
        int len = snprintf(message, sizeof(message), "Child PID %d counted %d alphabets.\n", getpid(), alpha_count);
        if (len < 0) {
            fprintf(stderr, "snprintf failed in child.\n");
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // Write to the second file
        if (write(fd2, message, len) != len) {
            fprintf(stderr, "Error writing to second file: %s\n", strerror(errno));
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // Exit child process
        exit(EXIT_SUCCESS);
    }

    // Close file descriptors
    close(fd1);
    close(fd2);

    return 0;
}
