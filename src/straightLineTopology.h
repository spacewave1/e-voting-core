//
// Created by wnabo on 08.11.2021.
//

#include "abstractTopology.h"
#include "straightLineSyncThread.h"
#include <set>
#include <map>
#include <string>
#include <set>

#ifndef VOTE_P2P_STRAIGHTLINETOPOLOGY_H
#define VOTE_P2P_STRAIGHTLINETOPOLOGY_H


class straightLineTopology : public abstractTopology{
public:
    void addPeer();
    abstractThread& getSyncWorker() override;
    straightLineTopology(straightLineSyncThread &worker);

private:
    straightLineSyncThread& worker;
};


#endif //VOTE_P2P_STRAIGHTLINETOPOLOGY_H
