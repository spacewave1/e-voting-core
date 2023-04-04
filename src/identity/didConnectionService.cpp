//
// Created by wld on 14.03.23.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "didConnectionService.h"

void didConnectionService::computeConnectionReply(abstractSocket& socket, inMemoryStorage& storage, did own_id) {
    socketMessage message = socket.recv();
    did attemptConnection = did(message.payload);
    _logger.log("doc controller: " + attemptConnection.str());
    _logger.log("doc id: " + own_id.str());

    if (storage.existDID(attemptConnection)) {
        // Maybe check authentication
    } else {
        didDocument connectorDoc = identitiy_service.createDidDocument(own_id, attemptConnection);
        storage.addDocument(own_id, connectorDoc);
        storage.addResource(attemptConnection, message.addressFrom);
    }
}

int didConnectionService::computeConnectionRequest(socketMessage message, inMemoryStorage& storage, did own_id) {
    if (!message.payload.empty()) {
        did attemptConnection = did(message.payload);

        if (storage.existDID(attemptConnection)) {
            _logger.log("Check authentication");
            // Maybe check authentication
        } else {
            didDocument own_doc;
            if(storage.existDID(own_id)) {
                own_doc = storage.getDocument(own_id);
                own_doc.controllers.insert(attemptConnection);
            } else {
                own_doc = identitiy_service.createDidDocument(own_id, own_id);
            }
            didDocument connectorDoc = identitiy_service.createDidDocument(attemptConnection, own_id);

            storage.addDocument(own_id, own_doc);
            storage.addDocument(attemptConnection, connectorDoc);
            storage.addResource(attemptConnection, message.addressFrom);
            _logger.log("add did");
            return 0;
        }
    } else {
        _logger.log("Peer rejected, already ip address already belongs to a known peer");
        return 1;
    }
    return -1;
}

void didConnectionService::sendConnectionRequest(abstractSocket &socket, std::string &input) {
    socket.send(input);
}

int didConnectionService::receiveConnectionRequest(abstractSocket &socket,
                                                   inMemoryStorage& storage,
                                                   did own_id) {
    const socketMessage &socketMessage = socket.recv();
    return computeConnectionRequest(socketMessage, storage, own_id);
}

void didConnectionService::connect(abstractSocket &socket, const std::string &input,
                                   std::set<std::string> &known_peer_addresses, inMemoryStorage &storage) {
    socket.connect("tcp", input, 5555);
}

void didConnectionService::exportDidRegistry(std::string directory, const inMemoryStorage& storage, std::string file) {
    std::ofstream exportStream;
    exportStream.open(directory + file);
    nlohmann::json didRegistryJson = nlohmann::json();
    didRegistryJson["registry"] = nlohmann::ordered_json(storage.getDidStorage());
    exportStream << didRegistryJson.dump() << "\n";
    exportStream.close();
}

void didConnectionService::exportDidResources(const std::string directory, const inMemoryStorage& storage, std::string file) {
    std::ofstream exportStream;
    exportStream.open(directory + file);
    nlohmann::json resourcesJson = nlohmann::json();
    resourcesJson["resources"] = nlohmann::ordered_json(storage.getDidResources());
    exportStream << resourcesJson.dump() << "\n";
    exportStream.close();
}

void didConnectionService::sendConnectionSuccess(abstractSocket &socket, did controller) {
    std::stringstream sstream;
    sstream << controller;
    std::string controller_string = sstream.str();
    socket.send(controller_string);
}

void didConnectionService::importDidRegistry(inMemoryStorage& storage, std::string path, std::string file) {
    std::ifstream importStream;
    std::string line;

    _logger.log("is importing registry file from " + path + file);

    // File Open in the Read Mode
    importStream.open(path + file);
    
    std::map<std::string, std::string> registry_str;
    

    if (importStream.is_open()) {
        if (getline(importStream, line)) {
            nlohmann::json registryJson = nlohmann::json::parse(line);
            _logger.log("File contents: ");
            _logger.displayData(registryJson.dump());
            registry_str = registryJson["registry"].get<std::map<std::string, std::string>>();

            std::for_each(registry_str.begin(), registry_str.end(), [&storage, this](std::pair<std::string, std::string> entry){
                const did &id = did{entry.first};
                const didDocument document = identitiy_service.deserializeString(entry.second);
                std::stringstream sstream;
                sstream << document;
                _logger.log("added document" + sstream.str());
                storage.addDocument(id, document);
            });
        };
        // File Close
        importStream.close();
        _logger.log("Successfully imported registry");
    } else {
        _logger.error("Unable to open the file!");
    }
}

void didConnectionService::importDidResources(inMemoryStorage &storage, std::string path, std::string file) {
    std::ifstream importStream;
    std::string line;

    _logger.log("is importing resources file from " + path + file);

    // File Open in the Read Mode
    importStream.open(path + file);

    std::map<std::string, std::string> resources_str;


    if (importStream.is_open()) {
        if (getline(importStream, line)) {
            nlohmann::json resources_json = nlohmann::json::parse(line);
            _logger.log("File contents: ");
            _logger.displayData(resources_json.dump());
            resources_str = resources_json["resources"].get<std::map<std::string, std::string>>();

            std::for_each(resources_str.begin(), resources_str.end(), [&storage](std::pair<std::string, std::string> entry){
                const did &id = did{entry.first};
                storage.addResource(id, entry.second);
            });
        };
        // File Close
        importStream.close();
        _logger.log("Successfully imported resources");
    } else {
        _logger.error("Unable to open the file!");
    }
}


