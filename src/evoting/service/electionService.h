//
// Created by wld on 26.02.23.
//

#ifndef E_VOTING_ELECTIONSERVICE_H
#define E_VOTING_ELECTIONSERVICE_H


#include "../logger.h"
#include "hillEncryptionService.h"
#include "../model/election.h"

class electionService {
private:
    logger _logger = logger::Instance();
    hillEncryptionService encryption_service;
public:
    bool placeEncryptedVote(std::string input, election& chosen_election, std::map<size_t, std::string>& own_election_keys, std::string peer_identity);

    explicit electionService(const hillEncryptionService &encryptionService);

    void decryptWithKeys(election &chosen_election, std::string peer_identity, std::vector<std::string> &keys);
};


#endif //E_VOTING_ELECTIONSERVICE_H
