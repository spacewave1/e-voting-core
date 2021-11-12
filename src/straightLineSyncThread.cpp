//
// Created by wnabo on 09.11.2021.
//

#include "straightLineSyncThread.h"
#include <iostream>
#include "zmq.hpp"
#include "nlohmann/json.hpp"
#include <regex>
#include <set>
#include <map>


void straightLineSyncThread::InternalThreadEntry() {
    std::cout << "check initial receiver address" << std::endl;
    std::cout << initial_receiver_address << std::endl;

    if (!initial_receiver_address.empty()) {
        initSyncProcedure();
    } else {
        syncForwardProcedure();
    }
}

void straightLineSyncThread::initSyncProcedure() {
    std::cout << "init sync to" << initial_receiver_address << std::endl;

    zmq::context_t *context = (zmq::context_t *) arg;
    zmq::socket_t sock(*context, ZMQ_REQ);

    sock.connect("tcp://" + std::string(initial_receiver_address) + ":5556");

    nlohmann::json send_json = nlohmann::json();
    send_json["receiverAddress"] = initial_receiver_address;
    send_json["nodes"] = nlohmann::ordered_json(peers);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    sock.send(zmq::message_t(nlohmann::to_string(send_json)), zmq::send_flags::none);
    std::cout << "Has send json" << std::endl;

    zmq::message_t json_data;
    sock.recv(json_data, zmq::recv_flags::none);
    std::cout << "Received Json" << std::endl;

    nlohmann::json receive_data = nlohmann::json::parse(json_data.to_string());
    std::map<std::string, std::string> received_connections = receive_data["connections"].get<std::map<std::string, std::string>>();
    std::set<std::string> received_nodes = receive_data["nodes"].get<std::set<std::string>>();

    peers = received_nodes;
    connection_table = received_connections;

    sock.send(zmq::str_buffer("accepted"));
    std::cout << "Sync complete" << std::endl;
}

void straightLineSyncThread::syncForwardProcedure() {
    std::cout << "Forward Data to sync" << std::endl;
    zmq::context_t *context = (zmq::context_t *) arg;
    zmq::message_t request;

    zmq::socket_t toward_branch_end_socket(*context, ZMQ_REP);
    zmq::socket_t toward_root_socket(*context, ZMQ_REQ);

    toward_branch_end_socket.bind("tcp://*:5556");


    toward_branch_end_socket.recv(request, zmq::recv_flags::none);
    std::cout << "Received: " << request.to_string() << std::endl;
    std::cout << "Parse JSON from edge" << request.to_string() << std::endl;
    nlohmann::json received_data = nlohmann::json::parse(request.to_string());
    std::string from_address = std::string(received_data["receiverAddress"]);


    std::cout << "Received sync request: [" << request.to_string() << "]" << std::endl;
    std::cout << "Parsed Json: [" << received_data.dump() << "]" << std::endl;

    if (std::regex_match(from_address, std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {

        // Merge data here
        std::map<std::string, std::string> received_connections = received_data["connections"].get<std::map<std::string, std::string>>();
        std::set<std::string> received_nodes = received_data["nodes"].get<std::set<std::string>>();

        std::cout << "Placed received json into variables" << std::endl;

        std::for_each(connection_table.begin(), connection_table.end(),
                      [&received_connections](const std::pair<std::string, std::string> pair) {
                          if (!received_connections.contains(pair.first)) {
                              received_connections[pair.first] = pair.second;
                          }
                      });
        received_nodes.insert(peers.begin(), peers.end());

        std::cout << "Merged Data here" << std::endl;

        std::cout << "Check if from address contained in connection table" << std::endl;
        if (connection_table.contains(from_address)) {
            // Forward Data in root direction

            std::string next_receiver = connection_table[from_address];

            std::cout << "Connect towards the next node in root direction" << std::endl;
            toward_root_socket.connect("tcp://" + std::string(next_receiver) + ":5556");

            nlohmann::json send_json;
            send_json["nodes"] = nlohmann::ordered_json(received_nodes);
            send_json["connections"] = nlohmann::ordered_json(received_connections);
            send_json["receiverAddress"] = next_receiver;

            std::cout << "Send the following json: " << std::endl;
            std::cout << send_json.dump() << std::endl;

            // Send the next receiver the updated nodes
            toward_root_socket.send(zmq::message_t(send_json.dump()), zmq::send_flags::none);

            // Receive the data from root direction
            zmq::message_t reply;
            std::cout << "Wait for json answer from next node in root direction: " << std::endl;
            toward_root_socket.recv(reply);

            std::cout << "Response: " << reply.to_string() << std::endl;

            received_data = nlohmann::json::parse(reply.to_string());
            received_connections = received_data["connections"].get<std::map<std::string, std::string>>();
            received_nodes = received_data["nodes"].get<std::set<std::string>>();

            connection_table = received_connections;
            peers = received_nodes;

        } else {
            // Forward to branches
            // TODO: Work over this piece, it is data access in secondary thread
            connection_table = received_connections;
            peers = received_nodes;
        }

        std::map<std::string, std::set<std::string>> reversed_connection_table;
        std::for_each(connection_table.begin(), connection_table.end(),
                      [&reversed_connection_table](std::pair<std::string, std::string> pair) {
                          reversed_connection_table[pair.second].insert(pair.first);
                      });

        nlohmann::json send_json;
        send_json["nodes"] = nlohmann::ordered_json(peers);
        send_json["connections"] = nlohmann::ordered_json(connection_table);

        std::cout << "send json: " << send_json.dump() << std::endl;

        std::for_each(reversed_connection_table[from_address].begin(),
                      reversed_connection_table[from_address].end(),
                      [&toward_branch_end_socket, &send_json](const std::string peer_address) {
                          std::cout << "send data to " << peer_address << std::endl;
                          toward_branch_end_socket.connect("tcp://" + peer_address + ":5556");
                          toward_branch_end_socket.send(zmq::message_t(send_json.dump()), zmq::send_flags::none);
                          zmq::message_t reply;
                          toward_branch_end_socket.recv(reply, zmq::recv_flags::none);
                          std::cout << reply.to_string() << std::endl;

                          toward_branch_end_socket.close();
                      });

        if(connection_table.contains(from_address)) {
            toward_root_socket.send(zmq::message_t("accepted"), zmq::send_flags::none);
        }

        // If no more addresses in table the top is reached of the connection hierarchy, its the turning point
        // Merge and send downwards

    }

}

void straightLineSyncThread::setParams(void *arg, std::map<std::string, std::string> connection_table,
                                       std::set<std::string> peers, std::string initial_receiver_address) {
    this->arg = arg;
    this->peers = peers;
    this->connection_table = connection_table;
    this->initial_receiver_address = initial_receiver_address;
}

const std::map <std::string, std::string> &straightLineSyncThread::getConnectionTable() const {
    return connection_table;
}

const std::set<std::string> &straightLineSyncThread::getPeers() const {
    return peers;
}
