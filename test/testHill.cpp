//
// Created by wnabo on 10.06.2022.
//

#include "gtest/gtest.h"
#include "../src/evoting/hillEncryptionService.h"
#include "../src/evoting/hillLengthException.h"
#include <iostream>

TEST(hillTest, encryption) {
    hillEncryptionService bas;
    std::string result = bas.encrypt("BAAA","DDCF");
    EXPECT_EQ(result, "DDAA");
}

TEST(hillTest, decryption) {
    hillEncryptionService bas;
    std::string result = bas.decrypt("AACF","DDCF");
    EXPECT_EQ(result, "AAAB");
}

TEST(hillTest, encryptDecrypt) {
    hillEncryptionService bas;
    std::string encrypted = bas.encrypt("ABBB", "FRZC");
    std::cout << "encrypted: " << encrypted << std::endl;
    std::string result = bas.decrypt(encrypted, "FRZC");
    std::cout << "result: " << result << std::endl;
    EXPECT_EQ(result, "ABBB");
}

TEST(hillTest, validArgumentsNoException) {
    hillEncryptionService bas;
    EXPECT_NO_THROW(bas.encrypt("aaaa", "aaaa"));
    EXPECT_NO_THROW(bas.decrypt("aaaa", "aaaa"));
}

TEST(hillTest, decryption_throw_exceptions_on_invalid_arguments) {
    hillEncryptionService bas;
    EXPECT_THROW(bas.decrypt("aaaab", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaa", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaaa", "aaaab"), hillLengthException);
    EXPECT_THROW(bas.decrypt("aaaa", "aaa"), hillLengthException);
}

TEST(hillTest, encryption_throw_exceptions_on_invalid_arguments) {
    hillEncryptionService bas;
    EXPECT_THROW(bas.encrypt("aaaab", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaa", "aaaa"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaaa", "aaaab"), hillLengthException);
    EXPECT_THROW(bas.encrypt("aaaa", "aaa"), hillLengthException);
}

TEST(hillTest, testCharMapping) {
    hillEncryptionService bas;
    EXPECT_EQ(bas.mapCharToInt('a'), 0);
    EXPECT_EQ(bas.mapCharToInt('A'), 0);
    EXPECT_EQ(bas.mapCharToInt('z'), 25);
    EXPECT_EQ(bas.mapCharToInt('z'), 25);
}

TEST(hillTest, testStringMapping) {
    hillEncryptionService bas;
    const std::vector<int> &result = bas.mapStringToNumberSequence("abcd");
    EXPECT_EQ(result, std::vector({ 0, 1, 2, 3}));
}

TEST(hill, testFillMessage) {
    hillEncryptionService bas;
    std::string message = "A";
    std::string result = bas.fillMessage(message);
    std::cout << "Output: " << result << std::endl;
    EXPECT_TRUE(result.length() == 4);
    EXPECT_TRUE(result[0] == 'A');
}

TEST(hill, testFillMessage_no_fill) {
    hillEncryptionService bas;
    std::string message = "ABRD";
    std::string result = bas.fillMessage(message);
    std::cout << "Output: " << result << std::endl;
    EXPECT_TRUE(result.length() == 4);
    EXPECT_TRUE(result == "ABRD");
}

TEST(hill, testFillMessage_with_empty_message) {
    hillEncryptionService bas;
    std::string message;
    std::string result = bas.fillMessage(message);
    std::cout << "Output: " << result << std::endl;
    EXPECT_TRUE(result.length() == 4);
}

TEST(hill, testGeneratingInvalidCombination) {
    hillEncryptionService bas;
    std::string key = "AAAA";
    std::string cipher = "ZCET";
    std::string code = "BAAA";
    bool result = bas.generateKeyWithLGS(cipher, key, code);
    std::cout << "Cipher: " << cipher << std::endl;
    std::cout << "Code: " << code << std::endl;
    std::cout << "Key: " << key << std::endl;

    EXPECT_FALSE(result);
    EXPECT_TRUE(key == "ZZZZ");
}

TEST(hill, testGeneratingValidCombination) {
    hillEncryptionService bas;
    std::string key = "AAAA";
    std::string cipher = "ZCET";
    std::string code = "ABBB";
    bool result = bas.generateKeyWithLGS(cipher, key, code);
    std::cout << "Cipher: " << cipher << std::endl;
    std::cout << "Code: " << code << std::endl;
    std::cout << "Key: " << key << std::endl;

    EXPECT_TRUE(result);
    EXPECT_TRUE(key == "FRZC");
}

TEST(hill, testCheckValidFunction) {
    hillEncryptionService bas;
    int code[] = { 2, 14, 3, 4};
    int key[] = { 3, 3, 2, 5};
    bool result = bas.isValidMessageKeyCombination(code, key);
    EXPECT_TRUE(result);
}

TEST(hill, testMapNumberToLetterString) {
    hillEncryptionService bas;
    std::string number_string = "1234";
    std::string result = bas.mapNumberStringToLetterString(number_string);
    std::cout << "result: "<<  result << std::endl;
    EXPECT_TRUE(result == "BCDE");
}

TEST(hill, testGenerate) {
    hillEncryptionService bas;
    std::string wishing_code = "ZAAA";
    std::string key = "AAAA";
    std::vector<std::string> cipher = {"BHVG", "UPEE", "INRU", "OPBV"};
    bool result = bas.generateFakeKeyWithLGS(cipher, key, wishing_code);
    std::cout << "Generated key: " << key << std::endl;
    std::cout << "Will decrypt to: " << wishing_code << std::endl;
    EXPECT_TRUE(wishing_code[0] > 76);
}