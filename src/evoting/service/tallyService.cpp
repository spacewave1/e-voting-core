//
// Created by wld on 21.02.23.
//

#include <queue>
#include "tallyService.h"

size_t tallyService::receiveKeyRequest(abstractSocket& socket){
    std::string result = socket.recv().payload;
    _logger.log("Received: " + result);

    size_t election_id = std::stoi(result);
    return election_id;
}

void tallyService::keyResponse(abstractSocket& socket, size_t election_id, std::map<size_t, std::queue<std::string>>* prepared_election_keys){
    std::string data = prepared_election_keys->at(election_id).front();

    socket.send(data);
    prepared_election_keys->at(election_id).pop();
}

void tallyService::keyResponseFinish(abstractSocket& socket) {
    std::string message = socket.recv().payload;
    _logger.log(message);
    socket.close();
}

void tallyService::requestKey(abstractSocket& socket, election& chosen_election, std::string identity, std::string participant){
    _logger.log("Send request towards " + participant);
    socket.connect("tcp", participant, 50061);
    socket.send(std::to_string(chosen_election.getId()));
}

void tallyService::receiveKey(abstractSocket& socket, size_t election_id, std::map<size_t, std::vector<std::string>>& received_election_keys){
    const socketMessage &message = socket.recv();
    received_election_keys[election_id].push_back(message.payload);
    _logger.log(message.payload);
    socket.send("accepted");
    socket.close();
}

std::vector<std::string> tallyService::findGroupAndFilterOwnIdentity(election& chosen_election, std::string identity_self){
    std::vector<std::string> participants_without_self;

    std::for_each(chosen_election.getEvaluationGroups().begin(), chosen_election.getEvaluationGroups().end(),
                  [&identity_self, &participants_without_self](std::vector<std::string> group) {
                      if (std::find(group.begin(), group.end(), identity_self) != group.end()) {
                          std::copy_if(group.begin(), group.end(), std::back_inserter(participants_without_self),
                                       [&identity_self](const std::string& participant) { return participant != identity_self; });
                      }
                  });
    return participants_without_self;
}

void tallyService::tally(abstractSocket& socket, election& chosen_election, std::string identity, std::vector<std::string> participants_without_self, std::map<size_t, std::vector<std::string>>& received_election_keys){
    //TODO: Check whether participants without self has some side effects
    std::for_each(participants_without_self.begin(), participants_without_self.end(), [this, &chosen_election, &socket, &participants_without_self, &received_election_keys, &identity](std::string participant){
        requestKey(socket, chosen_election, identity, participant);
        receiveKey(socket, chosen_election.getId(),  received_election_keys);
    });
}

bool tallyService::prepareTally(election &chosen_election,
                                std::map<size_t, bool> is_evaluated_votes_map,
                                std::string peer_address, std::map<size_t, std::string>& own_election_keys,
                                std::map<size_t, std::queue<std::string>>& prepared_election_keys) {
    if (chosen_election.hasFreeEvaluationGroups() && !is_evaluated_votes_map[chosen_election.getId()]) { // && peer not evaluated for the election yet
        chosen_election.addToNextEvaluationGroup(peer_address);
        generate_keys(chosen_election, peer_address, own_election_keys, prepared_election_keys);
        //replyThread.set_election_keys_queue(prepared_election_keys);

        // TODO: keys need to be migrated to correct application
        //prepared_election_keys.insert(std::make_pair(chosen_election.getId(), keys));

        _logger.log("Inside eval group");
        return true;
    } else {
        _logger.warn("Not inside eval group");
        return false;
    }
}

void tallyService::generate_keys(election &chosen_election, std::string peer_identity,
                                 std::map<size_t, std::string>& own_election_keys,
                                 std::map<size_t, std::queue<std::string>>& prepared_election_keys) {

    std::srand(std::time(nullptr)); // use current time as seed for random generator
    int random_variable = std::rand() % 3;

    size_t evaluation_group_size = 3;

    for (int i = 0; i < evaluation_group_size; ++i) {
        if (i == random_variable) {

            std::string key_string = own_election_keys[chosen_election.getId()];
            _logger.displayData(key_string, "Own Key: ");
            _logger.displayData(std::to_string(key_string.length()), "Own Key length: ");

            prepared_election_keys[chosen_election.getId()].push(key_string);

        } else {
            std::string wish_code = "ZAAA";
            std::vector<std::string> ciphers;
            std::transform(chosen_election.getVotes().begin(), chosen_election.getVotes().end(),
                           std::back_inserter(ciphers), [](std::pair<std::string, std::string> identityToVote) {
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

tallyService::tallyService(hillEncryptionService &encryptionService) : encryption_service(encryptionService) {}
