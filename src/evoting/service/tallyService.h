//
// Created by wld on 21.02.23.
//

#ifndef E_VOTING_TALLYSERVICE_H
#define E_VOTING_TALLYSERVICE_H


#include "../model/election.h"
#include "hillEncryptionService.h"
#include "../../network/abstractSocket.h"

class tallyService {
private:
    logger _logger = _logger.Instance();
    hillEncryptionService& encryption_service;
    void
    generate_keys(election &chosen_election, std::string peer_identity, std::map<size_t, std::string>& own_election_keys,
                  std::map<size_t, std::queue<std::string>>& prepared_election_keys);

public:
    bool prepareTally(election &chosen_election, std::map<size_t, bool> is_evaluated_votes_map, std::string peer_address,
                      std::map<size_t, std::string>& own_election_keys,
                      std::map<size_t, std::queue<std::string>>& prepared_election_keys);
    explicit tallyService(hillEncryptionService &encryptionService);

    void requestKey(abstractSocket &socket, election &chosen_election, std::string identity, std::string participant);

    void receiveKey(abstractSocket &socket, size_t election_id,
                    std::map<size_t, std::vector<std::string>> &received_election_keys);

    std::vector<std::string> findGroupAndFilterOwnIdentity(election& chosen_election, std::string identity_self);

    void tally(abstractSocket &socket, election &chosen_election, std::string identity,
               std::vector<std::string> participants_without_self, std::map<size_t, std::vector<std::string>>& received_election_keys);

    size_t receiveKeyRequest(abstractSocket &socket);

    void keyResponse(abstractSocket &socket, size_t election_id,
                     std::map<size_t, std::queue<std::string>> *prepared_election_keys);

    void keyResponseFinish(abstractSocket &socket);
};


#endif //E_VOTING_TALLYSERVICE_H
