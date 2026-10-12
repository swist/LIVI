/* Minimal TCP line client: connect to 127.0.0.1:<port>, send argv[2]+"\n", print whatever comes
 * back for up to ~1.5s, then exit. For probing livid's loopback-only control ports by hand on a
 * device with no nc/netcat. */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: tcpcli <port> <command>\n");
        return 2;
    }
    int port = atoi(argv[1]);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return 1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }

    char line[4096];
    int n = snprintf(line, sizeof(line), "%s\n", argv[2]);
    write(fd, line, (size_t)n);

    struct timeval tv = {1, 500000};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    char buf[4096];
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        write(1, buf, (size_t)r);
    }
    close(fd);
    return 0;
}
