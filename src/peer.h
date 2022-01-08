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
    void createElection(size_t election_id);
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
    void distributeElection(void* context, straightLineDistributeThread& thread);
    void passiveDistribution(void* context, straightLineDistributeThread& thread);
    void dumpElectionBox();
private:
    void calculatePositionFromTable();
    std::string peer_identity;
    std::string peer_address;
    size_t position;
    std::vector<election> election_box;
    std::set<std::string> known_peer_addresses;
    std::map<std::string, std::string> connection_table;

    election& selectElection();

    bool isNumber(const std::string s);

    size_t getAndIncrement(std::string self_address, std::string current_address,
                           std::map<std::string, std::string> &connection_table, size_t current_position);
};


#endif //VOTE_P2P_PEER_H
