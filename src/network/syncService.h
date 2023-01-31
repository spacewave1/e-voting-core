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
    bool receiveSyncRequest(abstractSocket& socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table);

    void sendSyncReply(abstractSocket* socket);

    void forwardSyncRequestUp(abstractSocket* socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table, std::string next_receiver_address, std::string received_from_address);
    void returnSyncRequestDown(abstractSocket* socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table, std::string local_address);
    void forwardConnectSync(abstractSocket *socket, const std::string &next_address);

private:
    logger _logger = logger::Instance();

    void receiveReturnSyncFromUp(abstractSocket* socket, std::set<std::string> &peers,
                                  std::map<std::string, std::string> &connection_table);
};


#endif //E_VOTING_SYNCSERVICE_H
