//
// Created by wnabo on 03.01.2022.
//

#ifndef VOTE_P2P_ABSTRACTSOCKET_H
#define VOTE_P2P_ABSTRACTSOCKET_H

#include <string>

class abstractSocket {
public:
    virtual void send(std::string payload) = 0;
    virtual std::string recv() = 0;
    virtual std::string interruptableRecv(bool &is_interrupt) = 0;
    virtual void disconnect(std::string protocol, std::string address, size_t port = 0) = 0;
    virtual void bind(std::string protocol, std::string address, size_t port = 0) = 0;
    virtual void unbind(std::string protocol, std::string address, size_t port = 0) = 0;
    virtual void connect(std::string protocol, std::string address, size_t port = 0) = 0;
    virtual void close() = 0;
    virtual bool isBound() = 0;
};


#endif //VOTE_P2P_ABSTRACTSOCKET_H
