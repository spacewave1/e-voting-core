//
// Created by wnabo on 31.10.2021.
//

#include <iostream>
#include <zmq.hpp>
#include <regex>
#include <memory>
#include <nlohmann/json.hpp>
#include "peer.h"
#include "model/electionBuilder.h"
#include "service/hillEncryptionService.h"
#include <charconv>

void peer::printConnections() {
    // Iterate through "receiver" nodes
    size_t index = 0;

    _logger.log("print connections");

    _logger.log("Host Ip Addresses:");

    std::for_each(known_peer_addresses.begin(), known_peer_addresses.end(), [this ,&index](const std::string ip_address) {
        _logger.log("[" + std::to_string(index) + "] " + ip_address);
        index++;
    });

    // Iterate through connections
    index = 0;
    _logger.log("Connections:");

    std::for_each(connection_table.begin(), connection_table.end(),
                  [this, &index](const std::pair<std::string, std::string> connection) {
        _logger.log("[" + std::to_string(index) + "] " + connection.first + "->" + connection.second);
        index++;
      });

    nlohmann::json send_json = nlohmann::json();
    send_json["nodes"] = nlohmann::ordered_json(known_peer_addresses);
    send_json["connections"] = nlohmann::ordered_json(connection_table);

    _logger.log("As Json: " + nlohmann::to_string(send_json));
}

peer::~peer() {
    connection_table.clear();
    known_peer_addresses.clear();
}

peer::peer() {
    connection_table = std::map<std::string, std::string>();
    known_peer_addresses = std::set<std::string>();
    peer_address = "";
    prepared_election_keys_pointer = std::make_shared<std::map<size_t, std::queue<std::string>>>(prepared_election_keys);
}

void peer::initSyncThread(void *context, straightLineSyncThread &thread, std::string initial_receiver_address) {
    thread.setParams(context, connection_table, known_peer_addresses, initial_receiver_address);
    thread.StartInternalThread();
}


std::set<std::string> &peer::getKnownPeerAddresses() {
    return known_peer_addresses;
}

void peer::setKnownPeerAddresses(const std::set<std::string> &known_peer_addresses) {
    peer::known_peer_addresses = known_peer_addresses;
}

std::map<std::string, std::string> &peer::getConnectionTable() {
    return connection_table;
}

void peer::setConnectionTable(const std::map<std::string, std::string> &connection_table) {
    peer::connection_table = connection_table;
}

void peer::vote(hillEncryptionService& encryption_service, size_t election_id) {
    if (election_box.size() > 0) {
        size_t chosen_id = election_id;
        if(chosen_id == -1) {
            chosen_id = selectElection();
        }
        std::string chosen_id_as_string = std::to_string(election_id);
        election &chosen_election = election_box.at(chosen_id);

        if (!chosen_election.isPreparedForDistribution()) {
            chosen_election.prepareForDistribtion(known_peer_addresses);
        }

        const std::map<size_t, std::string> &map = chosen_election.getOptions();

        std::for_each(map.begin(), map.end(), [this](std::pair<size_t, std::string> idToOption) {
            _logger.displayData(std::to_string(idToOption.first) + ": " + idToOption.second);
        });

        std::string input;
        bool inputValid = true;
        std::size_t chosen_option;

        while(inputValid) {
            try {
                std::getline(std::cin, input);
                _logger.displayInput(input);
                chosen_option = std::stoi(input);
                inputValid = false;
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid argument: " << ia.what() << '\n';
            }
        }

        _logger.log("identity: " + peer_identity);


        std::string message = encryption_service.mapNumberStringToLetterString(input);
        input = encryption_service.fillMessage(message);
        std::string key;
        key = encryption_service.fillMessage(key);
        std::string cipher;
        cipher = encryption_service.fillMessage(cipher);

        _logger.log("filled message: " + input);
        if(encryption_service.generateKeyWithLGS(cipher, key, input)){
            own_election_keys[chosen_id] = key;

            std::string encrypted = encryption_service.encrypt(input, key);

            _logger.log("Cipher: " + cipher);
            _logger.log("Key: " + key);
            _logger.log("Encrypted Vote: " + encrypted);

            chosen_election.placeVote(peer_identity, encrypted);
        } else {
            _logger.log("Can't generate key message pair");
        }
    }
}

