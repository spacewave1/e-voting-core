//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
#define VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H

#include "abstractThread.h"
#include "election.h"
#include <zmq.hpp>
#include <string>

class straightLineDistributeThread : public abstractThread {
private:
    void InternalThreadEntry() override;
    std::string receiveDistributionRequest();
    void receiveData(std::string direction);
    void forwardData(std::string direction);
    void sendDistributionRequest(std::string direction);
    void log(std::string address, std::string content);
    std::string address_up;
    std::string address_down;
    election election_snapshot_to_send;
    size_t node_position;
    size_t network_size;
    zmq::socket_t subscribe_socket;
    zmq::socket_t publish_socket;
    size_t publish_port;
    size_t subscribe_port;
    bool is_initial_requester;
    void* arg;
public:
    straightLineDistributeThread(election &election_snapshot);
    election getElectionSnapshot() const;
    void setupDistribution(zmq::message_t &request, nlohmann::json receiveJson);
    void setInitialDistributer(const bool _is_initial_requester);
    void setParams(void* arg, std::string address_up = "", std::string address_down = "", size_t node_position = 0, size_t network_size = 0, const election &election_snapshot = election());

    void forwardUp();
    void forwardDown();
    void receiveFromUp();
    void receiveFromDown();

    void receiveInitialSetupRequest();
    void sendInitialRequest();
    straightLineDistributeThread();

    std::string invertDirection(std::string direction);
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
