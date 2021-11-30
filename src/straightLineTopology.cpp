//
// Created by wnabo on 08.11.2021.
//

#include "straightLineTopology.h"

#include <utility>

void straightLineTopology::addPeer() {

}

abstractThread &straightLineTopology::getSyncWorker() {
    return (abstractThread &) sync_worker;
}

straightLineTopology::straightLineTopology(straightLineSyncThread &sync_worker,
                                           straightLineDistributeThread &distribution_worker) :
                                           sync_worker(sync_worker),distribution_worker(distribution_worker) {}

abstractThread &straightLineTopology::getDistributeWorker() {
    return (abstractThread &) distribution_worker;
}