election peer::createElection(size_t election_id) {
    _logger.promptUserInput("Enter how many possible Options can be elected");

    std::string input;
    std::getline(std::cin, input);

    std::size_t number_of_options = std::atoi(input.c_str());
    std::vector<std::string> options(number_of_options);

    size_t index = 0;
    std::transform(options.begin(), options.end(), options.begin(),
                   [&index, this](const std::string &option) -> std::string {
                       this->_logger.promptUserInput("Enter Option #" + std::to_string(index));
                       std::string input;
                       std::getline(std::cin, input);
                       index++;
                       return input;
                   });

    nlohmann::json electionJson = nlohmann::json(options);
    _logger.displayData(electionJson.dump());

    std::map<size_t, std::string> map_options = std::map<size_t, std::string>();

    index = 0;
    if (electionJson.is_array()) {
        std::for_each(electionJson.begin(), electionJson.end(), [&map_options, &index, this](const nlohmann::json &option) {
            map_options[index] = option.dump();
            index++;
            _logger.displayData(option.dump());
        });
    }

    election initialElectionState = election::create(election_id)
            .withSequenceNumber(0)
            .withVoteOptions(map_options);

    _logger.log("show election");
    initialElectionState.print();

    return initialElectionState;
}


void peer::dumpElectionBox() {
    std::for_each(election_box.begin(), election_box.end(), [](election electionEntry) {
        electionEntry.print();
    });

    std::for_each(received_election_keys.begin(), received_election_keys.end(), [this](std::pair<size_t, std::vector<std::string>> election_id_received_keys){
        _logger.displayData(std::to_string(election_id_received_keys.first), "id:");
        _logger.displayVectorData(election_id_received_keys.second, "received keys:");
    });

    std::for_each(own_election_keys.begin(), own_election_keys.end(), [this](std::pair<size_t, std::string> election_id_key){
        _logger.displayData(std::to_string(election_id_key.first), "id:");
        _logger.displayData(election_id_key.second, "key:");
    });

    std::for_each(prepared_election_keys.begin(), prepared_election_keys.end(), [this](std::pair<size_t, std::queue<std::string>> election_id_keys_queue_map){
        _logger.displayData(std::to_string(election_id_keys_queue_map.first), "id:");
        _logger.displayData(std::to_string(election_id_keys_queue_map.second.size()), "keys size:");
    });
}

void peer::passiveDistribution(void *context, straightLineDistributeThread &thread) {
    thread.setInitialDistributer(false);
    thread.setContext(context);
    updateDistributionThread(&thread);
    thread.setIsRunning(true);
    thread.StartInternalThread();
}

size_t peer::selectElection() {
    _logger.promptUserInput("Select which election to distribute by id");
    size_t idx = 0;
    std::for_each(election_box.begin(), election_box.end(), [&idx, this](const election &current_election) {
        this->_logger.displayData("[" + std::to_string(idx) + "]: " + current_election.getElectionOptionsJson().dump() +
                                  ", with election_id="
                                  + std::to_string(current_election.getId()));
        idx++;
    });
    std::string input_string;
    size_t selected_election_id;

    while (true && input_string != "exit") {
        try {
            std::getline(std::cin, input_string);
            selected_election_id = std::stoi(input_string);
            if (isNumber(input_string) && selected_election_id < election_box.size()) {
                return selected_election_id;
            }
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << '\n';
        }
    }
    _logger.log("Exit executable during election selection");
    std::exit(10);
}

