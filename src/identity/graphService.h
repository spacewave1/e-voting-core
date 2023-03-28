//
// Created by wld on 01.12.22.
//

#ifndef E_VOTING_GRAPHSERVICE_H
#define E_VOTING_GRAPHSERVICE_H

#include "../evoting/logger.h"
#include "ngraph.hpp"
#include "identityGraph.h"

class graphService {
public:
    // TODO: Think about whether the graph logic is needed
    std::string importPeerIdentity(std::string importPath = "./");
    void addClaimToIdentityGraph(std::string claim, identityGraph& graph, size_t& counter);
    void exportIdentityGraph(identityGraph graph, std::string exportPath  = "./");
private:
    logger _logger = logger::Instance();
};


#endif //E_VOTING_GRAPHSERVICE_H
