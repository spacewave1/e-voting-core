//
// Created by wnabo on 10.06.2022.
//

#include <iostream>
#include <vector>
#include "basicEncryptionService.h"
#include <ctime>
#include <cstring>
#include "hillLengthException.h"

std::string basicEncryptionService::encrypt(std::string message_string, std::string key_string) {
    if (message_string.length() != 4 || key_string.length() != 4) {
        throw hillLengthException();
    }
    const std::vector<int> message = mapStringToNumberSequence(message_string);
    const std::vector<int> key = mapStringToNumberSequence(key_string);
    int codeNumbers[] = {0, 0, 0, 0};
    codeNumbers[0] = (message[0] * key[0] + message[1] * key[2]) % 26;
    codeNumbers[1] = (message[0] * key[1] + message[1] * key[3]) % 26;
    codeNumbers[2] = (message[2] * key[0] + message[3] * key[2]) % 26;
    codeNumbers[3] = (message[2] * key[1] + message[3] * key[3]) % 26;

    std::string code = "0000";
    code[0] = (char) (codeNumbers[0] + 65);
    code[1] = (char) (codeNumbers[1] + 65);
    code[2] = (char) (codeNumbers[2] + 65);
    code[3] = (char) (codeNumbers[3] + 65);
    return code;
}

std::string basicEncryptionService::decrypt(std::string cipher_string, std::string key_string) const {
    if (cipher_string.length() != 4 || key_string.length() != 4) {
        throw hillLengthException();
    }
    const std::vector<int> cipher = mapStringToNumberSequence(cipher_string);
    const std::vector<int> key = mapStringToNumberSequence(key_string);
    std::string code = "0000";
    int inverseKey[4];
    int det = (key[0] * key[3] - key[1] * key[2]);
    int inverseDet, y;

    if (gcdExtended(det, 26, &inverseDet, &y)) {
        inverseKey[0] = (key[3] * inverseDet) % 26;
        inverseKey[1] = ((-key[1] * inverseDet) + 260) % 26;
        inverseKey[2] = ((-key[2] * inverseDet) + 260) % 26;
        inverseKey[3] = (key[0] * inverseDet) % 26;

        int codeNumbers[4];

        codeNumbers[0] = ((cipher[0] * inverseKey[0] + cipher[1] * inverseKey[2]) % 26);
        codeNumbers[1] = ((cipher[0] * inverseKey[1] + cipher[1] * inverseKey[3]) % 26);
        codeNumbers[2] = ((cipher[2] * inverseKey[0] + cipher[3] * inverseKey[2]) % 26);
        codeNumbers[3] = ((cipher[2] * inverseKey[1] + cipher[3] * inverseKey[3]) % 26);

        code[0] = (char) (codeNumbers[0] + 65);
        code[1] = (char) (codeNumbers[1] + 65);
        code[2] = (char) (codeNumbers[2] + 65);
        code[3] = (char) (codeNumbers[3] + 65);
    }
    return code;
}

std::vector<int> basicEncryptionService::mapStringToNumberSequence(std::string str) const {
    std::vector<int> numbers;
    std::for_each(str.begin(), str.end(), [this, &numbers](char character) {
        numbers.push_back(mapCharToInt(character));
    });
    return numbers;
}

int basicEncryptionService::mapCharToInt(char character) const {
    if (character >= 65 && character <= 90) {
        return character - 65;
    } else if (character >= 97 && character <= 122) {
        return character - 97;
    }
}

bool basicEncryptionService::isValidMessageKeyCombination(int codeNumbers[], int key[]) {
    std::string code_string = {static_cast<char>(codeNumbers[0] + 65),
                               static_cast<char>(codeNumbers[1] + 65),
                               static_cast<char>(codeNumbers[2] + 65),
                               static_cast<char>(codeNumbers[3] + 65),
    };
    std::string key_string = {static_cast<char>(key[0] + 65),
                              static_cast<char>(key[1] + 65),
                              static_cast<char>(key[2] + 65),
                              static_cast<char>(key[3] + 65),
    };
    return decrypt(encrypt(code_string, key_string), key_string) == code_string;
}

