//
// Created by wld on 01.12.22.
//

#include "connectionService.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include "abstractSocket.h"

void connectionService::exportPeerConnections(std::string exportPath, std::map<std::string, std::string> connection_table) {
    std::ofstream exportStream;
    exportStream.open(exportPath + "connections.json");
    nlohmann::json connectionsJson = nlohmann::json();
    connectionsJson["connections"] = nlohmann::ordered_json(connection_table);
    exportStream << connectionsJson.dump() << "\n";
    exportStream.close();
}

void connectionService::exportPeersList(std::string exportPath, std::set<std::string> known_peer_addresses, std::string exportFile) {
    std::ofstream exportStream;
    exportStream.open(exportPath + exportFile);
    nlohmann::json peersJson = nlohmann::json();
    peersJson["peers"] = nlohmann::ordered_json(known_peer_addresses);
    exportStream << peersJson.dump() << "\n";
    exportStream.close();
}

std::map<std::string, std::string>& connectionService::importPeerConnections(std::map<std::string, std::string>& connection_table, std::string importPath) {
    std::ifstream importStream;
    std::string line;

    _logger.log("is importing connections file from " + importPath + "connections.json");

    // File Open in the Read Mode
    importStream.open(importPath + "connections.json");

    if (importStream.is_open()) {
        if (getline(importStream, line)) {

            nlohmann::json connectionsJson = nlohmann::json::parse(line);
            _logger.log("File contents: ");
            _logger.displayData(connectionsJson.dump());
            connection_table = connectionsJson["connections"].get<std::map<std::string, std::string>>();

        };
        // File Close
        importStream.close();
        _logger.log("Successfully imported connections");
    } else {
        _logger.error("Unable to open the file!");
    }
}

std::set<std::string>& connectionService::importPeersList(std::set<std::string> &peer_addresses, std::string importPath) {
    std::ifstream importStream;
    std::string line;

     _logger.log("is importing peers file from " + importPath + "peers.json");

    // File Open in the Read Mode
    importStream.open(importPath + "peers.json");

    if (importStream.is_open()) {
        if (getline(importStream, line)) {
            _logger.displayData(line);
            nlohmann::json peersJson = nlohmann::json::parse(line);
            _logger.log("File contents as json: ");
            _logger.displayData(peersJson.dump());
            peer_addresses = peersJson["peers"].get<std::set<std::string>>();

        };
        // File Close
        importStream.close();
        _logger.log("Successfully imported connections");
    } else {
        _logger.error("Unable to open the file!");
    }
    return peer_addresses;
}

void connectionService::connect(abstractSocket& socket, const std::string& input, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table) {
    //socket.connect("tcp", input, 5555);
    socket.send(input);
}

std::string connectionService::createNetworkRegistrationRequest(std::string connectToAddress) {
    return connectToAddress;
}

std::string connectionService::createOnBoardingReply(std::string connectFromAddress) {
    return connectFromAddress;
}

void connectionService::connect(std::string &input, void *abstractContext, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table, std::string& peer_address) {
    _logger.log("is connecting to " + input);
    const std::string delimiter = " ";
    size_t position_of_whitespace = input.find(delimiter);
    auto address = input.substr(position_of_whitespace + delimiter.size(), input.size() - position_of_whitespace);

    zmq::context_t *zmq_context = (zmq::context_t *) abstractContext;
    zmq::socket_t sock(*zmq_context, zmq::socket_type::req);

    sock.connect("tcp://" + std::string(address) + ":5555");

    sock.send(zmq::message_t(address), zmq::send_flags::none);

    zmq::message_t reply;
    sock.recv(reply, zmq::recv_flags::none);
    if (!reply.empty()) {
        _logger.log("has replied" + reply.to_string(), reply.gets("Peer-Address"));
        if (std::regex_match(reply.to_string(),std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {
            _logger.log("print message metadata\n\n");
            printMetaData(reply);

            _logger.log("added ip");

            peer_address = std::string(address);
            known_peer_addresses.insert(std::string(address));
            connection_table[reply.to_string()] = std::string(address);
        } else {
            _logger.warn("requesting peer already has a bound a peer to the net");
        }
    }
}

void connectionService::receiveAlt(abstractSocket& socket, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table) {
    //socket.bind("tcp", "*", 5555);
    socket.recvAlt();
    socketMessage message;
    if (!message.payload.empty()) {
        if (!known_peer_addresses.contains(message.payload)) {
            if (std::regex_match(message.payload,
                                 std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {

                // Add to connection to topology
                known_peer_addresses.insert(message.payload);
                known_peer_addresses.insert(message.addressFrom);
                connection_table.insert(
                        std::make_pair(message.addressFrom, message.payload));

                _logger.log("added: " + message.payload + " to network");

                socket.send(message.addressFrom);
            } else {
                socket.send(std::string("rejected"));
                _logger.log("the requested peer address is rejected, an ip4 is required");
            }
        } else {
            socket.send(std::string("rejected"));
            _logger.log("Peer rejected, already ip address already belongs to a known peer");
        }
    }
}

void connectionService::receive(void *abstractContext, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table) {
    _logger.log("Wait for connection");

    zmq::context_t *zmq_context = (zmq::context_t *) abstractContext;
    zmq::socket_t sock(*zmq_context, zmq::socket_type::rep);

    zmq::version();
    sock.bind("tcp://*:5555");
    zmq::message_t request;

    sock.recv(request, zmq::recv_flags::none);

    if (!request.empty()) {
        if (!known_peer_addresses.contains(request.to_string())) {
            if (std::regex_match(request.to_string(),
                                 std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {

                // Add to connection to topology
                known_peer_addresses.insert(request.to_string());
                known_peer_addresses.insert(std::string(request.gets("Peer-Address")));
                connection_table.insert(
                        std::make_pair(std::string(request.gets("Peer-Address")), std::string(request.to_string())));

                printMetaData(request);
                _logger.log("added: " + request.to_string() + " to network");

                sock.send(zmq::message_t(std::string(request.gets("Peer-Address"))), zmq::send_flags::none);
            } else {
                sock.send(zmq::message_t(std::string("rejected")), zmq::send_flags::none);
                _logger.log("the requested peer address is rejected, an ip4 is required");
            }
        } else {
            sock.send(zmq::message_t(std::string("rejected")), zmq::send_flags::none);
            _logger.log("Peer rejected, already ip address already belongs to a known peer");
        }
    }
}

void connectionService::printMetaData(zmq::message_t &msg) {
    try {
        std::string socket_type_string = msg.gets("Socket-Type");
        _logger.log("Socket-Type: " + socket_type_string);
    } catch (zmq::error_t er) {
        _logger.error("Can't read Socket-Type");
    }

    try {
        std::string peer_address_string = msg.gets("Peer-Address");
        _logger.log("Peer-Address: " + peer_address_string);
    } catch (zmq::error_t er) {
        _logger.error("Can't read Peer-Address");
    }

    try {
        std::string user_id_string = msg.gets("User-Id");
        _logger.log("User-Id: " + user_id_string);
    } catch (zmq::error_t er) {
        _logger.error("Can't read User-Id");
    }

    try {
        std::string routing_id_string = msg.gets("Routing-Id");
        _logger.log("Routing-Id: " + routing_id_string);
    } catch (zmq::error_t er) {
        _logger.error("Can't read Routing-Id");
    }
}
