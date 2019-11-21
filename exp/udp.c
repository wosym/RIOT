#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"
#include "thread.h"

#define SERVER_MSG_QUEUE_SIZE   (8)
#define SERVER_BUFFER_SIZE      (64)

static bool server_running = false;
static sock_udp_t sock;
static char server_buffer[SERVER_BUFFER_SIZE];
static char server_stack[THREAD_STACKSIZE_DEFAULT];

void *_udp_server(void *args)
{
    sock_udp_ep_t server = { .port = atoi(args), .family = AF_INET6};

    if(sock_udp_create(&sock, &server, NULL, 0) < 0) {
        return NULL;
    }

    server_running = true;
    printf("Started UDP sever on port %u\n", server.port);

    while(1) {
        int res = 0;

        res = sock_udp_recv(&sock, server_buffer, sizeof(server_buffer) -1, \
                SOCK_NO_TIMEOUT, NULL);
        if(res < 0) {
            puts("Error while receiving UDP data");
        } else if (res == 0) {
            puts("No data received");
        } else {
            server_buffer[res] = '\0';
            printf("Recvd: %s\n", server_buffer);
        }
    }

    return NULL;
}

int udp_send(int argc, char **argv)
{
    int res = 0;

    if(argc != 4) {
        puts("Usage: udp_send <ipv6_addr> <port> <payload>");
        return -1;
    }

    sock_udp_ep_t remote = { .family = AF_INET6};

    if(ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, argv[1]) == NULL) {
        puts("Error: unable to parse destination address");
        return -1;
    }
    if(ipv6_addr_is_link_local((ipv6_addr_t *)&remote.addr)) {
        //Choose first address when destination is link local
        gnrc_netif_t *netif = gnrc_netif_iter(NULL);
        remote.netif = (uint16_t)netif->pid;
    }

    remote.port = atoi(argv[2]);
    if((res = sock_udp_send(NULL, argv[3], strlen(argv[3]), &remote)) < 0) {
        puts("failed to send");
    } else {
        printf("Succes: sent %u bytes to %s\n", (unsigned) res, argv[1]);
    }



    return 0;
}

int udp_server(int argc, char **argv)
{
    int ret = 0;

    if(argc != 2)
    {
        puts("Usage: udp_server <port>");
        return -1;
    }

    if (server_running == false)
    {
        ret = thread_create(server_stack, sizeof(server_stack), \
                THREAD_PRIORITY_MAIN -1, THREAD_CREATE_STACKTEST, \
                _udp_server, argv[1], "UDP Server");
        if(ret <= KERNEL_PID_UNDEF)
        {
            puts("failed starting udp thread");
            return -1;
        }
    } else {
        puts("Server already running");
        return -1;
    }



    return 0;
}




