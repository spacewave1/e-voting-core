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

bool syncService::receiveSyncRequest(abstractSocket& socket, std::set<std::string>& peers, std::map<std::string, std::string>& connection_table){
    socketMessage socketMessage = socket.recv();
    std::string receive_json = socketMessage.payload;
    _logger.log("received json");
    _logger.log(receive_json);
    nlohmann::json receive_data = nlohmann::json::parse(receive_json);
    _logger.log("parsed json");
    std::map<std::string, std::string> received_connections = receive_data["connections"].get<std::map<std::string, std::string>>();
    std::set<std::string> received_nodes = receive_data["nodes"].get<std::set<std::string>>();

    _logger.log("received sync response");

    std::for_each(received_connections.begin(), received_connections.end(),
                  [&connection_table, this](const std::pair<std::string, std::string> pair) {
                      if (!connection_table.contains(pair.first)) {
                          _logger.log("add");
                          _logger.log(pair.first + "->" + pair.second);
                          connection_table[pair.first] = pair.second;
                      }
                  });
    peers.insert(received_nodes.begin(), received_nodes.end());

    _logger.log("now return true");
    return true;
}

void syncService::sendSyncReply(abstractSocket* socket){
    socket->send("accepted");
    _logger.log("Sync operation complete");
}

void syncService::forwardConnectSync(abstractSocket* socket, const std::string& next_address, int port) {
    _logger.log("forward sync to " + next_address);

    socket->connect("tcp",std::string(next_address), port);
}

void syncService::forwardSyncRequestUp(abstractSocket* socket,
                                       std::set<std::string>& peers,
                                       std::map<std::string, std::string>& connection_table,
                                       std::string next_receiver_address,
                                       std::string received_from_address) {


    /*
    nlohmann::json send_json = nlohmann::json();
    send_json["receiverAddress"] = next_receiver_address;
    send_json["nodes"] = nlohmann::ordered_json(peers);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    socket->send(send_json.dump());
    _logger.log("Has send json");
     */

    _logger.log("Merged Data");

    if (peers.contains(received_from_address)) {
        // Forward Data in root direction

        _logger.log("connects to " + std::string(next_receiver_address));

        //socket->connect("tcp://", std::string(next_receiver_address), 5556);

        nlohmann::json send_json;
        send_json["nodes"] = nlohmann::ordered_json(peers);
        send_json["connections"] = nlohmann::ordered_json(connection_table);
        send_json["receiverAddress"] = next_receiver_address;

        _logger.log("send: " + send_json.dump());

        // Send the next receiver the updated nodes
        socket->send(send_json.dump());

        _logger.log("Wait for json answer from next node in root direction");
    } else {
        _logger.log("did not contain received from address");
        _logger.log(received_from_address);

        std::for_each(connection_table.begin(), connection_table.end(), [this](std::pair<std::string, std::string> connection){
            _logger.log(connection.first + "->" + connection.second);
        });
        std::for_each(peers.begin(), peers.end(), [this](std::string peer){
            _logger.log(peer);
        });

    }
}

void syncService::receiveReturnSyncFromUp(abstractSocket* socket,
                                           std::set<std::string>& peers,
                                           std::map<std::string, std::string>& connection_table){
    socketMessage message = socket->recv();

    _logger.log("has send response: " + message.payload, message.addressFrom);

    nlohmann::json received_data = nlohmann::json::parse(message.payload);

    connection_table = received_data["connections"].get<std::map<std::string, std::string>>();
    peers = received_data["nodes"].get<std::set<std::string>>();
}

void syncService::returnSyncRequestDown(abstractSocket *socket,
                                        std::set<std::string> &peers,
                                        std::map<std::string, std::string> &connection_table,
                                        std::string local_address) {

    std::map<std::string, std::set<std::string>> reversed_connection_table;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversed_connection_table](std::pair<std::string, std::string> pair) {
                      reversed_connection_table[pair.second].insert(pair.first);
                  });

    nlohmann::json send_json;
    send_json["nodes"] = nlohmann::ordered_json(peers);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    _logger.log("sending json: " + send_json.dump());

    std::for_each(reversed_connection_table[local_address].begin(), reversed_connection_table[local_address].end(),
                  [socket, &send_json, this](const std::string& peer_address) {
                      this->_logger.log("send data to " + peer_address);
                      socket->connect("tcp",peer_address, 5557);
                  });
}

void syncService::returnSyncRequestDownSendData(abstractSocket *socket,
                                        std::set<std::string> &peers,
                                        std::map<std::string, std::string> &connection_table,
                                        std::string local_address) {

    std::map<std::string, std::set<std::string>> reversed_connection_table;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversed_connection_table](std::pair<std::string, std::string> pair) {
                      reversed_connection_table[pair.second].insert(pair.first);
                  });

    nlohmann::json send_json;
    send_json["nodes"] = nlohmann::ordered_json(peers);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    _logger.log("sending json: " + send_json.dump());

    std::for_each(reversed_connection_table[local_address].begin(), reversed_connection_table[local_address].end(),
                  [socket, &send_json, this](const std::string& peer_address) {
                      socket->send(send_json.dump());
                  });
}

