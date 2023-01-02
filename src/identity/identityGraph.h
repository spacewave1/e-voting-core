//
// Created by wld on 01.01.23.
//

#ifndef E_VOTING_IDENTITYGRAPH_H
#define E_VOTING_IDENTITYGRAPH_H


#include <unordered_map>
#include "ngraph.hpp"

enum NodeType{
    VERIFIER,
    ISSUER,
    CREDENTIAL,
    CLAIM
};

class identityGraph {
public:
    NGraph::tGraph<size_t> graph;
    std::unordered_map<size_t, std::string> data;
    void addFullyBackedClaim(std::string claim, std::string verifier, std::string issuer, std::string credential, size_t& key_count, size_t id_to_attach = 1);
    std::string serializeEdges() const;
    std::string serializeNodeContent();
    identityGraph(std::string id_string) { data[1] = "id:" + id_string; }
};


#endif //E_VOTING_IDENTITYGRAPH_H