bool basicEncryptionService::findInverseKeyCombination(std::string& key, std::vector<std::string> ciphers) {
    std::vector<bool> conditions;
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    int a_offset = std::rand() % 26;
    int b_offset = std::rand() % 26;
    int c_offset = std::rand() % 26;
    int d_offset = std::rand() % 26;

    _logger.displayData(std::to_string(a_offset), "rand: ");
    _logger.displayData(std::to_string(b_offset), "rand: ");
    _logger.displayData(std::to_string(c_offset), "rand: ");
    _logger.displayData(std::to_string(d_offset), "rand: ");


    for (int a = 0; a < 26; a++) {
        for (int b = 0; b < 26; b++) {
            for (int c = 0; c < 26; c++) {
                for (int d = 0; d < 26; d++) {
                    std::vector<int> numbers {a,b,c,d};

                    if(!hasInverseMatrixZeros(numbers)){

                        std::string keyFromNumbers = "0000";
                        keyFromNumbers[0] = (char) ((a + a_offset) % 26 + 65);
                        keyFromNumbers[1] = (char) ((b + b_offset) % 26 + 65);
                        keyFromNumbers[2] = (char) ((c + c_offset) % 26 + 65);
                        keyFromNumbers[3] = (char) ((d + d_offset) % 26 + 65);

                        std::transform(ciphers.begin(), ciphers.end(), std::back_inserter(conditions), [this, keyFromNumbers](const std::string& cipher){
                            return mapCharToInt(decrypt(cipher, keyFromNumbers)[0]) > 10 &&
                                        mapCharToInt(decrypt(cipher, keyFromNumbers)[1]) < 26 &&
                                        mapCharToInt(decrypt(cipher, keyFromNumbers)[2]) < 26 &&
                                        mapCharToInt(decrypt(cipher, keyFromNumbers)[3]) < 26;
                        });

                        if(std::count(conditions.begin(), conditions.end(), true) == 4) {
                            _logger.displayData(keyFromNumbers);
                            key[0] = keyFromNumbers[0];
                            key[1] = keyFromNumbers[1];
                            key[2] = keyFromNumbers[2];
                            key[3] = keyFromNumbers[3];

                            return true;
                        }

                        conditions.clear();
                    }
                }
            }
        }
    }
    return false;
}

