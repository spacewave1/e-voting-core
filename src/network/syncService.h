//
// Created by wld on 19.01.23.
//

#ifndef E_VOTING_SYNCSERVICE_H
#define E_VOTING_SYNCSERVICE_H


#include "../evoting/logger.h"
#include "abstractSocket.h"
#include <set>

class syncService {
public:
    void initSync(abstractSocket* socket, const std::string& initial_receiver_address);
    void sendInitialSyncRequest(abstractSocket* socket, const std::string& initial_receiver_address, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table);
    bool receiveSyncRequest(abstractSocket* socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table);
    void sendSyncReply(abstractSocket* socket);
private:
    logger _logger = logger::Instance();
};


#endif //E_VOTING_SYNCSERVICE_H
