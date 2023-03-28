//
// Created by wld on 14.03.23.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "didConnectionService.h"

void didConnectionService::computeConnectionReply(abstractSocket& socket, inMemoryStorage& storage, did own_id) {
    socketMessage message = socket.recv();
    did attemptConnection = did(message.payload);

    if (storage.existDID(attemptConnection)) {
        // Maybe check authentication
    } else {
        didDocument connectorDoc = identitiy_service.createDidDocument(own_id, attemptConnection);
        storage.addDocument(attemptConnection, connectorDoc);
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
            didDocument connectorDoc = identitiy_service.createDidDocument(attemptConnection, own_id);
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

void didConnectionService::exportDidRegistry(std::string directory, inMemoryStorage storage, std::string file) {
    std::ofstream exportStream;
    exportStream.open(directory + file);
    nlohmann::json connectionsJson = nlohmann::json();
    connectionsJson["dids"] = nlohmann::ordered_json(storage.getDidStorage());
    exportStream << connectionsJson.dump() << "\n";
    exportStream.close();
}

void didConnectionService::sendConnectionSuccess(abstractSocket &socket, did controller) {
    std::stringstream sstream;
    sstream << controller;
    std::string controller_string = sstream.str();
    socket.send(controller_string);
}


