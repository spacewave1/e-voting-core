//
// Created by wld on 02.04.23.
//

#ifndef E_VOTING_DIDDISTRIBUTIONSERVICE_H
#define E_VOTING_DIDDISTRIBUTIONSERVICE_H


#include <nlohmann/json.hpp>
#include "../datasource/inMemoryStorage.h"
#include "../network/abstractSocket.h"
#include "../evoting/model/election.h"

class didDistributionService {

public:
    unsigned long calculatePosition(const inMemoryStorage& storage, const did id);

    void getDistributionParams(const inMemoryStorage& storage, const did id, std::string &address_up, std::string &address_down);

    void sendInitialPortsSetupRequest(abstractSocket &socket_up, abstractSocket &socket_down, std::string local_address,
                                      size_t position, std::string address_up, std::string address_down);

    void sendDirectionRequestNumberOfHops(abstractSocket &socket, size_t hops);

    void sendDirectionRequest(abstractSocket& socket, nlohmann::json data, size_t position, std::string address_down,
                              std::string address_up);

    std::string sendInitialDistributionRequestDirection(abstractSocket &socket_up, abstractSocket &socket_down,
                                                        std::string local_address, std::string address_up,
                                                        std::string address_down, size_t network_size, size_t position);

    std::string
    sendForwardistributionRequestDirection(abstractSocket &socket_up, abstractSocket &socket_down, std::string direction);

    void sendElection(abstractSocket& socket, election &election_update, int port);

    void sendSuccessResponse(abstractSocket &socket);

    std::string invertDirection(std::string input_direction);

    election receiveElection(abstractSocket &socket);

    void updateElectionBox(election election_update, std::vector<election> election_box);

    size_t getAndIncrement(const did& own_id, did current_id, std::map<did, did> did_chain_down, size_t position);

private:
    logger _logger = logger::Instance();
};


#endif //E_VOTING_DIDDISTRIBUTIONSERVICE_H
