//
// Created by wld on 31.01.23.
//

#ifndef E_VOTING_DISTRIBUTIONSERVICE_H
#define E_VOTING_DISTRIBUTIONSERVICE_H


#include "../network/abstractSocket.h"
#include "logger.h"

class distributionService {

private:
    logger _logger = logger::Instance();

    void receiveInitial3PResponse(abstractSocket *socket, std::string address);

    void
    sendInitial3PRequest(abstractSocket *socket, size_t node_position, std::string address_up,
                         std::string address_down);

    void sendDirectionRequest(abstractSocket *forward_request_socket, nlohmann::json receiveJson, size_t node_position,
                              std::string address_down, std::string address_up);

    void receiveDirectionResponse(abstractSocket *forward_request_socket, std::string address);
};


#endif //E_VOTING_DISTRIBUTIONSERVICE_H
