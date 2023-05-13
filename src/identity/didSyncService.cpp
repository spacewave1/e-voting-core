//
// Created by wld on 16.03.23.
//

#include <nlohmann/json.hpp>
#include "didSyncService.h"
#include <iostream>

void didSyncService::sendInitialSyncRequest(abstractSocket *socket, did initial_receiver_id, inMemoryStorage& storage) {
    nlohmann::json send_json = nlohmann::json();
    std::string initial_receiver_address = storage.fetchResource(initial_receiver_id);

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
didSyncService::returnSyncRequestDown(abstractSocket *socket, inMemoryStorage &storage,
                                      did own_id) {
    
    std::map<did, did> didChain = storage.getDIDChainDown();

    _logger.log("didChain size: " + std::to_string(didChain.size()));

    nlohmann::json send_json;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    _logger.log("sending return json: " + send_json.dump());

    did connect_id = didChain[own_id];

    const std::string &address = storage.fetchResource(connect_id.withoutVersion());
    this->_logger.log("send data to " + address);
    socket->connect("tcp",address, 5557);

    //TODO: When having hirarchy instead of chain
    /*std::for_each(didChain[own_id].begin(), didChain[own_id].end(),
                  [socket, this, &storage](const did& id) {
                      const std::string &address = storage.fetchResource(id);
                              this->_logger.log("send data to " + address);
                      socket->connect("tcp",address, 5557);
                  });
    */
}


void
didSyncService::returnSyncRequestDownData(abstractSocket *socket, inMemoryStorage &storage,
                                      did own_id) {

    std::map<did, did> didChain = storage.getDIDChainDown();

    _logger.log("didChain size: " + std::to_string(didChain.size()));

    nlohmann::json send_json;
    send_json["didDocuments"] = nlohmann::ordered_json(storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(storage.getDidResources());

    _logger.log("sending return json: " + send_json.dump());

    did connect_id = didChain[own_id];
    const std::string &address = storage.fetchResource(connect_id);
    this->_logger.log("send data to " + address);
    socket->send(send_json.dump());

    //TODO: When having hirarchy instead of chain
    /*std::for_each(didChain[own_id].begin(), didChain[own_id].end(),
                  [socket, this, &storage](const did& id) {
                      const std::string &address = storage.fetchResource(id);
                              this->_logger.log("send data to " + address);
                      socket->send("tcp",address, 5557);
                  });
    */
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
        std::stringstream sstream;
        sstream << document;
        _logger.log(did(entry.first).str() + "->" + sstream.str());
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
                      if (storage.existDIDInAnyVersion(pair.first)) {
                          did most_recent_did = storage.getLatest(pair.first);
                          if(most_recent_did.getVersion() == 1 && pair.first.getVersion() > 1){
                              storage.addDocument(pair.first, pair.second);
                          }
                      } else {
                          _logger.log("add resource");
                          std::stringstream sstringstream;
                          sstringstream << pair.first << "->" << pair.second;
                          _logger.log(sstringstream.str());
                          storage.addDocument(pair.first, pair.second);
                      }
                  });

    std::for_each(received_did_resources.begin(), received_did_resources.end(), [&storage, this](std::pair<did, std::string> entry){
       if(!storage.existsResource(entry.first.withoutVersion())){
           storage.addResource(entry.first.withoutVersion(), entry.second);
       }
    });
    _logger.log("now return true");
    return true;
}

void didSyncService::sendSyncReply(abstractSocket *socket) {
    socket->send("accepted");
}
