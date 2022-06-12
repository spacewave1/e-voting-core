//
// Created by wnabo on 14.05.2022.
//

#ifndef VOTE_P2P_LIBSODIUMENCRYPTIONSERVICE_H
#define VOTE_P2P_LIBSODIUMENCRYPTIONSERVICE_H

#include <string>
#include <iostream>
#include <memory>
#include "sodium/crypto_aead_chacha20poly1305.h"

class libSodiumEncryptionService {
public:
    unsigned char* generateKey(unsigned char *key);
    std::string encrypt(const std::string& vote, const std::string& key_string, unsigned char* ciphertextPointer);
    unsigned char* encryptBasic(unsigned char *vote, unsigned char *key, unsigned char *ciphertextPointer);
    std::string decrypt(std::string encrypted_vote_string, const std::string& key, unsigned char *decrypt_pointer) const;
    unsigned char* decryptBasic(unsigned char * encrypted_vote, unsigned char * key, unsigned char *decrypt_pointer) const;
    std::string decode(const std::string& str, unsigned char *decoded, unsigned int max_decoded_length = 64) const;
    unsigned char* decodeBasic(unsigned char * str, unsigned char *decoded);
    std::string encode(const std::string& str, char *encoded, unsigned int max_encoded_length = 64);
    unsigned char* encodeBasic(unsigned char * str, char *encoded);
    void setNonce(std::string new_nonce);
    unsigned char *hexKey2binKey(std::string hexString, unsigned char *const bin_pointer);
    unsigned char *hex2bin(std::string hexString, unsigned char *const pointer);
    unsigned char *hexNonce2binNonce(std::string hexString, unsigned char *const nonce_pointer);
    unsigned char *hexMax2binMac(std::string hexString, unsigned char *const mac_pointer);
    std::string toHexString(const std::string & str) const;
    std::string toDecimalString(std::string str) const;
    int initLibSodium();
private:
    std::string nonce;

    void printStringAsIntArray(std::string str) const;
    void printCharSequenceAsIntArray(unsigned char * str, unsigned int str_length) const;


};


#endif //VOTE_P2P_LIBSODIUMENCRYPTIONSERVICE_H
