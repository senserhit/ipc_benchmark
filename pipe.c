#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


double
getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}


int
main(int argc, char *argv[]) {
    int              pipefd[2] = {0};

    long              i, size, count, sum, n;
    char            *buf;
    struct timeval   begin, end;

    if (argc != 3) {
        printf("usage: ./pipe <size> <count>\n");
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);

    buf = malloc(size);
    if (buf == NULL) {
        perror("malloc");
        return 1;
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    if (fork() == 0) {
        sum = 0;
        for (i = 0; i < count; i++) {
            n = read(pipefd[0], buf, size);
            if (n == -1) {
                perror("read");
                return 1;
            }

            sum += n;
        }

        if (sum != count * size) {
            fprintf(stderr, "sum error: %ld != %ld\n", sum, count * size);
            return 1;
        }

    } else {
        gettimeofday(&begin, NULL);

        for (i = 0; i < count; i++) {
            if (write(pipefd[1], buf, size) != size) {
                perror("wirte");
                return 1;
            }
        }
        gettimeofday(&end, NULL);

        double tm = getdetlatimeofday(&begin, &end);
        printf("duration %lf\n", tm);
        printf("%.0fMB/s  %.0fMbps %.0fmsg/s\n",
            (long)count * size * 1.0 / (tm * 1024 * 1024),
            (long)count * size * 1.0 * 8 / (tm * 1024 * 1024),
            (long)count * 1.0 / tm);
    }

    return 0;
}
