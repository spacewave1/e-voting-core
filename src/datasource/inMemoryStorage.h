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
    std::set<did> getAllDIDs() const;
    std::set<did> getLastestDids() const;
    didDocument getDocument(did id) const;
    std::map<std::string, std::string> getDidStorage() const;
    std::map<std::string, std::string> getDidResources() const;
    std::string fetchResource(did input_id) const;

    std::vector<did> findAddressMatch(std::string address_hash);
    std::map<did, did> getDIDChainDown() const;
    std::map<did, did> getReversedDIDChainDown() const;
    std::map<did, did> getDIDChainUp(const did &origin_id) const;

    bool hasIdDown(did id);

    bool existsResource(did id) const;

    bool existDIDInAnyVersion(did id);

    std::set<did> findAllDIDVersions(const did &id) const;

    did getLatest(const did& id) const;

    size_t removeDocument(did& id);
};


#endif //E_VOTING_INMEMORYSTORAGE_H
