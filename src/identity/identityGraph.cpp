//
// Created by wld on 01.01.23.
//

#include "identityGraph.h"
#include <utility>

void
identityGraph::addFullyBackedClaim(std::string claim, std::string verifier, std::string issuer, std::string credential,
                                   size_t& key_count, size_t id_to_attach) {
    size_t claim_id = key_count;
    size_t verifier_id = key_count + 1;
    size_t issuer_id = key_count + 2;
    size_t credential_id = key_count + 3;
    
    graph.insert_edge(claim_id, id_to_attach);
    graph.insert_edge(id_to_attach, claim_id);
    data.try_emplace(claim_id,claim);

    graph.insert_edge(verifier_id, id_to_attach);
    graph.insert_edge(id_to_attach, verifier_id);
    data.try_emplace(verifier_id,verifier);

    graph.insert_edge(issuer_id, id_to_attach);
    graph.insert_edge(id_to_attach, issuer_id);
    data.try_emplace(issuer_id,issuer);

    graph.insert_edge(credential_id, id_to_attach);
    graph.insert_edge(id_to_attach, credential_id);
    data.try_emplace(credential_id,credential);

    key_count += 4;
}

std::string identityGraph::serializeEdges() const {
    std::vector<std::pair<size_t, size_t>> edgePairs = graph.edge_list();
    std::string edges_string;
    std::for_each(edgePairs.begin(), edgePairs.end(), [&edges_string](std::pair<size_t, size_t> edge){
        edges_string += std::to_string(edge.first);
        edges_string += ",";
        edges_string += std::to_string(edge.second);
        edges_string += "\n";
    });
    return edges_string;
}

std::string identityGraph::serializeNodeContent() {
    std::string content_string;
    std::for_each(data.begin(), data.end(), [&content_string](const std::pair<size_t,std::string>& node){
        content_string += std::to_string(node.first);
        content_string += ": ";
        content_string += node.second;
        content_string += "\n";
    });
    return content_string;
}
