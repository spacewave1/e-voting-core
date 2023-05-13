//
// Created by wld on 26.02.23.
//

#include "electionService.h"

bool electionService::placeEncryptedVote(std::string input, election &chosen_election,
                                         std::map<size_t, std::string> &own_election_keys, std::string peer_identity) {
    std::string message = encryption_service.mapNumberStringToLetterString(input);
    input = encryption_service.fillMessage(message);
    std::string key;
    key = encryption_service.fillMessage(key);
    std::string cipher;
    cipher = encryption_service.fillMessage(cipher);

    _logger.log("filled message: " + input);
    if (encryption_service.generateKeyWithLGS(cipher, key, input)) {
        own_election_keys[chosen_election.getId()] = key;

        std::string encrypted = encryption_service.encrypt(input, key);

        _logger.log("Cipher: " + cipher);
        _logger.log("Key: " + key);
        _logger.log("Encrypted Vote: " + encrypted);
        _logger.log("id: " + peer_identity);

        chosen_election.placeVote(peer_identity, encrypted);
        return true;
    } else {
        _logger.log("Can't generate key message pair");
        return false;
    }
}

void
electionService::decryptWithKeys(election &chosen_election, std::string peer_identity, std::vector<std::string> &keys) {
    std::vector<std::string> participants_without_self;

    std::string nonce = std::to_string(chosen_election.getId());
    //chosen_election.countInVotesWithKeys(election_keys, encryption_service);

    std::vector<std::string> votes;
    std::transform(chosen_election.getParticipantsVotes().begin(), chosen_election.getParticipantsVotes().end(),
                   std::back_inserter(votes), [](const std::pair<std::string, std::string> &idToVote) -> std::string {
                std::cout << idToVote.first << std::endl;
                std::cout << idToVote.second << std::endl;
                return idToVote.second;
            });
    size_t index = 0;
    std::for_each(votes.begin(), votes.end(), [&keys, &chosen_election, &index, this](std::string vote) {
        index++;
        std::for_each(keys.begin(), keys.end(), [vote, &chosen_election, this](std::string key) {
            std::cout << "Encrypted vote: " << vote << std::endl;
            std::cout << "key: " << key << std::endl;

            std::string decrypted_vote = encryption_service.decrypt(vote, key);

            std::cout << "Decrypted: " << decrypted_vote << std::endl;
            int ascii_decimal = encryption_service.mapCharToInt(decrypted_vote[0]);
            if (ascii_decimal < chosen_election.getOptions().size() && ascii_decimal >= 0) {
                std::cout << "Count in the vote: " << decrypted_vote.substr(0, log(chosen_election.getOptions().size()))
                          << std::endl;
                std::vector<int> integers = encryption_service.mapStringToNumberSequence(
                        decrypted_vote.substr(0, log(chosen_election.getOptions().size())));
                std::string vote_at_id_str;
                std::transform(integers.begin(), integers.end(), std::back_inserter(vote_at_id_str), [](int number) {
                    return (char) number + 48;
                });
                std::cout << "Increment one  for: " << vote_at_id_str << std::endl;
                int vote_at_id = std::stoi(vote_at_id_str);
                chosen_election.getElectionResult().at(vote_at_id) += 1;
            }
        });
    });
}

electionService::electionService(const hillEncryptionService &encryptionService) : encryption_service(
        encryptionService) {}
