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
    MOCK_METHOD(void, bind, (std::string address, size_t port), (override));
    MOCK_METHOD(void, connect, (std::string address, size_t port), (override));
    MOCK_METHOD(void, unbind, (std::string address, size_t port), (override));
    MOCK_METHOD(bool, isBound, (), (override));

    mockSocket() {

    }
};


#endif //VOTE_P2P_MOCKSOCKET_H
