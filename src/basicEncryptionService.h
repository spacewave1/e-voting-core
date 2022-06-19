//
// Created by wnabo on 10.06.2022.
//

#ifndef VOTE_P2P_BASICENCRYPTIONSERVICE_H
#define VOTE_P2P_BASICENCRYPTIONSERVICE_H

#include <string>

class basicEncryptionService {
public:
    std::string encrypt(std::string message_string, std::string key_string);
    std::string decrypt(std::string cipher_string, std::string key_string) const;
    std::vector<int> mapStringToNumberSequence(std::string str) const;
    int mapCharToInt(char character) const;

    bool hasInverseMatrixZeros(std::vector<int> numbers);

    int gcdExtended(int a, int b, int *x, int *y) const;

    bool generateKeyWithLGS(std::string & cipher, std::string &key_string, std::string code);
    bool generateFakeKeyWithLGS(std::vector<std::string> ciphers, std::string &key_string, std::string &code);

    bool findKeyCombination(int *key, std::vector<bool> conditions, int *codeNumbers,
                            std::vector<int> cipherNumbers);

    bool findInverseKeyCombination(std::string& key, std::vector<std::string> ciphers);

    std::string fillMessage(std::string& message);

    bool isValidMessageKeyCombination(int *codeNumbers, int *key);

    std::string mapNumberStringToLetterString(std::string number_string);
};

#endif //VOTE_P2P_BASICENCRYPTIONSERVICE_H
