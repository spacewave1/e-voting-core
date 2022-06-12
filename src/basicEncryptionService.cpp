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

std::pair<std::string, std::string> basicEncryptionService::generateKeyWithLGS(std::string cipher, std::string code) {
    std::vector<int> cipherNumbers = mapStringToNumberSequence(cipher);
    std::vector<int> codeNumbersVector = mapStringToNumberSequence(code);
    int codeNumbers[4] = { codeNumbersVector[0] % 26, codeNumbersVector[1] % 26, codeNumbersVector[2] % 26, codeNumbersVector[3] % 26};

    std::vector<int> key_vector = {0, 0, 0, 0};
    int key[4] = {1, 1, 1, 1};
    int shuffleCipher = 0;
    std::srand(std::time(nullptr)); // use current time as seed for random generator
    std::vector<bool> conditions = { false, false, false, false};
    bool foundCombination = false;

    size_t index = 0;
    unsigned long max = 26 * 26 * 26 * 26;
    while(shuffleCipher < 30 && !foundCombination) {
        foundCombination = false;
        for(size_t a = 0; a < 26 && !foundCombination; a++) {
            for(size_t b = 0; b < 26 && !foundCombination; b++) {
                for(size_t c = 0; c < 26 && !foundCombination; c++) {
                    for(size_t d = 0; d < 26; d++) {
                        key[0] = a;
                        key[1] = b;
                        key[2] = c;
                        key[3] = d;
                        conditions[0] = cipherNumbers[0] == ((codeNumbers[0] * key[0] + codeNumbers[1] * key[2]) % 26);
                        conditions[1] = cipherNumbers[1] == ((codeNumbers[0] * key[1] + codeNumbers[1] * key[3]) % 26);
                        conditions[2] = cipherNumbers[2] == ((codeNumbers[2] * key[0] + codeNumbers[3] * key[2]) % 26);
                        conditions[3] = cipherNumbers[3] == ((codeNumbers[2] * key[1] + codeNumbers[3] * key[3]) % 26);

                        if(std::count(conditions.begin(), conditions.end(), true) == 4) {
                            key_vector[0] = key[0];
                            key_vector[1] = key[1];
                            key_vector[2] = key[2];
                            key_vector[3] = key[3];
                            if(!hasInverseMatrixZeros(key_vector)) {
                                foundCombination = true;
                            }
                            break;
                        }
                        index++;
                    }
                }
                const std::string format = std::to_string(100 * index / max);
                printf("\r%s", format.c_str());
            }
        }
        if(!foundCombination) {
            cipherNumbers[0] = std::rand() % 26;
            cipherNumbers[1] = std::rand() % 26;
            cipherNumbers[2] = std::rand() % 26;
            cipherNumbers[3] = std::rand() % 26;
            std::cout << "Shuffled Cipher: " << shuffleCipher++ << std::endl;
        }
    }

    std::cout << "Key: (a = " << key[0] << ",b = " << key[1] << ",c = " << key[2] << ",d = " << key[3] << ")" << std::endl;
    std::cout << "With Cipher: ( " << cipherNumbers[0] << " " << cipherNumbers[1] << " " << cipherNumbers[2] << " " << cipherNumbers[3] << ")" << std::endl;
    std::cout << "has inverse zeroes: " << hasInverseMatrixZeros(key_vector) << std::endl;

    std::string newCipher = "0000";
    newCipher[0] = (char) (cipherNumbers[0] + 65);
    newCipher[1] = (char) (cipherNumbers[1] + 65);
    newCipher[2] = (char) (cipherNumbers[2] + 65);
    newCipher[3] = (char) (cipherNumbers[3] + 65);

    std::string key_string = "0000";
    key_string[0] = key[0] + 65;
    key_string[1] = key[1] + 65;
    key_string[2] = key[2] + 65;
    key_string[3] = key[3] + 65;
    return std::make_pair(key_string, newCipher);
}

std::string basicEncryptionService::generateKey() {
    std::vector<int> numbers = {0, 0, 0, 0};

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    while (hasInverseMatrixZeros(numbers)) {
        std::cout << "next key" << std::endl;
        numbers = {0, 0, 0, 0};
        std::transform(numbers.begin(), numbers.end(), numbers.begin(),
                       [](int number) -> unsigned int { return number + std::rand() % 26; });
        std::cout << numbers[0] << " " << numbers[1] << " " << numbers[2] << " " << numbers[3] << std::endl;
    }

    std::string key = "0000";
    key[0] = numbers[0] + 65;
    key[1] = numbers[1] + 65;
    key[2] = numbers[2] + 65;
    key[3] = numbers[3] + 65;
    return key;
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