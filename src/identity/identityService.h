//
// Created by wld on 01.12.22.
//

#ifndef E_VOTING_IDENTITYSERVICE_H
#define E_VOTING_IDENTITYSERVICE_H


#include <string>
#include "../evoting/logger.h"

class identityService {
public:
    std::string importPeerIdentity(std::string importPath = "./");
private:
    logger _logger = logger::Instance();
};


#endif //E_VOTING_IDENTITYSERVICE_H
