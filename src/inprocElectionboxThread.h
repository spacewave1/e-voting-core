//
// Created by wnabo on 15.01.2022.
//

#ifndef VOTE_P2P_INPROCELECTIONBOXTHREAD_H
#define VOTE_P2P_INPROCELECTIONBOXTHREAD_H

#include "abstractThread.h"
#include "election.h"
#include "network/abstractSocket.h"
#include "logger.h"

class inprocElectionboxThread : public abstractThread  {
    void InternalThreadEntry() override;
public:
    ~inprocElectionboxThread() override;
    void runElectionUpdate();
    inprocElectionboxThread(std::vector<election> &election_box, abstractSocket &abstract_socket);
    void interrupt();
private:
    std::vector<election>& election_box;
    abstractSocket& abstract_socket;
    logger _logger = _logger.Instance();
    bool is_interrupted = false;
};


#endif //VOTE_P2P_INPROCELECTIONBOXTHREAD_H
