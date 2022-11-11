//
// Created by wnabo on 02.01.2022.
//

#ifndef VOTE_P2P_MOCKSOCKET_H
#define VOTE_P2P_MOCKSOCKET_H

#include <gmock/gmock.h>

class mockSocket : public abstractSocket {
public :
    MOCK_METHOD(void, send, (std::string payload), (override));
    MOCK_METHOD(std::string, recv, (), (override));
    MOCK_METHOD(std::string, interruptableRect, (bool &is_interrupt), (override));
    MOCK_METHOD(void, bind, (std::string protocol, std::string address, size_t port), (override));
    MOCK_METHOD(void, connect, (std::string protocol, std::string address, size_t port), (override));
    MOCK_METHOD(void, disconnect, (std::string protocol, std::string, size_t port), (override));
    MOCK_METHOD(void, unbind, (std::string protocol, std::string address, size_t port), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(bool, isBound, (), (override));

    mockSocket() = default;
};


#endif //VOTE_P2P_MOCKSOCKET_H
