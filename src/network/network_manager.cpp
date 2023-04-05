
#include "network_manager.hpp"
#include <SDL_net.h>
#include <string>
#include <tl/expected.hpp>


NetworkManager::NetworkManager() : m_connections{ std::vector<Connection>{} } {};

// started from example from: https://gist.github.com/psqq/b92243f2149fcf4dd46370d4c0b5fef9
MaybeConnection NetworkManager::add_connection(NetworkType type) {

    if (type == NetworkType::Server) {

        bool done = false;
        TCPsocket server, client;
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, NULL, NetworkManager::Port) == -1) {
            std::string error = "SDLNet_ResolveHost: " + std::string{ SDLNet_GetError() };
            return tl::make_unexpected(error);
        }
        server = SDLNet_TCP_Open(&ip);
        if (!server) {
            std::string error = "SDLNet_TCP_Open: " + std::string{ SDLNet_GetError() };
            return tl::make_unexpected(error);
        }
        while (!done) {
            /* try to accept a connection */
            client = SDLNet_TCP_Accept(server);
            if (!client) { /* no connection accepted */
                /*printf("SDLNet_TCP_Accept: %s\n",SDLNet_GetError()); */
                SDL_Delay(100); /*sleep 1/10th of a second */
                continue;
            }

            /* get the clients IP and port number */
            IPaddress* remoteip;
            remoteip = SDLNet_TCP_GetPeerAddress(client);
            if (!remoteip) {
                std::string error = "SDLNet_TCP_GetPeerAddress: " + std::string{ SDLNet_GetError() };
                return tl::make_unexpected(error);
            }

            /* print out the clients IP and port number */
            Uint32 ipaddr;
            ipaddr = SDL_SwapBE32(remoteip->host);
            printf("Accepted a connection from %d.%d.%d.%d port %hu\n", ipaddr >> 24, (ipaddr >> 16) & 0xff,
                   (ipaddr >> 8) & 0xff, ipaddr & 0xff, remoteip->port);

            while (1) {
                /* read the buffer from client */
                char message[1024];
                int len = SDLNet_TCP_Recv(client, message, 1024);
                if (!len) {
                    std::string error = "SDLNet_TCP_Recv: " + std::string{ SDLNet_GetError() };
                    return tl::make_unexpected(error);
                }
                /* print out the message */
                printf("Received: %.*s\n", len, message);
                if (message[0] == 'q') {
                    printf("Disconecting on a q\n");
                    break;
                }
                if (message[0] == 'Q') {
                    printf("Closing server on a Q.\n");
                    done = true;
                    break;
                }
            }
            SDLNet_TCP_Close(client);
        }
    } else if (type == NetworkType::Client) {
        printf("Starting client...\n");
        IPaddress ip;
        TCPsocket tcpsock;

        if (SDLNet_ResolveHost(&ip, NetworkManager::ServerHost, NetworkManager::Port) == -1) {
            std::string error = "SDLNet_ResolveHost: " + std::string{ SDLNet_GetError() };
            return tl::make_unexpected(error);
        }

        tcpsock = SDLNet_TCP_Open(&ip);
        if (!tcpsock) {
            std::string error = "SDLNet_TCP_Open: " + std::string{ SDLNet_GetError() };
            return tl::make_unexpected(error);
        }

        while (1) {
            printf("message: ");
            char message[1024];
            [[maybe_unused]] auto result = fgets(message, 1024, stdin);
            int len = strlen(message);

            /* strip the newline */
            message[len - 1] = '\0';

            if (len) {
                int result;

                /* print out the message */
                printf("Sending: %.*s\n", len, message);

                result = SDLNet_TCP_Send(tcpsock, message, len); /* add 1 for the NULL */
                if (result < len) {
                    std::string error = "SDLNet_TCP_Send: " + std::string{ SDLNet_GetError() };
                    return tl::make_unexpected(error);
                }
            }

            if (len == 2 && tolower(message[0]) == 'q') {
                break;
            }
        }

        SDLNet_TCP_Close(tcpsock);
    }

    return {};
}