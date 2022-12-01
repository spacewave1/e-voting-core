//
// Created by wnabo on 08.11.2021.
//

#include "straightLineTopology.h"
#include "networkPlan.h"


void networkPlan::startForwardData() {

}

void networkPlan::initDataDistribution() {

}

straightLineSyncThread networkPlan::getSyncWorker() {
    return (straightLineSyncThread&) topology.getSyncWorker();
}

networkPlan::networkPlan(abstractTopology &topology) : topology(topology) {}

