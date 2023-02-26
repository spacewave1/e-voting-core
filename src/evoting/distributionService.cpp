//
// Created by wld on 31.01.23.
//

#include <nlohmann/json.hpp>
#include "distributionService.h"
#include "electionBuilder.h"

void distributionService::sendInitialPortsSetupRequest(abstractSocket *socketUp, abstractSocket *socketDown, std::string local_address, size_t node_position,
                                                       std::string address_up, std::string address_down) {
    int subscribe_port = 5051;
    int publish_port = 5050;

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;
    sendJson["origin_publish_port"] = publish_port;

    _logger.log("up not empty: " + std::to_string(!address_up.empty()), local_address);
    _logger.log("down not empty: " + std::to_string(!address_down.empty()), local_address);

    if (!address_up.empty()) {
        socketUp->setupSocket(local_address, 5049);
        socketUp->connect("tcp", address_up, 5049);
        socketUp->send(sendJson.dump());
    }

    if (!address_down.empty()) {
        socketDown->setupSocket(local_address, 5049);
        socketDown->connect("tcp", address_down, 5049);
        socketDown->send(sendJson.dump());
    }

    _logger.log("Has setup from active Distribution");
}

void distributionService::connectDirectionRequest(abstractSocket *forward_request_socket,
                                               nlohmann::json receiveJson,
                                               size_t node_position,
                                               std::string address_down,
                                               std::string address_up) {
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    nlohmann::json sendJson;

    if (originPosition < node_position) {
        if (!address_down.empty()) {
            forward_request_socket->connect("tcp", address_down, 5049);
        }
    } else if (originPosition > node_position) {
        if (!address_up.empty()) {
            forward_request_socket->connect("tcp", address_up, 5049);
        }
    }
}

void distributionService::sendDirectionRequest(abstractSocket *forward_request_socket,
                                               nlohmann::json receiveJson,
                                               size_t node_position,
                                               std::string address_down,
                                               std::string address_up) {
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    int subscribe_port = std::stoi(receiveJson["origin_publish_port"].dump());

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;
    int publish_port;

    if (originPosition < node_position) {
        _logger.log("Set subscribe socket for upwards", "localhost", "distribute");

        if (!address_down.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request_socket->send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    } else if (originPosition > node_position) {
        _logger.log("Set subscribe socket for downwards", "localhost", "distribute");

        if (!address_up.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request_socket->send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    }
}

void distributionService::getDistributionParams(std::map<std::string, std::string> connection_table,
                                                const std::string peer_address,
                                                std::string &address_up,
                                                std::string &address_down) {
    std::map<std::string, std::string> reversedConnectionTable;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversedConnectionTable](std::pair<std::string, std::string> addressToAddress) {
                      reversedConnectionTable[addressToAddress.second] = addressToAddress.first;
                  });

    if (connection_table.contains(peer_address)) {
        address_up = connection_table[peer_address];
    }

    if (reversedConnectionTable.contains(peer_address)) {
        address_down = reversedConnectionTable[peer_address];
    }
}


size_t distributionService::getAndIncrement(std::string self_address, std::string current_address,
                                            std::map<std::string, std::string> &connection_table,
                                            size_t current_position) {

    _logger.log("current address: " + current_address + ", self_address: " + self_address);

    if (current_address == self_address || !connection_table.contains(current_address)) {
        return current_position;
    } else {
        current_position++;
        return getAndIncrement(self_address, connection_table[current_address], connection_table, current_position);
    }
}

std::map<std::string, std::string> distributionService::getReversedConnectionTable(std::map<std::string, std::string> &connection_table) {
    std::map<std::string, std::string> reversed_connection_table;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversed_connection_table](std::pair<std::string, std::string> pair) {
                      reversed_connection_table[pair.second] = pair.first;
                  });

    return reversed_connection_table;
}


