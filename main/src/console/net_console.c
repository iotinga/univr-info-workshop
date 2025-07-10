#include "adapter/logging.h"

#include "console/console.h"
#include "core/service.h"

#ifdef DEVICE
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#endif

#ifdef EMULATOR
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

LOG_INIT(net_console)

/* Local port the console server will listen on. */
#define CONSOLE_PORT 3333

/* Keep-alive idle time. In idle time without receiving any data from peer, will send keep-alive probe packet */
#define CONSOLE_KEEPALIVE_IDLE 5

/* Keep-alive probe packet interval time. */
#define CONSOLE_KEEPALIVE_INTERVAL 5

/* Keep-alive probe packet retry count. */
#define CONSOLE_KEEPALIVE_COUNT 3

static int Console_WriteFd(void *writeFunctionCtx, size_t length, const char *string)
{
    return write((int)writeFunctionCtx, string, length);
}

static void Console_GetCommand(const int clientSocket)
{
    int length;
    char cmd[500];

    ConsoleCtx ctx = {
        .writeFunction = Console_WriteFd,
        .writeFunctionCtx = (void *)clientSocket,
    };

    do
    {
        length = read(clientSocket, &cmd, sizeof(cmd));
        if (length > 0)
        {
            cmd[length - 1] = '\0';
            Console_RunCommand(&ctx, cmd);
        }
    }
    while (length > 0);

    LOG_INFO("client disconnected");
}

void Console_NetConsoleMain(Service *service)
{
    (void)service;

    LOG_DEBUG("open socket...");

    char clientAddressStr[128];
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;
    int keepAlive = 1;
    int keepIdle = CONSOLE_KEEPALIVE_IDLE;
    int keepInterval = CONSOLE_KEEPALIVE_INTERVAL;
    int keepCount = CONSOLE_KEEPALIVE_COUNT;

    int serverSocket = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (serverSocket < 0)
    {
        LOG_ERROR("unable to create socket (%d)", errno);
        return;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = addr_family;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(CONSOLE_PORT);

    LOG_DEBUG("bind socket...");
    int bindError = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindError < 0)
    {
        LOG_ERROR("bind failure (%d)", errno);
        goto failure;
    }

    LOG_INFO("socket bound, port %d", CONSOLE_PORT);

    int listenError = listen(serverSocket, 1);
    if (listenError < 0)
    {
        LOG_ERROR("error occurred during listen: (%d)", errno);
        goto failure;
    }

    while (true)
    {
        LOG_INFO("remote console listening");

        struct sockaddr_in clientAddress;
        socklen_t clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket < 0)
        {
            LOG_ERROR("unable to accept connection (%d)", errno);
            goto failure;
        }

        /* set tcp keepalive option */
        setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));

#ifndef __APPLE__
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
#endif

        /* convert ip address to string */
        if (clientAddress.sin_family == PF_INET)
        {
            inet_ntop(AF_INET, &(((struct sockaddr_in *)&clientAddress)->sin_addr), clientAddressStr,
                      sizeof(clientAddressStr));
        }

        LOG_INFO("client %s connected to remote console", clientAddressStr);

        Console_GetCommand(clientSocket);

        shutdown(clientSocket, 0);
        close(clientSocket);
    }

failure:
    close(serverSocket);
}

SERVICE_DEFINE(SERVICE_NETWORK_CONSOLE){
    .name = "TEL",
    .main = Console_NetConsoleMain,
    .taskStackSize = 4096,
};