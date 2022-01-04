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
    virtual void bind(std::string address, size_t port) = 0;
    virtual void connect(std::string address, size_t port) = 0;
};


#endif //VOTE_P2P_ABSTRACTSOCKET_H
