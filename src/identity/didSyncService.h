//
// Created by wld on 16.03.23.
//

#ifndef E_VOTING_DIDSYNCSERVICE_H
#define E_VOTING_DIDSYNCSERVICE_H


#include "../network/abstractSocket.h"
#include "../../../did/src/did.h"
#include "../datasource/inMemoryStorage.h"
#include "../evoting/logger.h"
#include "../evoting/service/hashService.h"
#include "identityService.h"

class didSyncService {
public:
    void initSync(abstractSocket* socket, did initial_receiver_id, inMemoryStorage& storage);
    void sendInitialSyncRequest(abstractSocket* socket, did initial_receiver_id, inMemoryStorage& storage);
    void forwardSyncRequestUp(abstractSocket* socket, inMemoryStorage& storage, did next_did, std::string from_address);
    void returnSyncRequestDown(abstractSocket* socket, inMemoryStorage& storage, did own_id);
    void forwardConnectSync(abstractSocket *socket, did next_did, inMemoryStorage& storage);

    bool receiveSyncRequest(abstractSocket &socket, inMemoryStorage &storage);
    void sendSyncReply(abstractSocket* socket);

    void returnSyncRequestDownData(abstractSocket *socket, inMemoryStorage &storage,
                              did own_id);

private:
    logger _logger = logger::Instance();
    hashService hash_service;
    identityService identity_service;

};


#endif //E_VOTING_DIDSYNCSERVICE_H
