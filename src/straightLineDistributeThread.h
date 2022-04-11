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
#include "logger.h"

class straightLineDistributeThread : public abstractThread {
private:
    logger _logger = logger::Instance();
    void InternalThreadEntry() override;
    std::string receiveDistributionRequest();
    void receiveData(std::string direction);
    void sendDistributionRequest(std::string direction);
    std::string address_up;
    std::string address_down;
    election election_snapshot_to_send;
    size_t node_position;
    size_t network_size;
    size_t publish_port;
    size_t subscribe_port;
    bool is_initial_requester;
    size_t current_number_of_hops = 0;
    bool is_interrupted = false;
    bool is_running = false;
    void* arg;
    void cleanUp();
public:
    straightLineDistributeThread(const abstractSocket& publish_socket,const abstractSocket& subscribe_socket);
    election getElectionSnapshot() const;
    void setupDistribution(zmq::message_t &request, nlohmann::json receiveJson);
    void setInitialDistributer(bool _is_initial_requester);
    void setParams(void* arg, std::string address_up = "", std::string address_down = "", size_t node_position = 0, size_t network_size = 0, const election &election_snapshot = election());
    void setSubscribeSocket(const abstractSocket &socket);
    void setPublishSocket(const abstractSocket &socket);
    void setPublishPort(size_t port_number);
    void setAddressUp(std::string addressUp);
    void setAddressDown(std::string addressDown);
    void setPosition(size_t position);
    void setSubscribePort(size_t port_number);
    void setNetworkSize(size_t networkSize);
    void forward();
    election receiveFrom(std::string address);
    void interruptReceiveRequest();

    void receiveInitialSetupRequest();
    void sendInitialRequest();

    std::string invertDirection(std::string direction);

    void updateElectionBox();

    void setContext(void *p_void);

    bool isRunning();

    void setIsRunning(bool b);

    void resetHops();

    straightLineDistributeThread();
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
