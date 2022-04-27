//
// Created by wnabo on 31.10.2021.
//

#include <set>
#include <map>
#include "networkPlan.h"
#include "election.h"
#include "inprocElectionboxThread.h"

#ifndef VOTE_P2P_PEER_H
#define VOTE_P2P_PEER_H


class peer {
public:
    peer();
    virtual ~peer();
    void receive(void *context);
    election createElection(size_t election_id);
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
    void importPeerIdentity(std::string importPath = "./");
    void distributeElection(void* context, straightLineDistributeThread& thread);
    void passiveDistribution(void* context, straightLineDistributeThread& thread);
    void dumpElectionBox();
    void pushBackElection(election election);
    void startInprocElectionSyncThread(void *context, inprocElectionboxThread& thread);
    std::vector<election> &getElectionBox();
    void updateDistributionThread(straightLineDistributeThread *p_thread);
    void encryptVote(election &selected_election, std::string vote, unsigned char *encry);
    void decryptVote(election election, unsigned char *ciphertext, unsigned char *decry);
    void eval_votes();

private:
    void calculatePositionFromTable();
    std::string peer_identity;
    std::string peer_address;
    size_t position;
    std::vector<election> election_box;
    std::set<std::string> known_peer_addresses;
    std::map<std::string, std::string> connection_table;

    std::map<size_t, std::string> electionKeys;
    std::map<size_t, bool> evaluatedVotes;

    logger _logger = logger::Instance();

    size_t selectElection();

    bool isNumber(const std::string s);

    size_t getAndIncrement(std::string self_address, std::string current_address,
                           std::map<std::string, std::string> &connection_table, size_t current_position);
};


#endif //VOTE_P2P_PEER_H
