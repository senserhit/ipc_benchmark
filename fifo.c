#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

double
getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}

int main(int argc, char *argv[]) {
    int              fd;
    long              i, size, count, sum, n;
    char            *buf;
    struct timeval   begin, end;

    if (argc != 3) {
        printf("usage: ./fifo <size> <count>\n");
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);

    buf = malloc(size);
    if (buf == NULL) {
        perror("malloc");
        return 1;
    }

    unlink("./fifo-ipc");
    if (mkfifo("./fifo-ipc", 0700) == -1) {
        perror("mkfifo");
        return 1;
    }

    fd = open("./fifo-ipc", O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fork() == 0) {
        sum = 0;
        for (i = 0; i < count; i++) {
            n = read(fd, buf, size);
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
            if (write(fd, buf, size) != size) {
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
