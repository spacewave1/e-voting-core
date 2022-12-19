//
// Created by wnabo on 31.10.2021.
//

#include <set>
#include <map>
#include <queue>
#include "election.h"
#include "inprocElectionboxThread.h"
#include "replyKeyThread.h"
#include "basicEncryptionService.h"
#include "straightLineSyncThread.h"
#include "straightLineDistributeThread.h"

#ifndef VOTE_P2P_PEER_H
#define VOTE_P2P_PEER_H


class peer {
public:
    peer();

    virtual ~peer();

    election createElection(size_t election_id);

    void vote(basicEncryptionService& encryption_service, size_t election_id = -1);

    void printConnections();

    void initSyncThread(void *context, straightLineSyncThread &thread, std::string initial_receiver_address = "");

    std::set<std::string> &getKnownPeerAddresses();

    std::map<std::string, std::string> &getConnectionTable();

    std::string &getIdentity();

    void setKnownPeerAddresses(const std::set<std::string> &known_peer_addresses);

    void setConnectionTable(const std::map<std::string, std::string> &connection_table);

    void setIdentity(std::string identity);

    void distributeElection(void *context, straightLineDistributeThread &thread, size_t chosen_election_id = -1);

    void passiveDistribution(void *context, straightLineDistributeThread &thread);

    void dumpElectionBox();

    void pushBackElection(election election);

    void startInprocElectionSyncThread(void *context, inprocElectionboxThread &thread);

    std::vector<election> &getElectionBox();

    void updateDistributionThread(straightLineDistributeThread *p_thread);

    bool eval_votes(replyKeyThread &replyThread, basicEncryptionService &encryption_service, size_t election_id = -1);

    void generate_keys(basicEncryptionService &encryption_service, size_t election_box_position = -1);

    void request_keys(void *args, size_t election_box_position = -1);

    void reply_keys(void *args, replyKeyThread &thread);
    void countInVotes(zmq::context_t *p_context, straightLineDistributeThread &thread, basicEncryptionService &encryption_service, size_t election_box_position = -1);
    size_t selectElection();

    void decrypt_vote(size_t election_id, basicEncryptionService &encryption_service);

private:
    void calculatePositionFromTable();

    std::string peer_identity;
    std::string peer_address;
    size_t position;
    std::vector<election> election_box;
    std::set<std::string> known_peer_addresses;
    std::map<std::string, std::string> connection_table;

    std::map<size_t, std::string> own_election_keys;
    std::shared_ptr<std::map<size_t, std::queue<std::string>>> prepared_election_keys_pointer;
    std::map<size_t, std::queue<std::string>> prepared_election_keys;
    std::map<size_t, std::vector<std::string>> received_election_keys;
    std::map<size_t, bool> isEvaluatedVotesMap;

    logger _logger = logger::Instance();


    bool isNumber(const std::string s);

    size_t getAndIncrement(std::string self_address, std::string current_address,
                           std::map<std::string, std::string> &connection_table, size_t current_position);
};


#endif //VOTE_P2P_PEER_H
