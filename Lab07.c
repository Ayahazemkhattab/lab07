#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("error\n");
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        printf("error\n");
        return 1;
    }

    dup2(fd, STDOUT_FILENO);

    char c1, c2, c3;
    scanf("%c %c %c", &c1, &c2, &c3);

    printf("c1 = %c, c2 = %c, c3 = %c\n", c1, c2, c3);

    close(fd);

    return 0;
}
