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
    void receiveFromDownForwardUp();
    void receiveFromUpForwardDown();
    std::string address_up;
    std::string address_down;
    election election_snapshot_to_send;
    size_t nodePosition;
    zmq::socket_t subscribe_socket;
    zmq::socket_t publish_socket;
    bool is_initial_requester;
    void* arg;
public:
    straightLineDistributeThread(election &election_snapshot);
    election getElectionSnapshot() const;
    void setupDistribution(zmq::message_t &request, nlohmann::json receiveJson);
    void setInitialDistributer(const bool _is_initial_requester);
    void setParams(void* arg, std::string address_up = "", std::string address_down = "", const size_t nodePosition = 0, const election &election_snapshot = election());
    void forwardUp();
    void forwardDown();
    void receiveFromUp();
    void receiveFromDown();
    void receiveRequest();
    void sendInitialRequest();
    straightLineDistributeThread();
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
