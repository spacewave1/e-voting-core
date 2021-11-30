//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
#define VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H

#include "abstractThread.h"
#include "election.h"
#include <string>

class straightLineDistributeThread : public abstractThread {
private:
    void InternalThreadEntry() override;
    void receiveFromDownForwardUp();
    void receiveFromUpForwardDown();
    std::string address_up;
    std::string address_down;
    election &election_snapshot_to_send;
    void* arg;
public:
    straightLineDistributeThread(election &election_snapshot);
    election &getElectionSnapshot() const;
    void setParams(void* arg, std::string address_up = "", std::string address_down = "", const election &election_snapshot = election());
    void forwardUp();
    void receiveFromUp();

    straightLineDistributeThread();
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