void peer::distributeElection(void *context, straightLineDistributeThread &thread, size_t chosen_election_id) {

    size_t selected_election_index;
    if (chosen_election_id != -1) {
        selected_election_index = chosen_election_id;
    } else {
        selected_election_index = selectElection();
    }
    election &chosen_election = election_box[selected_election_index];
    if (!chosen_election.isPreparedForDistribution()) {
        chosen_election.prepareForDistribtion(known_peer_addresses);
    }

    std::map<std::string, std::string> reversedConnectionTable;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversedConnectionTable](std::pair<std::string, std::string> addressToAddress) {
                      reversedConnectionTable[addressToAddress.second] = addressToAddress.first;
                  });

    std::string address_up;
    if (connection_table.contains(peer_address)) {
        address_up = connection_table[peer_address];
    }

    std::string address_down;
    if (reversedConnectionTable.contains(peer_address)) {
        address_down = reversedConnectionTable[peer_address];
    }

    calculatePositionFromTable();

    thread.setInitialDistributer(true);
    thread.setParams(context, address_up, address_down, position, known_peer_addresses.size(), chosen_election);
    thread.StartInternalThread();
    _logger.log("Distributing Election, wait for thread to exist", "localhost", "main");
    thread.WaitForInternalThreadToExit();
}

void peer::calculatePositionFromTable() {
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

    size_t pos = getAndIncrement(peer_address, root_address, reversed_connection_table, 0);

    this->position = pos;
    _logger.log("Calculated position for " + peer_address + " is " + std::to_string(pos));
}

size_t peer::getAndIncrement(std::string self_address, std::string current_address,
                             std::map<std::string, std::string> &connection_table, size_t current_position) {

    if (current_address == self_address || !connection_table.contains(current_address)) {
        return current_position;
    } else {
        current_position++;
        return getAndIncrement(self_address, connection_table[current_address], connection_table, current_position);
    }
}

