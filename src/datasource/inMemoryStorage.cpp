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

std::set<did> inMemoryStorage::findAllDIDVersions(const did& id) const {
    std::set<did> all_did_versions;
    std::for_each(didStorage.begin(), didStorage.end(),[&id, &all_did_versions](std::pair<did, didDocument> entry){
        if(entry.first.withoutVersion() == id.withoutVersion()) {
            all_did_versions.insert(entry.first);
        }
    });
    return all_did_versions;
}

bool inMemoryStorage::existDIDInAnyVersion(did id) {
    std::set<did> version_agnostic_dids;
    std::transform(didStorage.begin(), didStorage.end(),std::inserter(version_agnostic_dids,version_agnostic_dids.begin()),[](std::pair<did, didDocument> entry){
        return entry.first.withoutVersion();
    });
    return version_agnostic_dids.contains(id.withoutVersion());
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
        if(entry.second.controller != entry.first.withoutVersion()){
            did_chain[entry.second.controller] = entry.first.withoutVersion();
        }
    });
    return did_chain;
}

std::map<did, did> inMemoryStorage::getReversedDIDChainDown() const {
    const std::map<did, did> &map = getDIDChainDown();
    std::map<did, did> reversed_map;
    std::for_each(map.begin(), map.end(),[&reversed_map](std::pair<did, did> entry){
        reversed_map[entry.second] = entry.first;
    });
    return reversed_map;
}

std::map<did, did> inMemoryStorage::getDIDChainUp(const did &origin_id) const {
    did controller = origin_id.withoutVersion();
    std::map<did, did> did_chain;

    while(!didStorage.at(controller).controller.method.empty() && didStorage.at(controller).controller != controller) {
        did_chain[controller.withoutVersion()] = didStorage.at(controller).controller;
        controller = didStorage.at(this->getLatest(controller)).controller;
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

bool inMemoryStorage::existsResource(did id) const {
    return resources.contains(id);
}

did inMemoryStorage::getLatest(const did& id) const {
    std::set<size_t> versions;
    std::set<did> dids = findAllDIDVersions(id);
    std::transform(dids.begin(), dids.end(), std::inserter(versions,versions.begin()), [](did id){
        return id.getVersion();
    });

    if(!versions.empty()) {
        size_t max_version = *versions.rbegin();
        return did{id.method, id.methodSpecifierIdentier}.withVersion(max_version);
    }
    throw std::invalid_argument( "versions empty, did does not exist" );
}
