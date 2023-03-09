//
// Created by wld on 09.03.23.
//

#include <string>
#include "../../crypt/sha.h"

#ifndef E_VOTING_HASHSERVICE_H
#define E_VOTING_HASHSERVICE_H


class hashService {
public:
    std::string hashMessage(std::string message);
};


#endif //E_VOTING_HASHSERVICE_H
