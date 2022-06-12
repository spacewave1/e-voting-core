//
// Created by wnabo on 14.05.2022.
//

#include <sodium/crypto_aead_chacha20poly1305.h>
#include "libSodiumEncryptionService.h"
#include <iostream>
#include <vector>
#include <cstddef>
#include <sodium.h>
#include <sstream>

#define ADDITIONAL_DATA (const unsigned char *) "123456"
#define ADDITIONAL_DATA_LEN 6

std::string encryptWithBox(){

}

std::string
libSodiumEncryptionService::encrypt(const std::string &vote, const std::string &key_string, unsigned char *ciphertextPointer) {

    unsigned long long int ciphertext_len = 114U + crypto_aead_chacha20poly1305_IETF_ABYTES;
    const unsigned char *vote_str = (unsigned char *) vote.c_str();

    const unsigned char ad[12U]
            = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };

    std::cout << "nonce: " << nonce << std::endl;
    std::cout << "key_string: " << key_string << std::endl;
    std::cout << "message: " << vote_str << std::endl;
    std::cout << "message length: " << vote.length() << std::endl;

    crypto_aead_chacha20poly1305_ietf_encrypt(ciphertextPointer, &ciphertext_len, vote_str, vote.length(), ad, 12U, NULL,
                                              reinterpret_cast<const unsigned char *>(nonce.c_str()),
                                              reinterpret_cast<const unsigned char *>(key_string.c_str()));

    std::cout << "cipher: " << ciphertextPointer << std::endl;

    return reinterpret_cast<const char *>(ciphertextPointer);
}

unsigned char *
libSodiumEncryptionService::encryptBasic(unsigned char *vote, unsigned char *key, unsigned char *ciphertextPointer) {

    unsigned long long int ciphertext_len = 114U + crypto_aead_chacha20poly1305_IETF_ABYTES;

    std::cout << "message to encrypt: " << vote << std::endl;
    std::cout << "message length: " << 1 << std::endl;
    std::cout << "key: " << key << std::endl;
    std::cout << "key numbers: "; this->printCharSequenceAsIntArray(key, crypto_aead_chacha20poly1305_IETF_KEYBYTES); std::cout << std::endl;
    std::cout << "nonce: " << nonce.length() << std::endl;
    unsigned char * nonce_chars = (unsigned char *) nonce.c_str();
    std::cout << "nonce as numbers: "; this->printStringAsIntArray(nonce); std::cout << std::endl;

    crypto_aead_chacha20poly1305_ietf_encrypt(ciphertextPointer, &ciphertext_len, vote,
                                              114U, ADDITIONAL_DATA,
                                              ADDITIONAL_DATA_LEN, NULL, nonce_chars, key);

    std::cout << "cipher: " << ciphertextPointer << std::endl;
    std::cout << "cipher int: " ; printStringAsIntArray(std::string(reinterpret_cast<const char *>(ciphertextPointer))); std::cout << std::endl;
    std::cout << "ciphertext_len: " << ciphertext_len << std::endl;

    return ciphertextPointer;
}

std::string
libSodiumEncryptionService::decrypt(std::string encrypted_vote_string, const std::string &key, unsigned char *decrypt_pointer) const {

    unsigned long long decrypted_len = 114U;

    const unsigned char ad[12U] = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };

    unsigned long long int clen = 114U + crypto_aead_chacha20poly1305_IETF_ABYTES;

    toHexString(key); std::cout << std::endl;

    unsigned char * encrypted_vote = (unsigned char *) encrypted_vote_string.c_str();

    if (crypto_aead_chacha20poly1305_ietf_decrypt(decrypt_pointer, &decrypted_len, nullptr, encrypted_vote,
                                                  clen,
                                                  ad,
                                                  12U,
                                                  reinterpret_cast<const unsigned char *>(nonce.c_str()),
                                                  reinterpret_cast<const unsigned char *>(key.c_str())) != 0) {

        /* message forged! */
        std::cout << "message forged" << std::endl;
    }

    std::cout << "decrypted: " << decrypt_pointer << std::endl;
    return reinterpret_cast<const char *>(decrypt_pointer);
}

unsigned char *
libSodiumEncryptionService::decryptBasic(unsigned char * encrypted_vote, unsigned char * key, unsigned char *decrypt_pointer) const {

    unsigned long long decrypted_len = 114U;

    //const auto *ciphertext_cstr = reinterpret_cast<const unsigned char *>(encrypted_vote.c_str());

    unsigned long long clen = 114U + crypto_aead_chacha20poly1305_IETF_ABYTES;


    const unsigned char ad[12U]
            = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };

    if (crypto_aead_chacha20poly1305_ietf_decrypt(decrypt_pointer, &decrypted_len, nullptr, encrypted_vote,
                                                  clen,
                                                  ad, 12U,
                                                  reinterpret_cast<const unsigned char *>(nonce.c_str()), key) != 0) {

        /* message forged! */
        std::cout << "message forged" << std::endl;
    }
    std::cout << "decry length: " << decrypted_len << std::endl;
    std::cout << "decrypted: " << decrypt_pointer << std::endl;

    return decrypt_pointer;
}

