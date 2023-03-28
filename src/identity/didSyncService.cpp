//
// Created by wld on 16.03.23.
//

#include <nlohmann/json.hpp>
#include "didSyncService.h"
#include <iostream>

void didSyncService::sendInitialSyncRequest(abstractSocket *socket, did initial_receiver_id, inMemoryStorage& storage) {
    nlohmann::json send_json = nlohmann::json();
    std::string initial_receiver_address = storage.fetchResource(initial_receiver_id);

    send_json["receiverAddress"] = initial_receiver_address;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    _logger.log(send_json.dump());
    socket->send(send_json.dump());
    _logger.log("Has send json");
}

void didSyncService::initSync(abstractSocket *socket, did initial_id, inMemoryStorage& storage) {
    // TODO: Dereference to get address
    const std::string &initial_receiver_address = storage.fetchResource(initial_id);
    
    _logger.log("init sync to " + initial_receiver_address);
    socket->connect("tcp",std::string(initial_receiver_address),5556);
}

void
didSyncService::forwardSyncRequestUp(abstractSocket *socket, inMemoryStorage &storage,
                                     did next_did, std::string from_address) {
    _logger.log("Merged Data");
    const std::string &address_hash = hash_service.hashMessage(from_address);
    std::vector<did> did_matches = storage.findAddressMatch(address_hash);
    //TODO: Further checks
    if (!did_matches.empty()) {
        // Forward Data in root direction
        const std::string &next_address = storage.fetchResource(next_did);
        _logger.log("connects to " + std::string(next_address));

        //socket->connect("tcp://", std::string(next_receiver_address), 5556);

        nlohmann::json send_json;
        send_json["receiverAddress"] = next_address;
        send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
        send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

        _logger.log("send: " + send_json.dump());

        // Send the next receiver the updated nodes
        socket->send(send_json.dump());

        _logger.log("Wait for json answer from next node in root direction");
    } else {
        _logger.log("did not contain received from address");
        _logger.log(from_address);
    }

}

void
didSyncService::returnSyncRequestDown(abstractSocket *socket, std::set<std::string> &peers, inMemoryStorage &storage,
                                      did own_id) {
    
    std::map<did, did> didChain = storage.getDIDControllerChain();
    std::map<did, std::set<did>> reversed_did_chain;
    
    std::for_each(didChain.begin(), didChain.end(),
                  [&reversed_did_chain](std::pair<did, did> pair) {
                      reversed_did_chain[pair.second].insert(pair.first);
                  });

    nlohmann::json send_json;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    _logger.log("sending json: " + send_json.dump());

    std::for_each(reversed_did_chain[own_id].begin(), reversed_did_chain[own_id].end(),
                  [socket, &send_json, this, &storage](const did& id) {
                      const std::string &address = storage.fetchResource(id);
                              this->_logger.log("send data to " + address);
                      socket->connect("tcp",address, 5557);
                  });
}


void
didSyncService::returnSyncRequestDownData(abstractSocket *socket, std::set<std::string> &peers, inMemoryStorage &storage,
                                      did own_id) {

    std::map<did, did> didChain = storage.getDIDControllerChain();
    std::map<did, std::set<did>> reversed_did_chain;

    std::for_each(didChain.begin(), didChain.end(),
                  [&reversed_did_chain](std::pair<did, did> pair) {
                      reversed_did_chain[pair.second].insert(pair.first);
                  });

    nlohmann::json send_json;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    _logger.log("sending json: " + send_json.dump());

    std::for_each(reversed_did_chain[own_id].begin(), reversed_did_chain[own_id].end(),
                  [socket, &send_json, this, &storage](const did& id) {
                      const std::string &address = storage.fetchResource(id);
                      this->_logger.log("send data to " + address);
                      socket->send(send_json.dump());
                  });
}

void didSyncService::forwardConnectSync(abstractSocket *socket, did next_did, inMemoryStorage& storage) {
    const std::string &next_address = storage.fetchResource(next_did);
    _logger.log("forward sync to " + next_address);
    socket->connect("tcp",std::string(next_address),5556);
}

bool
didSyncService::receiveSyncRequest(abstractSocket &socket, inMemoryStorage &storage) {
    socketMessage socketMessage = socket.recv();
    std::string receive_json = socketMessage.payload;
    _logger.log(socketMessage.payload);
    _logger.log("received json");
    _logger.log(receive_json);
    nlohmann::json receive_data = nlohmann::json::parse(receive_json);
    _logger.log("parsed json");

    nlohmann::json send_json;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    std::map<std::string, std::string> received_did_documents = receive_data["didDocuments"].get<std::map<std::string, std::string>>();
    std::map<did, didDocument> received_did_storage;

    std::for_each(received_did_documents.begin(), received_did_documents.end(), [&received_did_storage, this](std::pair<std::string, std::string> entry){
        nlohmann::json json = nlohmann::json::parse(entry.second);
        const didDocument &document = identity_service.deserializeString(entry.second);
        received_did_storage[did(entry.first)] = document;
    });

    std::map<std::string, std::string> received_resources = receive_data["didResources"].get<std::map<std::string, std::string>>();
    std::map<did, std::string> received_did_resources;

    std::for_each(received_resources.begin(), received_resources.end(), [&received_did_resources](std::pair<std::string, std::string> entry){
        received_did_resources[did(entry.first)] = entry.second;
    });

    _logger.log("received sync response");

    std::for_each(received_did_storage.begin(), received_did_storage.end(),
                  [&storage, this](const std::pair<did, didDocument> pair) {
                      if (!storage.existDID(pair.first)) {
                          _logger.log("add resource");
                          std::stringstream sstringstream;
                          sstringstream << pair.first << "->" << pair.second;
                          _logger.log(sstringstream.str());
                          storage.addDocument(pair.first, pair.second);
                      }
                  });
    _logger.log("now return true");
    return true;
}

void didSyncService::sendSyncReply(abstractSocket *socket) {
    socket->send("accepted");
}
