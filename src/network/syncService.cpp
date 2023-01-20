//
// Created by wld on 19.01.23.
//

#include <nlohmann/json.hpp>
#include "syncService.h"
#include "abstractSocket.h"

void syncService::initSync(abstractSocket* socket, const std::string& initial_receiver_address){
    _logger.log("init sync to " + initial_receiver_address);

    socket->connect("tcp",std::string(initial_receiver_address),5556);
}

void syncService::sendInitialSyncRequest(abstractSocket* socket, const std::string& initial_receiver_address, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table){
    nlohmann::json send_json = nlohmann::json();
    send_json["receiverAddress"] = initial_receiver_address;
    send_json["nodes"] = nlohmann::ordered_json(peers);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    socket->send(send_json.dump());
    _logger.log("Has send json");
}

bool syncService::receiveSyncRequest(abstractSocket* socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table){

    socketMessage socketMessage = socket->recv();
    std::string receive_json = socketMessage.payload;
    _logger.log("received json");

    nlohmann::json receive_data = nlohmann::json::parse(receive_json);
    std::map<std::string, std::string> received_connections = receive_data["connections"].get<std::map<std::string, std::string>>();
    std::set<std::string> received_nodes = receive_data["nodes"].get<std::set<std::string>>();

    _logger.log("received sync response");
    peers = received_nodes;
    connection_table = received_connections;

    return true;
}

void syncService::sendSyncReply(abstractSocket* socket){
    socket->send("accepted");
    _logger.log("Sync complete");
}
