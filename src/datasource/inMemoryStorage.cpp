//
// Created by wld on 14.03.23.
//

#include "inMemoryStorage.h"
#include <algorithm>
#include <set>

int inMemoryStorage::addDocument(did id, didDocument doc) {
    bool result = didStorage.try_emplace(id, doc).second;
    if (result) {
        return 0;
    }
    return -1;
}

bool inMemoryStorage::existDID(did id) {
    return didStorage.contains(id);
}

std::set<did> inMemoryStorage::getAllDIDs() {
    std::set<did> dids;
    std::for_each(didStorage.begin(), didStorage.end(),
                  [&dids](const std::pair<did, didDocument> &pair) { dids.emplace(pair.first); });
    return dids;
}

std::map<std::string, std::string> inMemoryStorage::getDidResources() const {
    std::map<did, std::string> map = resources;
    std::map<std::string, std::string> serialized;
    std::for_each(map.begin(), map.end(), [&serialized](std::pair<did, std::string> entry) {
        std::stringstream didStream;
        didStream << entry.first;
        serialized[didStream.str()] = entry.second;
    });
    return serialized;
}

std::map<std::string, std::string> inMemoryStorage::getDidStorage() const {
    std::map<did, didDocument> map = didStorage;
    std::map<std::string, std::string> serialized;
    std::for_each(map.begin(), map.end(), [&serialized](std::pair<did, didDocument> entry) {
        std::stringstream didStream;
        std::stringstream docStream;
        didStream << entry.first;
        docStream << entry.second;
        serialized[didStream.str()] = docStream.str();
    });
    return serialized;
}

std::string inMemoryStorage::fetchResource(did input_id) const {
    return resources.at(input_id);
}

bool inMemoryStorage::addResource(did id, std::string input) {
    try {
        resources[id] = input;
        return true;
    } catch (std::exception ex) {
        return false;
    }
}

std::vector<did> inMemoryStorage::findAddressMatch(std::string address_hash) {
    const std::set<did> did_set = getAllDIDs();
    std::vector<did> did_matches;

    std::for_each(did_set.begin(), did_set.end(), [&did_matches, address_hash](did id){
        if(id.methodSpecifierIdentier.find(address_hash) != std::string::npos){
            did_matches.push_back(id);
        }
    });
    return did_matches;
}

std::map<did, did> inMemoryStorage::getDIDChainDown() const {
    std::map<did, did> did_chain;
    std::for_each(didStorage.begin(), didStorage.end(),[&did_chain](std::pair<did, didDocument> entry){
        if(entry.second.controller != entry.first){
            did_chain[entry.second.controller] = entry.first;
        }
    });
    return did_chain;
}

std::map<did, did> inMemoryStorage::getDIDChainUp(const did &origin_id) const {
    did controller = origin_id;
    std::map<did, did> did_chain;
    while(!didStorage.at(controller).controller.method.empty() && didStorage.at(controller).controller != controller) {
        did_chain[controller] = didStorage.at(controller).controller;
        controller = didStorage.at(controller).controller;
    }
    return did_chain;
}

bool inMemoryStorage::hasIdDown(did id) {
    bool idDownExists = false;
    std::for_each(didStorage.begin(), didStorage.end(),[&id, &idDownExists](std::pair<did, didDocument> entry){
        if(entry.second.controller == id && entry.second.controller != entry.first){
            idDownExists = true;
        }
    });
    return idDownExists;
}

didDocument inMemoryStorage::getDocument(did id) const {
    return didStorage.at(id);
}

std::shared_ptr<inMemoryStorage> inMemoryStorage::getPtr() {
    return _ptr;
}

bool inMemoryStorage::existsResource(did id) {
    return resources.contains(id);
}
