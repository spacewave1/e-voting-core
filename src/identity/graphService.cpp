//
// Created by wld on 01.12.22.
//

#include <fstream>
#include <ctime>
#include "graphService.h"

std::string graphService::importPeerIdentity(std::string importPath) {
    std::ifstream importStream;
    std::string line;

    _logger.log("is importing peers file from " + importPath + "id.dat");

    // File Open in the Read Mode
    importStream.open(importPath + "id.dat");

    if (importStream.is_open()) {
        if (getline(importStream, line)) {
            _logger.log("Display File Content:");
            _logger.displayData(line);

            //peer_identity = line;
            //peer_address = line;
            return line;

        };
        // File Close
        importStream.close();
        _logger.error("Could not return line");
    } else {
        _logger.error("Unable to open the file!");
    }
    return "";
}

void graphService::addClaimToIdentityGraph(std::string claim, identityGraph& graph, size_t& counter) {
    graph.addFullyBackedClaim(claim, "<verifier>","<issuer>","<credential",counter);
}

void graphService::exportIdentityGraph(identityGraph graph, std::string exportPath){
    std::string edgesAsString = graph.serializeEdges();
    std::string node_content_as_string = graph.serializeNodeContent();

    std::ofstream exportStream;
    exportStream.open(exportPath + "graph.edges");
    exportStream << edgesAsString << "\n";
    exportStream.close();

    exportStream.open(exportPath + "graph.nodes");
    exportStream << node_content_as_string << "\n";
    exportStream.close();
}