bool basicEncryptionService::findKeyCombination(int *key, std::vector<bool> conditions, int codeNumbers[4],
                                                std::vector<int> cipherNumbers) {
    std::vector<int> key_vector = {1, 1, 1, 1};
    for (size_t a = 0; a < 26; a++) {
        for (size_t b = 0; b < 26; b++) {
            for (size_t c = 0; c < 26; c++) {
                for (size_t d = 0; d < 26; d++) {
                    key[0] = a;
                    key[1] = b;
                    key[2] = c;
                    key[3] = d;

                    conditions[0] = cipherNumbers[0] == ((codeNumbers[0] * key[0] + codeNumbers[1] * key[2]) % 26);
                    conditions[1] = cipherNumbers[1] == ((codeNumbers[0] * key[1] + codeNumbers[1] * key[3]) % 26);
                    conditions[2] = cipherNumbers[2] == ((codeNumbers[2] * key[0] + codeNumbers[3] * key[2]) % 26);
                    conditions[3] = cipherNumbers[3] == ((codeNumbers[2] * key[1] + codeNumbers[3] * key[3]) % 26);

                    if (std::count(conditions.begin(), conditions.end(), true) == 4) {
                        key_vector[0] = key[0];
                        key_vector[1] = key[1];
                        key_vector[2] = key[2];
                        key_vector[3] = key[3];
                        if (!hasInverseMatrixZeros(key_vector) && isValidMessageKeyCombination(codeNumbers, key)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool basicEncryptionService::generateFakeKeyWithLGS(std::vector<std::string> ciphers, std::string &key_string, std::string &code) {
    std::vector<int> wish_code_numbers = mapStringToNumberSequence(code);

    int codeNumbers[4] = {wish_code_numbers[0] % 26, wish_code_numbers[1] % 26, wish_code_numbers[2] % 26,
                          wish_code_numbers[3] % 26};

    std::string key = "0000";
    int shuffleKey = 30;
    bool foundCombination = false;

    while(shuffleKey > 0 && !foundCombination) {
        foundCombination = findInverseKeyCombination(key, ciphers);
        if (!foundCombination) {
            codeNumbers[0] = std::rand() % 26;
            codeNumbers[0] = codeNumbers[0] < 10 ? codeNumbers[0] + 10 : codeNumbers[0];

            codeNumbers[1] = std::rand() % 26;
            codeNumbers[2] = std::rand() % 26;
            codeNumbers[3] = std::rand() % 26;
            _logger.log("Shuffles left: " + std::to_string(shuffleKey--));
        }
    }

    std::string newKeyString = "0000";
    newKeyString[0] = key[0];
    newKeyString[1] = key[1];
    newKeyString[2] = key[2];
    newKeyString[3] = key[3];

    key_string = newKeyString;
    return foundCombination;
}

bool basicEncryptionService::generateKeyWithLGS(std::string &cipher, std::string &key_string, std::string code) {
    std::vector<int> cipherNumbers = mapStringToNumberSequence(cipher);
    std::vector<int> codeNumbersVector = mapStringToNumberSequence(code);
    int codeNumbers[4] = {codeNumbersVector[0] % 26, codeNumbersVector[1] % 26, codeNumbersVector[2] % 26,
                          codeNumbersVector[3] % 26};
    std::vector<int> key_vector = {1, 1, 1, 1};

    int key[4] = {1, 1, 1, 1};
    int shuffleCipher = 30;
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    std::vector<bool> conditions = {false, false, false, false};
    bool foundCombination = false;
    while (shuffleCipher > 0 && !foundCombination) {
        foundCombination = findKeyCombination(key, conditions, codeNumbers, cipherNumbers);
        if (!foundCombination) {
            cipherNumbers[0] = std::rand() % 26;
            cipherNumbers[1] = std::rand() % 26;
            cipherNumbers[2] = std::rand() % 26;
            cipherNumbers[3] = std::rand() % 26;
            _logger.log("Shuffles left: " + std::to_string(shuffleCipher--));
        }
    }

    key_vector[0] = key[0];
    key_vector[1] = key[1];
    key_vector[2] = key[2];
    key_vector[3] = key[3];

    _logger.log("Key: (a = " + std::to_string(key[0]) + ",b = " + std::to_string(key[1]) + ",c = " + std::to_string(key[2]) + ",d = " + std::to_string(key[3])+ ")");

    _logger.log("With Cipher: ( " + std::to_string(cipherNumbers[0]) + " " + std::to_string(cipherNumbers[1]) + " " + std::to_string(cipherNumbers[2]) + " " + std::to_string(cipherNumbers[3]) + ")");

    _logger.log("has inverse zeroes: " + std::to_string(hasInverseMatrixZeros(key_vector)));

    std::string newCipher = "0000";
    newCipher[0] = (char) (cipherNumbers[0] + 65);
    newCipher[1] = (char) (cipherNumbers[1] + 65);
    newCipher[2] = (char) (cipherNumbers[2] + 65);
    newCipher[3] = (char) (cipherNumbers[3] + 65);

    cipher = newCipher;

    std::string newKeyString = "0000";
    newKeyString[0] = key[0] + 65;
    newKeyString[1] = key[1] + 65;
    newKeyString[2] = key[2] + 65;
    newKeyString[3] = key[3] + 65;

    key_string = newKeyString;
    return foundCombination;
}

std::string basicEncryptionService::fillMessage(std::string &message) {
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    size_t padding_length = 4 - message.length();
    std::string padding(padding_length, 'Z');
    std::transform(padding.begin(), padding.end(), std::inserter(padding, padding.begin()), [](char character) {
        return (char) ((int) ((std::rand() % 26) + 65));
    });
    message = message + padding.substr(0, padding_length);
    return message;
}

bool basicEncryptionService::hasInverseMatrixZeros(std::vector<int> numbers) {
    int inverseKey[4];
    int det = (numbers[0] * numbers[3] - numbers[1] * numbers[2]);
    int inverseDet, y;

    if (gcdExtended(det, 26, &inverseDet, &y)) {
        inverseKey[0] = (numbers[3] * inverseDet) % 26;
        inverseKey[1] = ((-numbers[1] * inverseDet) + 260) % 26;
        inverseKey[2] = ((-numbers[2] * inverseDet) + 260) % 26;
        inverseKey[3] = (numbers[0] * inverseDet) % 26;

        return inverseKey[0] == 0 || inverseKey[1] == 0 || inverseKey[2] == 0 || inverseKey[3] == 0;
    }
    return true;
}


// See: https://www.geeksforgeeks.org/euclidean-algorithms-basic-and-extended/
// Function for extended Euclidean Algorithm
int basicEncryptionService::gcdExtended(int a, int b, int *x, int *y) const {
    // Base Case
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }

    int x1, y1; // To store results of recursive call
    int gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of
    // recursive call
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

std::string basicEncryptionService::mapNumberStringToLetterString(std::string number_string) {
    std::string letter_string;
    std::transform(number_string.begin(), number_string.end(), std::back_inserter(letter_string),
                   [](char character) {
                       return character - 48 + 65;
                   });
    return letter_string;
}