unsigned char *libSodiumEncryptionService::hex2bin(std::string hexString, unsigned char *const pointer) {
    const char * const hex = hexString.c_str();
    sodium_hex2bin(pointer, crypto_aead_chacha20poly1305_ietf_KEYBYTES, hex, hexString.length(), NULL, NULL, NULL);
    return pointer;
}

unsigned char *libSodiumEncryptionService::hexKey2binKey(std::string hexString, unsigned char *const key_pointer) {
    const char * const hex = hexString.c_str();
    sodium_hex2bin(key_pointer, crypto_aead_chacha20poly1305_ietf_KEYBYTES, hex, hexString.length(), NULL, NULL, NULL);
    return key_pointer;
}

unsigned char *libSodiumEncryptionService::hexNonce2binNonce(std::string hexString, unsigned char *const nonce_pointer) {
    const char * const hex = hexString.c_str();
    sodium_hex2bin(nonce_pointer, crypto_aead_chacha20poly1305_ietf_NPUBBYTES, hex, hexString.length(), NULL, NULL, NULL);
    return nonce_pointer;
}

unsigned char *libSodiumEncryptionService::hexMax2binMac(std::string hexString, unsigned char *const mac_pointer) {
    const char * const hex = hexString.c_str();
    sodium_hex2bin(mac_pointer, crypto_aead_chacha20poly1305_ietf_ABYTES, hex, hexString.length(), NULL, NULL, NULL);
    return mac_pointer;
}

std::string libSodiumEncryptionService::decode(const std::string& str, unsigned char *decoded, unsigned int max_decoded_length) const {
    const char *c_string = reinterpret_cast<const char *>(str.c_str());
    const char *ignore;
    const char **p_string = NULL;
    sodium_base642bin(decoded, max_decoded_length, c_string, str.length(), ignore, NULL, p_string, sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    return reinterpret_cast<const char *>(decoded);
}

std::string libSodiumEncryptionService::encode(const std::string& str, char *encoded, unsigned int max_encoded_length) {
    const unsigned char *c_string = reinterpret_cast<const unsigned char *>(str.c_str());
    sodium_bin2base64(encoded, max_encoded_length, c_string, str.length(),
                      sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    return reinterpret_cast<const char *>(encoded);
}

unsigned char *libSodiumEncryptionService::generateKey(unsigned char *key) {
    // TODO: Check whether this valid code
    std::cout << "generate key " << std::endl;
    crypto_aead_chacha20poly1305_ietf_keygen(key);
    return key;
}

void libSodiumEncryptionService::setNonce(std::string new_nonce) {
    this->nonce = new_nonce;
};

void libSodiumEncryptionService::printCharSequenceAsIntArray(unsigned char * str, unsigned int str_length) const {
    std::byte bytes[str_length];
    for (size_t i = 0; i < str_length; i++) {
        bytes[i] = std::byte(str[i]);
    }

    for (auto &b: bytes) {
        std::cout << std::to_integer<int>(b) << ' ';
    }
}

void libSodiumEncryptionService::printStringAsIntArray(std::string str) const {
    std::byte bytes[str.size()];
    for (size_t i = 0; i < str.size(); i++) {
        bytes[i] = std::byte(str[i]);
    }

    for (auto &b: bytes) {
        std::cout << std::to_integer<int>(b) << ", ";
    }
}

std::string libSodiumEncryptionService::toHexString(const std::string& str) const {
    std::vector<std::byte> bytes;
    for (size_t i = 0; i < str.size(); i++) {
        bytes.push_back(std::byte(str[i]));
    }
    std::vector<int> integers;
    std::transform(bytes.begin(), bytes.end(), std::back_inserter(integers), [](std::byte bt){ return std::to_integer<int>(bt);});

    std::stringstream stream;
    std::for_each(integers.begin(), integers.end(),[&stream](unsigned int i){
        stream << "0x" <<  std::hex << i << " ";
    });
    std::string result( stream.str() );
    return result;
}

std::string libSodiumEncryptionService::toDecimalString(std::string str) const {
    std::vector<std::byte> bytes;
    for (size_t i = 0; i < str.size(); i++) {
        bytes.push_back(std::byte(str[i]));
    }
    std::vector<int> integers;
    std::transform(bytes.begin(), bytes.end(), std::back_inserter(integers), [](std::byte bt){ return std::to_integer<int>(bt);});

    std::stringstream stream;
    std::for_each(integers.begin(), integers.end(),[&stream](unsigned int i){
        stream << std::dec << i;
    });
    std::string result( stream.str() );
    std::cout << result;
    return result;
}

unsigned char *libSodiumEncryptionService::encodeBasic(unsigned char *str, char *encoded) {
    sodium_bin2base64(encoded, 180, str, (114U + crypto_aead_chacha20poly1305_ietf_ABYTES),
                      sodium_base64_VARIANT_URLSAFE);
    return reinterpret_cast<unsigned char *>(encoded);
}

int libSodiumEncryptionService::initLibSodium() {
    return sodium_init();
}
