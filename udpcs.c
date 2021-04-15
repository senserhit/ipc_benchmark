#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

double
getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}

int main(int argc, char *argv[]) {
    int                  fd, yes;
    long                  i, size, count, sum, n;
    char                *buf;
    struct timeval       begin, end;
    struct sockaddr_in   in;

    if (argc != 5) {
        printf("usage: ./udp <size> <count> <ip> <C|S>\n");
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);
    char * ip = argv[3];
    char * mode = argv[4];
    buf = malloc(size);

    memset(&in, 0, sizeof(in));
    if (strcmp(mode, "S") == 0)
    {
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        in.sin_family = AF_INET;
        in.sin_port = htons(15323);
        inet_pton(AF_INET, ip, &in.sin_addr);

        yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        
        if (bind(fd, (struct sockaddr *)&in, sizeof(in)) == -1) {
            perror("bind");
            return 1;
        }

        sum = 0;
        for (i = 0; i < count; i++) {
            n = recvfrom(fd, buf, size, 0,NULL, NULL);
            if (n == 0) {
                break;
            }
            if (n == -1) {
                perror("recv");
                return 1;
            }
            sum += n;
        }
        if (sum != count * size) {
            fprintf(stderr, "sum error: %ld != %ld\n", sum, count * size);
            return 1;
        }
        close(fd);
    } 
    else 
    {

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        in.sin_family = AF_INET;
        in.sin_port = htons(15323);
        inet_pton(AF_INET, ip, &in.sin_addr);

        gettimeofday(&begin, NULL);

        for (i = 0; i < count; i++) {
            if (sendto(fd, buf, size, 0, (struct sockaddr *)&in, sizeof(in)) != size) {
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
