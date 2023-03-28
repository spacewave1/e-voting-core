//
// Created by wld on 01.12.22.
//

#ifndef E_VOTING_IDENTITYSERVICE_H
#define E_VOTING_IDENTITYSERVICE_H


#include <string>
#include "../evoting/logger.h"
#include "../../../did/src/did.h"
#include "../evoting/service/hashService.h"
#include "../../../did/src/didDocument.h"

class identityService {
public:
    did createLocalDid(time_t timestamp, std::string node_address, std::string nonce);
    didDocument createDidDocument(did identifier, did controller);
    didDocument deserializeString(std::string document_string);
private:
    hashService hash_service;
    logger _logger = logger::Instance();
};


#endif //E_VOTING_IDENTITYSERVICE_H