bool peer::isNumber(const std::string s) {
    for (char const &ch: s) {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

void peer::pushBackElection(election election) {
    election_box.push_back(election);
}

void peer::startInprocElectionSyncThread(void *context, inprocElectionboxThread &thread) {
    _logger.log("starting election inproc");
    thread.StartInternalThread();
}

std::vector<election> &peer::getElectionBox() {
    return election_box;
}

void peer::updateDistributionThread(straightLineDistributeThread *p_thread) {
    calculatePositionFromTable();
    std::map<std::string, std::string> reversedConnectionTable;
    std::for_each(connection_table.begin(), connection_table.end(),
                  [&reversedConnectionTable](std::pair<std::string, std::string> addressToAddress) {
                      reversedConnectionTable[addressToAddress.second] = addressToAddress.first;
                  });

    std::string address_up;
    if (connection_table.contains(peer_address)) {
        address_up = connection_table[peer_address];
    }

    std::string address_down;
    if (reversedConnectionTable.contains(peer_address)) {
        address_down = reversedConnectionTable[peer_address];
    }

    p_thread->setAddressDown(address_down);
    p_thread->setAddressUp(address_up);
    p_thread->setPosition(position);
    p_thread->setNetworkSize(known_peer_addresses.size());
}

bool peer::eval_votes(replyKeyThread &replyThread, hillEncryptionService& encryption_service, size_t election_id) {
    size_t chosen_id = election_id;
    if(chosen_id == -1) {
        chosen_id = selectElection();
    }
    election &chosen_election = election_box.at(chosen_id);

    if (chosen_election.hasFreeEvaluationGroups() && !isEvaluatedVotesMap[chosen_id]) { // && peer not evaluated for the election yet
        chosen_election.addToNextEvaluationGroup(peer_address);
        generate_keys(encryption_service, chosen_id);
        replyThread.set_election_keys_queue(prepared_election_keys);
        _logger.log("Inside eval group");
        return true;
    } else {
        _logger.warn("Not inside eval group");
        return false;
    }
}

void peer::generate_keys(hillEncryptionService &encryption_service, size_t election_box_position) {
    size_t chosen_id = election_box_position;
    if(election_box_position == -1) {
       chosen_id = selectElection();
    }
    election &chosen_election = election_box.at(chosen_id);
    std::vector<std::string> participants_without_self;

    std::string identity = peer_identity;

    std::srand(std::time(nullptr)); // use current time as seed for random generator
    int random_variable = std::rand() % 3;

    size_t evaluation_group_size = 3;

    for (int i = 0; i < evaluation_group_size; ++i) {
        if(i == random_variable) {

            std::string key_string = own_election_keys[chosen_election.getId()];
            _logger.displayData(key_string, "Own Key: ");
            _logger.displayData(std::to_string(key_string.length()), "Own Key length: ");

            prepared_election_keys[chosen_election.getId()].push(key_string);

        } else {
            std::string wish_code = "ZAAA";
            std::vector<std::string> ciphers;
            std::transform(chosen_election.getVotes().begin(), chosen_election.getVotes().end(), std::back_inserter(ciphers), [](std::pair<std::string, std::string> identityToVote){
                return identityToVote.second;
            });
            std::string cipher = chosen_election.getVotes().at(peer_identity);
            std::string fake_key;
            encryption_service.generateFakeKeyWithLGS(ciphers, fake_key, wish_code);

            _logger.displayData(fake_key, "Key: ");
            prepared_election_keys[chosen_election.getId()].push(fake_key);
        }
    }
    _logger.log("Keys generated");
}

void peer::request_keys(void *args, size_t election_box_position) {
    _logger.log("Requesting keys for " + election_box_position);
    size_t chosen_id = election_box_position;
    if(election_box_position == -1) {
        chosen_id = selectElection();
    }
    election &chosen_election = election_box.at(chosen_id);
    std::vector<std::string> participants_without_self;

    std::string identity = peer_identity;

    std::for_each(chosen_election.getEvaluationGroups().begin(), chosen_election.getEvaluationGroups().end(),
                  [&identity, &participants_without_self](std::vector<std::string> group) {
                      if (std::find(group.begin(), group.end(), identity) != group.end()) {
                          std::copy_if(group.begin(), group.end(), std::back_inserter(participants_without_self),
                                       [&identity](const std::string& participant) { return participant != identity; });
                      }
                  });

    zmq::context_t *context = (zmq::context_t *) args;
    std::for_each(participants_without_self.begin(), participants_without_self.end(), [this, &chosen_id, &context](std::string participant){
        _logger.log("Send request towards " + participant);
        zmq_sleep(1);
        zmq::socket_t key_request_socket = zmq::socket_t(*context, zmq::socket_type::req);
        key_request_socket.connect("tcp://" + participant + ":50061");
        key_request_socket.send(zmq::buffer(std::to_string(chosen_id)),zmq::send_flags::none);

        zmq::message_t message;
        key_request_socket.recv(message);
        received_election_keys[chosen_id].push_back(message.to_string());
        _logger.log(message.to_string());
        key_request_socket.send(zmq::buffer("accepted"));
        _logger.log("accepted",participant);
        key_request_socket.disconnect("tcp://" + participant + ":50061");
        key_request_socket.close();
    });

    _logger.log("Has Received all the keys");
}

void peer::setIdentity(std::string identity) {
    peer_identity = identity;
}

void peer::reply_keys(void *args, replyKeyThread &thread) {
    thread.setParams(args, prepared_election_keys);
    thread.setIsRunning(true);
    thread.StartInternalThread();
}

void peer::countInVotes(zmq::context_t *p_context, straightLineDistributeThread &thread, hillEncryptionService &encryption_service, size_t election_box_position) {
    size_t chosen_id = election_box_position;
    if(election_box_position == -1) {
        chosen_id = selectElection();
    }
    _logger.log(&"Count in votes for " [chosen_id]);
    election &chosen_election = election_box.at(chosen_id);
    std::vector<std::string> participants_without_self;

    std::string nonce = std::to_string(chosen_id);
    chosen_election.countInVotesWithKeys(received_election_keys[chosen_id], encryption_service);

    //chosen_election.addToNextEvaluationGroup(peer_address);
    distributeElection(p_context, thread, chosen_id);

    std::string identity = peer_identity;
}

void peer::decrypt_vote(size_t i, hillEncryptionService &encryption_service) {
    std::string decrypted_vote = encryption_service.decrypt(election_box.at(i).getParticipantsVotes().at(peer_identity),own_election_keys[i]);
    _logger.displayData(decrypted_vote, "Own vote: ");
}

std::string& peer::getIdentity() {
    return peer_identity;
}