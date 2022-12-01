//
// Created by wnabo on 08.11.2021.
//

#ifndef VOTE_P2P_NETWORKPLAN_H
#define VOTE_P2P_NETWORKPLAN_H


#include <vector>
#include "straightLineTopology.h"

class networkPlan {
public:
    straightLineSyncThread getSyncWorker();
    void startForwardData();
    void initDataDistribution();
    networkPlan(abstractTopology &topology);

private:
    abstractTopology& topology;
};


#endif //VOTE_P2P_NETWORKPLAN_H
