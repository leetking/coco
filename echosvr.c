#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

#include "co.h"
#include "co_io.h"

static bool quit = false;

static void die(char const *s)
{
    perror(s);
    exit(1);
}

static void *handle_client(void* cli)
{
    int fd = (intptr_t)cli;
    char buf[1024];
    ssize_t rdn = 0;
    while ((rdn = co_read(fd, buf, 1024)) > 0) {
        ssize_t wrn = co_write(fd, buf, rdn);
        if (wrn == -1) {
            perror("write");
            break;
        }
    }
    if (rdn == -1) {
        perror("read");
    }
    close(fd);
    return NULL;
}

void *accept_svr(void *cfg)
{
    int port = (intptr_t)cfg;

    int svr_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_fd == -1) {
        die("socket");
    }

    // 设置SO_REUSEADDR避免TIME_WAIT状态导致的端口占用
    int reuse = 1;
    if (setsockopt(svr_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        die("setsockopt");
    }

    struct sockaddr_in sip;     // IPv4
    memset(&sip, 0, sizeof(sip));
    sip.sin_family = AF_INET;
    sip.sin_port = htons(port);
    sip.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Start echosvr at %s:%d\n", "0.0.0.0", port);

    if (bind(svr_fd, (struct sockaddr const*)&sip, sizeof(sip)) == -1) {
        die("bind");
    }
    if (listen(svr_fd, 7) == -1) {
        die("listen");
    }

    struct sockaddr_in cip;
    socklen_t cip_len;
    while (!quit) {
        intptr_t cli_fd = co_accept(svr_fd, (struct sockaddr*)&cip, &cip_len);
        if (cli_fd == -1) {
            die("accept");
        }
        printf("Accept new client: %d\n", (int)cli_fd);
        cosch_add(handle_client, (void*)cli_fd);
    }
    close(svr_fd);
    printf("bye\n");
    return NULL;
}

bool quit_svr(void *_)
{
    return quit;
}

void print_usage()
{
    printf("Usage: echosvr [port]\n");
}

int main(int argc, char **argv)
{
    intptr_t port = 8182;
    if (argc == 2) {
        port = atoi(argv[1]);
    } else if (argc > 2) {
        print_usage();
        return 0;
    }
    coroutine_t *accept_co = co_create(accept_svr, NULL);
    co_resume(accept_co, (void*)port);
    cosch_take(accept_co);
    cosch_run(quit_svr, NULL);
    return 0;
}
