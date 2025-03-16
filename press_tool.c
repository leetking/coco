#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <arpa/inet.h>

#include <glib.h>

#include "co_io.h"

#define autoclose __attribute__((cleanup(auto_close_fd)))
static inline void auto_close_fd(int *fd)
{
    if (fd && *fd >= 0)
        close(*fd);
}

struct press_args {
    char const *host;
    int port;
};

int connect_to_server(char const *host, int port)
{
    // TODO: getaddrinfo for host

    struct sockaddr_in host_ip = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        //.sin_addr.s_addr = ?,
    };
    if (!inet_pton(AF_INET, host, &host_ip.sin_addr)) {
        printf("Unspport host %s\n", host);
        return -1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd) {
        perror("socket");
        return -1;
    }
    if (co_connect(fd, (struct sockaddr const*)&host_ip, sizeof(host_ip)) == -1) {
        perror("connect");
        return -1;
    }
    //printf("co[#%zu] connected to %s:%d fd: %d\n", co_id(), host, port, fd);
    return fd;
}

int alive_press = 10;
void *press_host(void *x)
{
    struct press_args const *target = (struct press_args*)x;
    autoclose int fd = connect_to_server(target->host, target->port);
    if (-1 == fd) {
        goto over;
    }

    g_autoptr(GString) hello = g_string_new("Hello World!");
    char buf[1024];
    for (int i = 0; i < 1000; ++i) {
        int wrn = co_write(fd, hello->str, hello->len);
        if (wrn <= 0) {
            perror("write");
            break;
        }
        int rdn = co_read(fd, buf, 1024);
        if (rdn <= 0) {
            perror("read");
            break;
        }
        //printf("co[#%zu]: '%.*s'\n", co_id(), rdn, buf);
    }
    //printf("co[#%zu]: Done.\n", co_id());

over:
    alive_press -= 1;
    return NULL;
}

bool abort_press(void *data)
{
    return !alive_press;
}

void print_usage()
{
    printf("Usage: press_tool [host]:port [concurrent]\n");
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) {
        print_usage();
        return 1;
    }
    g_autofree char *host = g_strdup("127.0.0.1");
    g_auto(GStrv) tokens = g_strsplit(argv[1], ":", -1);
    if (!g_str_equal(tokens[0], "")) {
        g_free(host);
        host = g_steal_pointer(&tokens[0]);
    }
    int port = atoi(tokens[1]);
    if (argc == 3) {
        alive_press = atoi(argv[2]);
    }
    printf("press x %d %s:%d\n", alive_press, host, port);
    struct press_args target = {
        .host = host,
        .port = port,
    };

    for (int i = 0; i < alive_press; ++i) {
        cosch_add(press_host, &target);
    }
    cosch_run(abort_press, NULL);
    printf("All done.\n");
    return 0;
}
