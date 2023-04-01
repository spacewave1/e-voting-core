//
// Created by wld on 14.03.23.
//

#ifndef E_VOTING_INMEMORYSTORAGE_H
#define E_VOTING_INMEMORYSTORAGE_H


#include <map>
#include "../../../did/src/did.h"
#include "../../../did/src/didDocument.h"

class inMemoryStorage {
private:
    std::map<did, didDocument> didStorage;
    std::map<did, std::string> resources;
public:
    int addDocument(did id, didDocument doc);
    bool existDID(did id);
    bool addResource(did id, std::string input);
    std::set<did> getAllDIDs();
    std::map<std::string, std::string> getDidStorage();
    std::map<std::string, std::string> getDidResources();
    std::string fetchResource(did inputId);

    std::vector<did> findAddressMatch(std::string address_hash);

    std::map<did, did> getDIDChainDown();
    std::map<did, did> getDIDChainUp(const did &origin_id);

    bool hasIdDown(did id);
};


#endif //E_VOTING_INMEMORYSTORAGE_H
