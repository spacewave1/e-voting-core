//
// Created by wnabo on 10.06.2022.
//

#include "gtest/gtest.h"
#include "../src/basicEncryptionService.h"
#include "../src/hillLengthException.h"
#include <iostream>

TEST(hillTest, encryption) {
    basicEncryptionService bas;
    std::string result = bas.encrypt("BAAA","DDCF");
    EXPECT_EQ(result, "DDAA");
}

TEST(hillTest, decryption) {
    basicEncryptionService bas;
    std::string result = bas.decrypt("AACF","DDCF");
    EXPECT_EQ(result, "AAAB");
}

TEST(hillTest, encryptDecrypt) {
    basicEncryptionService bas;
    std::pair<std::string, std::string> key_and_cipher = bas.generateKeyWithLGS("ZCET", "ABFR");
    std::cout << "key: " << key_and_cipher.first << std::endl;
    std::cout << "cipher: " << key_and_cipher.second << std::endl;
    std::string encrypted = bas.encrypt("ABFR", key_and_cipher.first);
    std::cout << "encrypted: " << encrypted << std::endl;
    std::string result = bas.decrypt(encrypted, key_and_cipher.first);
    std::cout << "result: " << result << std::endl;
    EXPECT_EQ(result, "ABFR");
}

TEST(hillTest, validArgumentsNoException) {
    basicEncryptionService bas;
    EXPECT_NO_THROW(bas.encrypt("aaaa", "aaaa"));
    EXPECT_NO_THROW(bas.decrypt("aaaa", "aaaa"));
}

TEST(hillTest, decryption_throw_exceptions_on_invalid_arguments) {
    basicEncryptionService bas;
    EXPECT_THROW(bas.decrypt("aaaab", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaa", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaaa", "aaaab"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaaa", "aaa"), hillLengthException);
}

TEST(hillTest, encryption_throw_exceptions_on_invalid_arguments) {
    basicEncryptionService bas;
    EXPECT_THROW(bas.encrypt("aaaab", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaa", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaaa", "aaaab"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaaa", "aaa"), hillLengthException);
}

TEST(hillTest, testCharMapping) {
    basicEncryptionService bas;
    EXPECT_EQ(bas.mapCharToInt('a'), 0);
    EXPECT_EQ(bas.mapCharToInt('A'), 0);
    EXPECT_EQ(bas.mapCharToInt('z'), 25);
    EXPECT_EQ(bas.mapCharToInt('z'), 25);
}

TEST(hillTest, testStringMapping) {
    basicEncryptionService bas;
    const std::vector<int> &result = bas.mapStringToNumberSequence("abcd");
    EXPECT_EQ(result, std::vector({ 0, 1, 2, 3}));
}

TEST(hillTest, testRandomKeyInConstraints) {
    basicEncryptionService bas;
    const std::string &key = bas.generateKey();
    std::cout << key << std::endl;

    EXPECT_TRUE(key.length() <= 4);

    std::for_each(key.begin(), key.end(), [](char character){
        EXPECT_TRUE(character >= 65 && character <= 90);
    });
}

TEST(hill, testKeysWithLGS) {
    basicEncryptionService bas;
    const std::string cipher = "ZCET";
    const char *code = "AAAB";
    std::pair<std::string, std::string> result = bas.generateKeyWithLGS(cipher, code);
    std::cout << "Cipher: " << result.second << std::endl;
    std::cout << "Code: " << code << std::endl;
    std::cout << "Key: " << result.first << std::endl;
    EXPECT_EQ(result.first, "DCDF");
}