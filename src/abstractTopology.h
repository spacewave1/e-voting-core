//
// Created by wnabo on 31.10.2021.
//

#include <set>
#include <map>
#include <string>
#include <memory>
#include "abstractThread.h"

#ifndef VOTE_P2P_TOPOLOGY_H
#define VOTE_P2P_TOPOLOGY_H

class abstractTopology {
public:
    //virtual void addPeer() = 0;
    //virtual void InitSyncronizeTopologyData(void *arg, std::string initial_receiver_address, std::map<std::string, std::string> connection_table, std::set<std::string> peers) = 0;
    //virtual void SyncronizeTopologyData(void* arg, std::map<std::string, std::string> connection_table, std::set<std::string> peers) = 0;

    virtual abstractThread& getSyncWorker() = 0;
};


#endif //VOTE_P2P_TOPOLOGY_H
