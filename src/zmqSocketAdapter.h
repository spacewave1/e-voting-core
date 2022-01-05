//
// Created by wnabo on 03.01.2022.
//

#ifndef VOTE_P2P_ZMQSOCKETADAPTER_H
#define VOTE_P2P_ZMQSOCKETADAPTER_H

#include "abstractSocket.h"
#include <zmq.hpp>

class zmqSocketAdapter : public abstractSocket {
private:
    zmq::socket_t& socket;
public:
    void setSocketPort(size_t socketPort);
    void setSocketAddress(std::string socketAddress);
    void setSocket(zmq::socket_t& new_socket);
    size_t socket_port;
    std::string socket_address;
    zmq::context_t context;
    void send(std::string payload) override;
    std::string recv() override;
    void connect(std::string address, size_t port) override;
    void bind(std::string address, size_t port) override;
    void log(std::string address, std::string content);
    zmqSocketAdapter(zmq::socket_t &socket);
};


#endif //VOTE_P2P_ZMQSOCKETADAPTER_H