size_t
distributionService::calculatePosition(std::map<std::string, std::string> &connection_table, std::string peer_address) {
    std::map<std::string, std::string> reversed_connection_table;
    auto connection_table_temp = connection_table;

    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversed_connection_table](std::pair<std::string, std::string> pair) {
                      reversed_connection_table[pair.second] = pair.first;
                  });

    std::string root_address;

    std::for_each(reversed_connection_table.begin(), reversed_connection_table.end(),
                  [&connection_table_temp, &root_address](std::pair<std::string, std::string> pair) {
                      if (!connection_table_temp.contains(pair.first)) {
                          root_address = pair.first;
                      }
                  });
    if(root_address.empty()) {
        root_address = peer_address;
    }

    size_t pos = getAndIncrement(peer_address, root_address, reversed_connection_table, 0);

    _logger.log("Calculated position for " + peer_address + " is " + std::to_string(pos));
    return pos;
}

election distributionService::receiveElection(abstractSocket *socket) {
    _logger.log("receive from up", "localhost", "distribute");
    //_logger.log("subscribe to " + address + ":" + std::to_string(subscribe_port), "localhost", "distribute");

    const std::string &election_id_string = socket->recv().payload;
    int election_id = std::stoi(election_id_string);
    _logger.log("received election id: " + std::to_string(election_id), "localhost", "distribute");

    const std::string &sequence_id_string = socket->recv().payload;
    int sequence_id = std::stoi(sequence_id_string);
    _logger.log("received sequence id: " + std::to_string(sequence_id), "localhost", "distribute");

    std::string setup_date_string = socket->recv().payload;
    std::for_each(setup_date_string.begin(), setup_date_string.end(), [](char c) {
        std::cout << c << " ";
    });
    std::cout << std::endl;
    std::cout << setup_date_string << std::endl;
    time_t setup_date = (unsigned int) std::stoul(setup_date_string);
    _logger.log("received setup date: " + std::to_string(setup_date), "localhost", "distribute");

    std::string json = socket->recv().payload;
    nlohmann::json electionOptionsJson = nlohmann::json::parse(json);
    _logger.log("received " + electionOptionsJson.dump(), "localhost", "distribute");

    std::string jsonVotes = socket->recv().payload;
    nlohmann::json election_votes_json = nlohmann::json::parse(jsonVotes);
    _logger.log("received " + election_votes_json.dump(), "localhost", "distribute");

    std::string jsonGroups = socket->recv().payload;
    nlohmann::json election_json_groups = nlohmann::json::parse(jsonGroups);
    _logger.log("received " + election_json_groups.dump(), "localhost", "distribute");

    std::string jsonResult = socket->recv().payload;
    nlohmann::json election_json_result = nlohmann::json::parse(jsonResult);
    _logger.log("received " + election_json_result.dump(), "localhost", "distribute");


    election receivedElection = election::create(election_id)
            .withVoteOptionsFromJson(electionOptionsJson)
            .withParticipantsVotesFromJson(election_votes_json)
            .withSetupDate(setup_date)
            .withParticipantsFromParticipantVotesKeySet()
            .withSequenceNumber(sequence_id)
            .withPreparedForDistribution(true)
            .withElectionGroupsFromJson(election_json_groups)
            .withElectionResultFromJson(election_json_result);

    return receivedElection;
}

void distributionService::sendDirectionRequestNumberOfHops(abstractSocket *socket, size_t current_number_of_hops) {
    socket->send(std::to_string(current_number_of_hops));
}

void distributionService::sendElection(abstractSocket *socket, election &election_snapshot, size_t publish_port) {

    _logger.log("send on port: " + std::to_string(publish_port));

    socket->send(std::to_string(election_snapshot.getId()));
    _logger.log("send: " + std::to_string(election_snapshot.getId()));
    socket->send(std::to_string(election_snapshot.getSequenceNumber() + 1));
    _logger.log("send: " + std::to_string(election_snapshot.getSequenceNumber() + 1));
    socket->send(std::to_string(election_snapshot.getSetupDate()));
    _logger.log("send: " + election_snapshot.getSetupDateAsString());
    socket->send(election_snapshot.getElectionOptionsJson().dump());
    _logger.log("send: " + std::string(election_snapshot.getElectionOptionsJson().dump()));
    socket->send(election_snapshot.participantVotesAsJson().dump(4, ' ', true));
    _logger.log("send: " + std::string(election_snapshot.participantVotesAsJson().dump(4, ' ', true)));
    socket->send(election_snapshot.getEvaluationGroupsAsJson().dump());
    _logger.log("send: " + std::string(election_snapshot.getEvaluationGroupsAsJson().dump()));
    socket->send(election_snapshot.getElectionResultAsJson().dump());
    _logger.log("send: " + std::string(election_snapshot.getElectionResultAsJson().dump()));

    _logger.log("finished broadcasting");
}

