//
// Created by wld on 31.01.23.
//

#ifndef E_VOTING_DISTRIBUTIONSERVICE_H
#define E_VOTING_DISTRIBUTIONSERVICE_H


#include "../../network/abstractSocket.h"
#include "../logger.h"
#include "../model/election.h"

class distributionService {
public:
    size_t calculatePosition(std::map<std::string, std::string> &connection_table, std::string peer_address);
    void
    sendInitialPortsSetupRequest(abstractSocket *socketUp, abstractSocket *socketDown, std::string local_address, size_t node_position, std::string address_up,
                                 std::string address_down);
    void sendSuccessResponse(abstractSocket *socket);
    void getDistributionParams(std::map<std::string, std::string> connection_table,
                               const std::string peer_address, std::string &address_up, std::string &address_down);
    void sendDirectionRequest(abstractSocket *forward_request_socket, nlohmann::json receiveJson, size_t node_position,
                              std::string address_down, std::string address_up);
    void connectDirectionRequest(abstractSocket *forward_request_socket,
                                                      nlohmann::json receiveJson,
                                                      size_t node_position,
                                                      std::string address_down,
                                                      std::string address_up);

    std::string
    sendInitialDistributionRequestDirection(abstractSocket *socket_up, abstractSocket *socket_down, std::string address, std::string address_up,
                                            std::string address_down,
                                            size_t network_size, size_t node_position);

    void sendDirectionRequestNumberOfHops(abstractSocket *socket, size_t current_number_of_hops);

    election receiveElection(abstractSocket *socket);

    void sendElection(abstractSocket *socket, election &election_snapshot, size_t publish_port);

    std::string
    sendForwardistributionRequestDirection(abstractSocket *socket_up, abstractSocket *socket_down,
                                            std::string address_up,
                                            std::string address_down, size_t current_number_of_hops,
                                            size_t network_size,
                                            size_t node_position, std::string directionTo);

    std::map<std::string, std::string> getReversedConnectionTable(std::map<std::string, std::string> &connection_table);

    void updateElectionBox(election el, std::vector<election>& election_box);

    std::string invertDirection(std::string direction);

private:
    logger _logger = logger::Instance();

    size_t getAndIncrement(std::string self_address, std::string current_address,
                           std::map<std::string, std::string> &connection_table, size_t current_position);

};


#endif //E_VOTING_DISTRIBUTIONSERVICE_H
