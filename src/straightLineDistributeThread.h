//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
#define VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H

#include "abstractThread.h"
#include "election.h"
#include "abstractSocket.h"
#include <nlohmann/json.hpp>
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
    abstractSocket& subscribe_socket;
    abstractSocket& publish_socket;
    size_t publish_port;
    size_t subscribe_port;
    bool is_initial_requester;
    void* arg;
public:
    election getElectionSnapshot() const;
    void setupDistribution(zmq::message_t &request, nlohmann::json receiveJson);
    void setInitialDistributer(bool _is_initial_requester);
    void setParams(void* arg, std::string address_up = "", std::string address_down = "", size_t node_position = 0, size_t network_size = 0, const election &election_snapshot = election());
    void setSubscribeSocket(abstractSocket& socket);
    void setPublishSocket(abstractSocket& socket);
    void setPublishPort(size_t port_number);
    void setSubscribePort(size_t port_number);
    void forwardUp();
    void forwardDown();
    election receiveFromUp();
    election receiveFromDown();

    void receiveInitialSetupRequest();
    void sendInitialRequest();

    std::string invertDirection(std::string direction);

    straightLineDistributeThread(abstractSocket &publish_socket,
                                 abstractSocket &subscribe_socket);
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