void distributionService::sendSuccessResponse(abstractSocket *socket) {
    socket->send("accept");
}

std::string
distributionService::sendInitialDistributionRequestDirection(abstractSocket *socket_up, abstractSocket *socket_down,
                                                             std::string address,
                                                             std::string address_up,
                                                             std::string address_down,
                                                             size_t network_size, size_t node_position) {
    if (address_down.empty() && !address_up.empty()) {
        socket_up->setupSocket(address, 5049);
        socket_up->connect("tcp", address_up, 5049);
        socket_up->send("up");
        return "up";
    } else if (!address_down.empty() && address_up.empty()) {
        socket_down->setupSocket(address, 5049);
        socket_down->connect("tcp", address_down, 5049);
        socket_down->send("down");
        return "down";
    } else if (!address_down.empty() && !address_up.empty()) {
        // Send to the closest extreme
        if (node_position < network_size / 2) {
            socket_up->setupSocket(address, 5049);
            socket_up->connect("tcp", address_up, 5049);
            socket_up->send("up");
            return "up";
        } else {
            socket_down->setupSocket(address, 5049);
            socket_down->connect("tcp", address_down, 5049);
            socket_down->send("down");
            return "down";
        }
        // Position check case
        // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
        // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
    }

}

std::string distributionService::sendForwardistributionRequestDirection(abstractSocket *socket_up,
                                                                        abstractSocket *socket_down,
                                                                        std::string address_up,
                                                                        std::string address_down,
                                                                        size_t current_number_of_hops,
                                                                        size_t network_size,
                                                                        size_t node_position,
                                                                        std::string directionTo) {
    if(directionTo.find("up") != -1) {
        socket_up->send("up");
        return "up";
    } else if(directionTo.find("down") != -1) {
        socket_down->send("down");
        return "down";
    }
    // Position check case
    // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
    // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
    return "";
}

void distributionService::updateElectionBox(election el, std::vector<election>& election_box) {
    auto p_function = [el](const election &_election) {
            return _election.getId() == el.getId() && _election.getSetupDate() == el.getSetupDate();
        };
        auto result = std::find_if(election_box.begin(), election_box.end(), p_function);
        if (result != election_box.end()) {
            result->setJsonOptionsToOptions(el.getElectionOptionsJson());
            result->setSequenceNumber(el.getSequenceNumber());
            result->setSetupDate(el.getSetupDate());
            result->setJsonVotesToVotes(el.getVotesAsJson());
            result->setJsonElectionGroupToGroups(el.getEvaluationGroupsAsJson());
            result->setJsonResultToResult(el.getElectionResultAsJson());
        } else {
            election new_election = election::create(el.getId())
                    .withVoteOptionsFromJson(el.getElectionOptionsJson())
                    .withParticipantsVotesFromJson(el.participantVotesAsJson())
                    .withSetupDate(el.getSetupDate())
                    .withParticipantsFromParticipantVotesKeySet()
                    .withSequenceNumber(el.getSequenceNumber())
                    .withPreparedForDistribution(true)
                    .withElectionGroupsFromJson(el.getEvaluationGroupsAsJson())
                    .withElectionResultFromJson(el.getElectionResultAsJson());

            election_box.push_back(new_election);
        }
}

std::string distributionService::invertDirection(std::string direction) {
    if (direction == "up") {
        return "down";
    } else {
        return "up";
    }
}


