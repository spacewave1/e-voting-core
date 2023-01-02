//
// Created by wld on 25.12.22.
//

#include <iostream>
#include <unordered_map>
#include "identityService.h"

std::unordered_map<int, std::string> data;

void createIdentity(){
    data[1] = "id:1";
}

int main() {
    identityService identityService;
    identityGraph identityGraph{"abc123"};

    size_t graph_counter = 2;
    identityService.addClaimToIdentityGraph("election:1:participate", identityGraph, graph_counter);
    identityService.exportIdentityGraph(identityGraph);

    return 0;
}
