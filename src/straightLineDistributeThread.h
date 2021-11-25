//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
#define VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H

#include <string>
#include "abstractThread.h"
#include "election.h"

class straightLineDistributeThread : public abstractThread {
private:
    void InternalThreadEntry() override;
    void receiveFromDownForwardUp();
    void receiveFromUpForwardDown();
    std::string address_up;
    std::string address_down;
    election &electionSnapshot;
    void* arg;
public:
    void setParams(void* arg, std::string address_up, std::string address_down, const election &election_snapshot);

    void forwardUp();

    void receiveFromUp();
};


#endif //VOTE_P2P_STRAIGHTLINEDISTRIBUTETHREAD_H
