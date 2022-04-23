//
// Created by wnabo on 18.04.2022.
//

#include "gtest/gtest.h"
#include "../src/peer.h"
#include "../src/electionBuilder.h"
#include "sodium/crypto_aead_chacha20poly1305.h"
#include "sodium/utils.h"

std::string to_utf8(std::wstring& wide_string)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(wide_string);
}

TEST(peerTest, encryption) {
    peer testee;

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    election election = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options);

    testee.pushBackElection(election);

    unsigned char* string = (unsigned char *) "5";
    unsigned char encry[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    testee.encryptVote( election, reinterpret_cast<const char *>(string), encry);

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
    testee.encryptVote(const_cast<class election &>(election), reinterpret_cast<const char *>(string), encry);

    nlohmann::json encrypted_json;
    std::string base_64_encoded_encrypted = reinterpret_cast<const char *>(encry);
    encrypted_json["cipher"] = base_64_encoded_encrypted;
    
    std::cout << encrypted_json.dump() << std::endl;
    
    unsigned char decoded[options_pot + crypto_aead_chacha20poly1305_IETF_ABYTES];

    const char* ignore;
    const char **p_string = NULL;

    std::cout << "Dump: " << encrypted_json.dump() << std::endl;

    unsigned char res[options_pot];
    std::string ciphertext = encrypted_json["cipher"].get<std::string>();
    std::cout << "Ciphertext: " << ciphertext << std::endl;

    sodium_base642bin(decoded, 64 / 4 * 3,
                      static_cast<const char *>(encrypted_json["cipher"].get<std::string>().c_str()), 64,
                      ignore, NULL, p_string, sodium_base64_VARIANT_ORIGINAL);

    std::cout << "Decoded: " << decoded << std::endl;

    testee.decryptVote(election, decoded, res);

    std::cout << "decrypted: " << res << std::endl;

    std::string resultString = reinterpret_cast<const char *>(res);
    EXPECT_EQ(resultString.substr(0,options_pot), "10");
}