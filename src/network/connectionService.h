//
// Created by wld on 01.12.22.
//

#ifndef E_VOTING_CONNECTIONSERVICE_H
#define E_VOTING_CONNECTIONSERVICE_H


#include <string>
#include <set>
#include <map>
#include <zmq.hpp>
#include "../evoting/logger.h"
#include "abstractSocket.h"

class connectionService {
public:
    void exportPeerConnections(std::string exportPath = "./", std::map<std::string, std::string> connection_table = std::map<std::string, std::string>());
    void exportPeersList(std::string exportPath = "./", std::set<std::string> known_peer_addresses = std::set<std::string>(), std::string exportFile = "peers.json");
    std::map<std::string, std::string>& importPeerConnections(std::map<std::string, std::string>& connection_table, std::string importPath = "./");
    std::set<std::string>& importPeersList(std::set<std::string>& peer_addresses, std::string importPath = "./");
    void connect(std::string &input, void *abstractContext, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table, std::string& peer_address);
    void connect(abstractSocket& socket, const std::string &input, std::set<std::string> &known_peer_addresses,
                 std::map<std::string, std::string> &connection_table);
    void printMetaData(zmq::message_t &msg);
    void receive(void *abstractContext, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table);
    void changeToListenState(abstractSocket& socket);

    std::string createNetworkRegistrationRequest(std::string connectToAddress);

    void sendConnectionRequest(abstractSocket& socket, std::string &input);
    void receiveConnectionReply(abstractSocket& socket, std::string &input, std::set<std::string>& known_peer_addresses, std::map<std::string, std::string>& connection_table, std::string peer_address);

private:
    logger _logger = logger::Instance();

    std::string createOnBoardingReply(std::string connectFromAddress);
};


#endif //E_VOTING_CONNECTIONSERVICE_H
