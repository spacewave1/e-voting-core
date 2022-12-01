//
// Created by wnabo on 03.01.2022.
//

#ifndef VOTE_P2P_ZMQSOCKETADAPTER_H
#define VOTE_P2P_ZMQSOCKETADAPTER_H

#include "../network/abstractSocket.h"
#include "logger.h"
#include <zmq.hpp>

class zmqSocketAdapter : public abstractSocket {
private:
    zmq::socket_t& socket;
    bool is_bound = false;
    size_t port;
    logger _logger = _logger.Instance();
public:
    void setSocketPort(size_t socketPort);
    void setSocketAddress(std::string socketAddress);
    void setSocket(zmq::socket_t& new_socket);
    size_t socket_port;
    std::string socket_address;
    zmq::context_t context;
    void send(std::string payload) override;
    std::string recv() override;
    std::string interruptableRecv(bool &is_interrupt) override;
    void disconnect(std::string protocol, std::string address, size_t port = 0) override;
    void connect(std::string protocol, std::string address, size_t port = 0) override;
    void bind(std::string protocol, std::string address, size_t port = 0) override;
    void unbind(std::string protocol, std::string address, size_t port = 0) override;

    void close() override;

    void printOptions();

    bool isBound() override;

    zmqSocketAdapter(zmq::socket_t &socket);
};


#endif //VOTE_P2P_ZMQSOCKETADAPTER_H
