#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

double
getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}

int main(int argc, char *argv[])
{
    int fd;
    long i, size, count, sum, n;
    char *buf;
    size_t len;
    struct timeval begin, end;
    struct sockaddr_un un;

    if (argc != 3)
    {
        printf("usage: ./udsd <size> <count>\n");
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);
    buf = malloc(size);

    memset(&un, 0, sizeof(un));
    if (fork() == 0)
    {        
        fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        unlink("./udsd-ipc");
        un.sun_family = AF_UNIX;
        strcpy(un.sun_path, "./udsd-ipc");
        len = offsetof(struct sockaddr_un, sun_path) + strlen("./udsd-ipc");

        if (bind(fd, (struct sockaddr *)&un, len) == -1)
        {
            perror("bind");
            return 1;
        }

        sum = 0;

        for (i = 0; i < count; i++) {
            n = recvfrom(fd, buf, size, 0,NULL,NULL);
            if (n == 0)
            {
                break;
            }
            else if (n == -1)
            {
                perror("read");
                return 1;
            }
            sum += n;
        }
        
        if (sum != count * size)
        {
            fprintf(stderr, "sum error: %ld != %ld\n", sum, count * size);
            return 1;
        }
    }
    else
    {
        sleep(1);

        fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        un.sun_family = AF_UNIX;
        strcpy(un.sun_path, "./udsd-ipc");
        len = offsetof(struct sockaddr_un, sun_path) + strlen("./udsd-ipc");

        gettimeofday(&begin, NULL);

        for (i = 0; i < count; i++)
        {
            if (sendto(fd, buf, size, 0, (struct sockaddr *)&un, len) != size) {
                perror("sendto");
                return 1;
            }
        }

        gettimeofday(&end, NULL);

        double tm = getdetlatimeofday(&begin, &end);
        printf("duration %lf\n", tm);
        printf("%.0fMB/s %.0fMbps %.0fmsg/s\n",
               (long)count * size * 1.0 / (tm * 1024 * 1024),
               (long)count * size * 1.0 * 8 / (tm * 1024 * 1024),
               count * 1.0 / tm);
    }

    return 0;
}
