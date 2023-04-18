
#pragma once


#include "network_transportable.hpp"
#include <SDL_net.h>
#include <concepts>
#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <vector>


/**
 * @brief This is a wrapper for a client TCPSocket. It has convenience functions to interact with the socket and send serialized data over it,
 *  note that the type 'TCPsocket' is a typedef for some '_TCPsocket *' so it's a raw pointer. You can't a connection yourself, 
 * to get one you have to call 'Server::try_get_client()' instead. That will handle everything for you automatically. 
 * If you would like to add some friend classes to do it manually, here are some notes about that: 
 * The lifetime of the underlying TCPsocket is managed by SDL and therefore it's closed and deallocated on Destruction of this class. 
 * By passing in a socket you give the connection the ownership of this pointer!
 */
struct Connection {

private:
    TCPsocket m_socket;

    /**
     * @brief static variable for the chunk size for receiving data
     */
    static constexpr std::size_t chunk_size = 1024;

public:
    explicit Connection(TCPsocket socket);
    ~Connection();

    //TODO write a method to check if the connection was destroyed

    /**
     * @brief check if the underlying socket has some data available
     * @param timeout_ms an optional paramater how long to wait for new data, default is 3 ms
     * @return tl::expected with the expected value being a bool that describes if data is available and error state is the error as string
     */
    [[nodiscard]] tl::expected<bool, std::string> is_data_available(Uint32 timeout_ms = 3);

    /**
     * @brief get all data that is available. this is a blocking function, so not calling 'Connection::is_data_available()' 
     * before this function may block for a long time! 
     * This isn't supposed to be used, since it returns raw bytes. But you can use it nevertheless, just note, that the underlying data 
     * is malloced and has to be freed. If you only transfer classes that are children of 'Transportable' don't use this, 
     * but use 'Connection::get_data()'
     * @return tl::expected with the expected value being a pointer and length wrapped in a pair. note that teh data was malloced 
     * and has to be freed. Note also that the data is continuous and may be multiple Message one after another. 
     * To avoid this use 'Connection::get_data()' The error state is the error as string
     */
    [[nodiscard]] tl::expected<RawBytes, std::string> get_all_data_blocking();

    /**
     * @brief get the available data of the Connection.
     * @return tl::expected with the expected value being a std::vector<RawTransportData> and the error the error as string
     * the vector contains 0 or more elements of the struct RawTransportData. A vector with length 0 means no data was available.
     * 'RawTransportData' has some helpful template member function that can get it's real type and get the underlying type. 
     * Every data is handled in form of shared pointers that wrap malloced data, it's automatically freed 
     * in the destructor of the std::shared_ptr and all function that are designed to be used with handle this correctly.
     */
    tl::expected<std::vector<RawTransportData>, std::string> get_data();

    /**
     * @brief get the available data of the Connection.
     * @param timeout_ms an optional paramater how long to wait for new data, default is 3 ms
     * @return tl::expected with the expected value being a std::vector<RawTransportData> and the error the error as string
     * the vector contains 0 or more elements of the struct RawTransportData. A vectro with length 0 means no data was available, This has some helpful template member function that can get it's real type and get the underlying type. Every dat is handled in form of shared pointers that wrap malloced data, it'S automatically freed in teh destructor of the std::shared_ptr and all function that are designed to be used with it are 
     */
    [[nodiscard]] tl::expected<std::vector<RawTransportData>, std::string>
    wait_for_data(std::size_t abort_after = 60 * 1000, Uint32 ms_delay = 100);


    template<class T>
    //TODO add again
    //requires std::derived_from<T, Transportable> std::shared_ptr<T>
    [[nodiscard]] tl::optional<std::string> send_data(const T* transportable) {


        const auto serialized = Transportable::serialize<T>(transportable);
        if (!serialized.has_value()) {
            return tl::make_optional("In Connection::send_data: " + serialized.error());
        }

        auto [message, length] = serialized.value();

        const auto result = SDLNet_TCP_Send(m_socket, message, length);
        if (result == -1) {
            std::free(message);
            return tl::make_optional("SDLNet_TCP_Send: invalid socket");
        }

        if (static_cast<std::size_t>(result) != length) {
            std::free(message);
            std::string error = "SDLNet_TCP_Send: " + std::string{ SDLNet_GetError() };
            return tl::make_optional(error);
        }

        std::free(message);

        return tl::nullopt;
    }
};


struct Server {

    //TODO shutdown server on quit

private:
    TCPsocket m_socket;
    std::vector<std::shared_ptr<Connection>> m_connections;

public:
    explicit Server(TCPsocket socket);
    ~Server();
    [[nodiscard]] tl::optional<std::shared_ptr<Connection>> try_get_client();

    [[nodiscard]] tl::optional<std::shared_ptr<Connection>>
    get_client(Uint32 ms_delay = 100, std::size_t abort_after = 60 * 1000);
    template<class T>
    //TODO add again
    //requires std::derived_from<T, Transportable> std::shared_ptr<T>
    [[nodiscard]] tl::optional<std::string>
    send_all(const T* transportable, std::vector<std::shared_ptr<Connection>> send_to) {

        for (std::size_t i = 0; i < send_to.size(); ++i) {
            auto result = send_to.at(i)->send_data<T>(transportable);
            if (result.has_value()) {
                return tl::make_optional(
                        "Error while sending to client: " + std::to_string(i) + " : " + result.value()
                );
            }
        }

        return tl::nullopt;
    }

    template<class T>
    //TODO add again
    //requires std::derived_from<T, Transportable> std::shared_ptr<T>
    [[nodiscard]] tl::optional<std::string> send_all(const T* transportable) {

        return send_all<T>(transportable, m_connections);
    }
};
