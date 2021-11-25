//
// Created by wnabo on 31.10.2021.
//

#include <string>
#include <set>
#include <map>
#include "networkPlan.h"
#include "election.h"

#ifndef VOTE_P2P_PEER_H
#define VOTE_P2P_PEER_H


class peer {
public:
    peer();
    virtual ~peer();
    void receive(void *context);
    void createElection();
    void vote();
    void connect(std::string& input, void *context);
    void printConnections();
    void initSyncThread(void* context, straightLineSyncThread& thread, std::string initial_receiver_address = "");
    const std::set <std::string> &getKnownPeerAddresses() const;
    const std::map <std::string, std::string> &getConnectionTable() const;
    void setKnownPeerAddresses(const std::set <std::string> &known_peer_addresses);
    void setConnectionTable(const std::map <std::string, std::string> &connection_table);
    void setIdentity(std::string identity);
    void exportPeerConnections(std::string exportPath = "./");
    void exportPeersList(std::string exportPath = "./");
    void importPeerConnections(std::string importPath = "./");
    void importPeersList(std::string importPath = "./");
    void dumpElectionBox();
private:
    std::string peer_identity;
    std::string peer_address;
    std::vector<election> election_box;
    std::set<std::string> known_peer_addresses;
    std::map<std::string, std::string> connection_table;
};


#endif //VOTE_P2P_PEER_H
