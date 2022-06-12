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

    std::string generateKey();

    bool hasInverseMatrixZeros(std::vector<int> numbers);

    int gcdExtended(int a, int b, int *x, int *y) const;

    std::pair<std::string, std::string> generateKeyWithLGS(std::string cipher, std::string code);

    void gaussElimiation();
};

#endif //VOTE_P2P_BASICENCRYPTIONSERVICE_H
