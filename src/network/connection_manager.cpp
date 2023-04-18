
#include "connection_manager.hpp"
#include "network_manager.hpp"
#include "network_transportable.hpp"
#include <SDL.h>
#include <cstdlib>
#include <memory>
#include <string>
#include <tl/optional.hpp>
#include <vector>

Connection::Connection(TCPsocket socket) : m_socket{ socket } {};

Connection::~Connection() {
    SDLNet_TCP_Close(m_socket);
}


Server::Server(TCPsocket socket) : m_socket{ socket }, m_connections{ std::vector<std::shared_ptr<Connection>>{} } {};

Server::~Server() {
    SDLNet_TCP_Close(m_socket);
}


tl::optional<std::shared_ptr<Connection>> Server::try_get_client() {

    TCPsocket client;
    /* try to accept a connection */
    client = SDLNet_TCP_Accept(m_socket);
    if (client) { /* no connection accepted */
        auto connection = std::make_shared<Connection>(client);
        m_connections.push_back(connection);
        return connection;
    }
    return tl::nullopt;
}

tl::optional<std::shared_ptr<Connection>> Server::get_client(Uint32 ms_delay, std::size_t abort_after) {
    auto start_time = SDL_GetTicks64();
    while (true) {
        /* try to accept a connection */
        auto client = try_get_client();
        if (client.has_value()) {
            return client;
        }

        auto elapsed_time = SDL_GetTicks64() - start_time;
        if (elapsed_time >= abort_after) {
            return tl::nullopt;
        }

        SDL_Delay(ms_delay);
        continue;
    }
}


tl::expected<bool, std::string> Connection::is_data_available(Uint32 timeout_ms) {

    SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
    if (!set) {
        return tl::make_unexpected("no more memory for creating a SDLNet_SocketSet");
    }


    auto num_sockets = SDLNet_TCP_AddSocket(set, m_socket);
    if (num_sockets != 1) {
        SDLNet_FreeSocketSet(set);
        return tl::make_unexpected("SDLNet_AddSocket failed, this is an implementation error");
    }

    auto result = SDLNet_CheckSockets(set, timeout_ms);
    if (result == -1) {
        SDLNet_FreeSocketSet(set);
        return tl::make_unexpected("SDLNet_CheckSockets error (select() system call error)");
    }


    SDLNet_FreeSocketSet(set);
    return result == 1;
}


tl::expected<RawBytes, std::string> Connection::get_all_data_blocking() {
    void* memory = std::malloc(Connection::chunk_size);
    if (!memory) {
        return tl::make_unexpected("error in malloc for receiving a socket message");
    }
    std::uint32_t data_size = 0;
    while (true) {
        int len = SDLNet_TCP_Recv(m_socket, memory, Connection::chunk_size);
        if (len <= 0) {
            free(memory);
            return tl::make_unexpected("SDLNet_TCP_Recv: " +std::string{ SDLNet_GetError() });
        }

        if (len != Connection::chunk_size) {

            return RawBytes{ static_cast<uint8_t*>(memory), data_size + len };
        }

        data_size += Connection::chunk_size;
        void* new_memory = std::realloc(memory, data_size + Connection::chunk_size);
        if (!new_memory) {
            free(memory);
            return tl::make_unexpected("error in realloc for receiving a socket message");
        }
        memory = new_memory;
    }

    return tl::make_unexpected("error in SDLNet_TCP_Recv: somehow exited the while loop");
}
/*
On error tl::unexpected<std:.string> is returned
if no data is available tl::optional is returned
if data is available a vector of such data is returned, it is guaranteed to be nonempty!
*/
MaybeData Connection::get_data() {

    auto data_available = is_data_available();
    if (!data_available.has_value()) {
        return tl::make_unexpected("in is_data_available: " + data_available.error());
    }

    if (!data_available.value()) {
        return tl::nullopt;
    }

    auto data = get_all_data_blocking();
    if (!data.has_value()) {
        return tl::make_unexpected("in get_all_data_blocking: " + data.error());
    }


    RawBytes raw_bytes = data.value();

    auto result = RawTransportData::from_raw_bytes(raw_bytes);
    if (!result.has_value()) {
        free(raw_bytes.first);
        return tl::make_unexpected("in RawTransportData::from_raw_bytes: " + result.error());
    }

    free(raw_bytes.first);
    return tl::make_optional(result.value());
}


tl::expected<std::vector<RawTransportData>, std::string>
Connection::wait_for_data(std::size_t abort_after, Uint32 ms_delay) {
    auto start_time = SDL_GetTicks64();
    while (true) {
        /* try if data is available  */
        auto is_data = is_data_available();
        if (!is_data.has_value()) {
            return tl::make_unexpected("In Connection::wait_for_data: " + is_data.error());
        }

        if (is_data.value()) {
            auto result = get_data();
            if (!result.has_value()) {
                return tl::make_unexpected("In Connection::wait_for_data: " + result.error());
            }

            if (!result.value().has_value()) {
                return tl::make_unexpected("In Connection::wait_for_data: " + result.error());
            }

            return result.value().value();
        }

        auto elapsed_time = SDL_GetTicks64() - start_time;
        if (elapsed_time >= abort_after) {
            return tl::make_unexpected("In Connection::wait_for_data: took to long");
        }

        SDL_Delay(ms_delay);
        continue;
    }
}