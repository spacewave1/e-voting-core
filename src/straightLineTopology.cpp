//
// Created by wnabo on 08.11.2021.
//

#include "straightLineTopology.h"

#include <utility>

void straightLineTopology::addPeer() {

}

abstractThread& straightLineTopology::getSyncWorker() {
    return (abstractThread&) worker;
}

straightLineTopology::straightLineTopology(straightLineSyncThread &worker) : worker(worker) {}
