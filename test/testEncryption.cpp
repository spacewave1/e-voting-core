//
// Created by wnabo on 18.04.2022.
//

#include "gtest/gtest.h"
#include "../src/peer.h"
#include "../src/electionBuilder.h"
#include "sodium/crypto_aead_chacha20poly1305.h"

TEST(peerTest, encryption) {
    peer testee;

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    const electionBuilder &election = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options);

    testee.pushBackElection(election);

    unsigned char* string = (unsigned char *) "5";
    unsigned char encry[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    testee.encryptVote(1, reinterpret_cast<const char *>(string), encry);

    std::string convertToString = reinterpret_cast<const char *>(encry);

    unsigned char* ss = (unsigned char*) convertToString.c_str();

    unsigned char * res;
    testee.decryptVote(election, ss, res);

    char i = res[0];
    EXPECT_EQ(i, '5');
}

TEST(peerTest, encryptionAndJsonSerialization) {
    peer testee;

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";
    options[4] = "A";
    options[5] = "B";
    options[6] = "C";
    options[7] = "A";
    options[8] = "B";
    options[9] = "C";
    options[10] = "C";

    const election election = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options);

    testee.pushBackElection(election);


    unsigned char* string = (unsigned char *) "10";
    const size_t options_pot = ((int) election.getOptions().size()) / 10 + 1;
    unsigned char encry[options_pot + crypto_aead_chacha20poly1305_IETF_ABYTES];
    testee.encryptVote(1, reinterpret_cast<const char *>(string), encry);

    nlohmann::json encrypted_json;
    std::string cipher_text_string = reinterpret_cast<const char *>(encry);
    encrypted_json["cipher"] = cipher_text_string;
    
    unsigned char res[options_pot];
    std::string ciphertext = encrypted_json["cipher"].get<std::string>();
    unsigned char *str = (unsigned char *) ciphertext.c_str();
    testee.decryptVote(election, str, res);

    std::string resultString = reinterpret_cast<const char *>(res);
    EXPECT_EQ(resultString.substr(0,options_pot), "10");
}