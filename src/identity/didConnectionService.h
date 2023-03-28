//
// Created by wld on 14.03.23.
//

#ifndef E_VOTING_DIDCONNECTIONSERVICE_H
#define E_VOTING_DIDCONNECTIONSERVICE_H


#include <set>
#include <map>
#include "../network/socketMessage.h"
#include "../../../did/src/did.h"
#include "identityService.h"
#include "../datasource/inMemoryStorage.h"
#include "../network/abstractSocket.h"

class didConnectionService {
public:
    int computeConnectionRequest(socketMessage message, inMemoryStorage& storage, did own_id);
    int receiveConnectionRequest(abstractSocket& socket, inMemoryStorage& storage, did own_id);
    void sendConnectionRequest(abstractSocket& socket, std::string &input);
    void sendConnectionSuccess(abstractSocket& socket, did controller);
    void computeConnectionReply(abstractSocket& socket, inMemoryStorage& storage, did own_id);
    void connect(abstractSocket& socket, const std::string& input, std::set<std::string>& known_peer_addresses, inMemoryStorage& storage);
    void exportDidRegistry(std::string directory, inMemoryStorage storage, std::string file);

private:
    identityService identitiy_service;
    logger _logger = logger::Instance();

};


#endif //E_VOTING_DIDCONNECTIONSERVICE_H
