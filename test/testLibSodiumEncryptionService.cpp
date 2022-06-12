//
// Created by wnabo on 18.04.2022.
//

#include <sodium.h>
#include <regex>
#include <iostream>
#include <iterator>
#include "gtest/gtest.h"
#include "../src/peer.h"
#include "sodium/utils.h"

TEST(peerTest, encryption) {
    libSodiumEncryptionService encryption_service;

    unsigned char k[crypto_aead_chacha20poly1305_IETF_KEYBYTES];
    encryption_service.generateKey(k);
    std::string key_string(reinterpret_cast<const char *>(k));

    unsigned char * vote = (unsigned char *) "1";

    unsigned char encrypt_pointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned char *encrypted;
    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;

    encrypted = encryption_service.encryptBasic(vote, k, encrypt_pointer);
    std::string encrypted_string(reinterpret_cast<const char *>(encrypted));
    decrypted = encryption_service.decryptBasic(encrypted, k, decrypted_pointer);

    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '1');
}

TEST(peerTest, encryption_decryption_with) {
    libSodiumEncryptionService encryption_service;

    unsigned char k[crypto_aead_chacha20poly1305_IETF_KEYBYTES];
    encryption_service.generateKey(k);
    std::string key_string(reinterpret_cast<const char *>(k));

    std::string vote = "1";

    unsigned char encrypt_pointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned char *encrypted;
    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;

    encryption_service.setNonce("1");
    encrypted = (unsigned char *) encryption_service.encrypt(vote, key_string, encrypt_pointer).c_str();
    std::string encrypted_string(reinterpret_cast<const char *>(encrypted));
    decrypted = (unsigned char*) encryption_service.decrypt(encrypted_string, key_string, decrypted_pointer).c_str();

    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '1');
}

TEST(peerTest, encryptionAndJsonSerialization) {
    libSodiumEncryptionService encryption_service;

    unsigned char k[crypto_aead_chacha20poly1305_IETF_KEYBYTES];
    encryption_service.generateKey(k);
    std::string key_string(reinterpret_cast<const char *>(k));

    std::string vote = "1";

    unsigned char *encrypted;
    unsigned char encrypt_pointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned char *encodedEncrypted;
    char encodedPointer[64];

    unsigned char *decodedEncrypted;
    unsigned char decodedPointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];

    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;
    std::string nonce = std::to_string(1);
    std::cout << "nonce: " << nonce << std::endl;

    encryption_service.setNonce(nonce);
    unsigned char *string = (unsigned char *) "1";
    encrypted = encryption_service.encryptBasic(string, k, encrypt_pointer);
    std::string encryptedString(reinterpret_cast<const char *>(encrypted));
    encodedEncrypted = (unsigned char *) encryption_service.encode(encryptedString, encodedPointer).c_str();

    std::cout << "Base64 Encoded: " << encodedEncrypted << std::endl;
    std::string encodedEncrypted_string(reinterpret_cast<const char *>(encodedEncrypted));
    std::cout << "Base64 Encoded String: " << encodedEncrypted << std::endl;

    std::string encodedEncryptedString(reinterpret_cast<const char *>(encodedEncrypted));

    decodedEncrypted = (unsigned char*) encryption_service.decode(reinterpret_cast<const char *>(encodedEncrypted), decodedPointer).c_str();
    std::cout << "Base64 Decoded Binary: " << decodedEncrypted << std::endl;
    decrypted = encryption_service.decryptBasic(decodedEncrypted, k, decrypted_pointer);

    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '1');
}

TEST(peerTest, encrypt_encode_decode_decrypt) {
    libSodiumEncryptionService encryption_service;
    std::string vote = "1";

    unsigned char generatedKey[crypto_aead_chacha20poly1305_IETF_KEYBYTES];
    encryption_service.generateKey(generatedKey);
    std::string keyStr(reinterpret_cast<const char *>(generatedKey));

    char encodedKeyPointer[64];
    unsigned char* encodedKey = (unsigned char *) encryption_service.encode(keyStr, encodedKeyPointer).c_str();
    std::string encodedKeyString(reinterpret_cast<const char *>(encodedKey));

    unsigned char encrypt_pointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned char *encrypted;
    unsigned char *encodedEncrypted;
    char encodedPointer[64];

    unsigned char *decodedEncrypted;
    unsigned char decodedPointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];

    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;

    encryption_service.setNonce("0");

    encrypted = (unsigned char *) encryption_service.encrypt(vote, encodedKeyString, encrypt_pointer).c_str();
    std::string encrypted_string(reinterpret_cast<const char *>(encrypted));
    encodedEncrypted = (unsigned char *) encryption_service.encode(encrypted_string, encodedPointer).c_str();

    std::cout << "Base64 Encoded: " << encodedEncrypted << std::endl;
    std::string encodedEncrypted_string(reinterpret_cast<const char *>(encodedEncrypted));
    std::cout << "Base64 Encoded String: " << encodedEncrypted << std::endl;

    encryption_service.setNonce("0");

    decodedEncrypted = (unsigned char*) encryption_service.decode(encodedEncrypted_string, decodedPointer).c_str();
    std::cout << "Base64 Decoded Binary: " << decodedEncrypted << std::endl;
    std::string decodedEncryptedString(reinterpret_cast<const char *>(decodedEncrypted));
    decrypted = (unsigned char*) encryption_service.decrypt(decodedEncryptedString, encodedKeyString, decrypted_pointer).c_str();

    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '1');
}

TEST(peerTest, encrypt_encode_decode_decrypt_json_serialization_also_with_key) {
    libSodiumEncryptionService encryption_service;
    std::string vote = "1";

    std::string encodedKeyString = "!bgOhMHjiAT7UL56Xd1OrByqIj28F7/vRtpb0IejfOX0=";

    unsigned char encrypt_pointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];
    unsigned char *encrypted;
    unsigned char *encodedEncrypted;
    char encodedPointer[64];

    unsigned char *decodedEncrypted;
    unsigned char decodedPointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];

    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;
    std::stringstream temp_str;
    temp_str << 0;

    encryption_service.setNonce("1");

    encrypted = (unsigned char *) encryption_service.encrypt(vote, encodedKeyString, encrypt_pointer).c_str();
    std::string encrypted_string(reinterpret_cast<const char *>(encrypted));
    encodedEncrypted = (unsigned char *) encryption_service.encode(encrypted_string, encodedPointer).c_str();

    std::cout << "Base64 Encoded: " << encodedEncrypted << std::endl;
    std::string encodedEncrypted_string(reinterpret_cast<const char *>(encodedEncrypted));
    std::cout << "Base64 Encoded String: " << encodedEncrypted << std::endl;

    encryption_service.setNonce("1");

    decodedEncrypted = (unsigned char*) encryption_service.decode(encodedEncrypted_string, decodedPointer).c_str();
    std::cout << "Base64 Decoded Binary: " << decodedEncrypted << std::endl;
    std::string decodedEncryptedString(reinterpret_cast<const char *>(decodedEncrypted));
    decrypted = (unsigned char*) encryption_service.decrypt(decodedEncryptedString, encodedKeyString, decrypted_pointer).c_str();

    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '1');
}

TEST(peerTest, decode_decrypt_election) {
    libSodiumEncryptionService encryption_service;

    std::string key = "F3oVOUeWCxszvC4QiWyI0XszWPr3ejqJ6Pmv3ydswGY=";
    unsigned char key_c[64] = "F3oVOUeWCxszvC4QiWyI0XszWPr3ejqJ6Pmv3ydswGY=";
    std::string vote = "KvopjNWB-LvVkA3w5wECaBDY____fwAAUQAAAAAAAAA=";
    unsigned char vote_c[64] = "KvopjNWB-LvVkA3w5wECaBDY____fwAAUQAAAAAAAAA=";

    encryption_service.setNonce("0");
    unsigned char *decodedEncrypted;
    unsigned char decodedPointer[1 + crypto_aead_chacha20poly1305_IETF_ABYTES];

    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;

    decodedEncrypted = (unsigned char*) encryption_service.decode(vote, decodedPointer).c_str();
    std::cout << "Base64 Decoded Binary: " << decodedEncrypted << std::endl;
    std::string decodedEncryptedString(reinterpret_cast<const char *>(decodedEncrypted));
    decrypted = (unsigned char*) encryption_service.decrypt(decodedEncryptedString, key, decrypted_pointer).c_str();
    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '2');
};

TEST(peerTest, decrypt_election) {
    libSodiumEncryptionService encryption_service;

    std::string key = "ujqg_F0GtigHqdvj9bKM66zZu1S9ZTmkMy3zyzaygHQ=";
    unsigned int ints_arr[] = {7, 3, 176, 236, 25, 178, 245, 59,
                               240, 237, 145, 177, 143, 9, 18, 140,
                               166, 227, 255, 255, 255, 127 };
    std::vector<unsigned int> ints(std::begin(ints_arr), std::end(ints_arr));
    std::vector<unsigned char> chars;

    std::cout << "{";
    std::transform(ints.begin(), ints.end(), std::back_inserter(chars),[](int i){
        std::cout << i << " ";
        return i;
    });
    unsigned char* a = &chars[0];
    ;
    std::cout << std::endl;

    std::string vote(reinterpret_cast<const char *>(a));
    std::string vote_substr = vote.substr(0, vote.length() - 3);

    encryption_service.setNonce("0");

    unsigned char decrypted_pointer[1];
    unsigned char *decrypted;

    decrypted = (unsigned char*) encryption_service.decrypt(vote_substr, key, decrypted_pointer).c_str();
    std::string result(reinterpret_cast<const char *>(decrypted));

    EXPECT_EQ(result.at(0), '2');
};

TEST(peerTest, encrypt_encode_libsodium_example) {
    libSodiumEncryptionService encryption_service;

    if(encryption_service.initLibSodium() != -1) {
        const unsigned char key[crypto_aead_chacha20poly1305_ietf_KEYBYTES]
                = {
                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
                };

        std::string key_string(reinterpret_cast<const char *>(key));
        std::string message = "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.";

        const unsigned char nonce[crypto_aead_chacha20poly1305_ietf_NPUBBYTES]
                = { 0x07, 0x00, 0x00, 0x00,
                    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };

        unsigned char * encrypted = (unsigned char *) sodium_malloc(114U + crypto_aead_chacha20poly1305_ietf_ABYTES);
        char * encoded = (char *) sodium_malloc(180);

        encryption_service.setNonce(reinterpret_cast<const char *>(nonce));
        std::string encrypted_string = encryption_service.encrypt(message, key_string, encrypted);

        //std::cout << "Bin: " << encrypted_string << std::endl;
        //std::cout << "Hex: " << encryption_service.toHexString(encrypted_string) << std::endl;

        std::string result = encryption_service.encode(encrypted_string, encoded, 180);

        std::cout << "Base64: " << result << std::endl;

        EXPECT_EQ(result, "NBKPNhvGlKwzbK8R4poPiI6z2vHZyEASeEmmTm82XUJVzfH9LwmjGqFEf7e-cX9yzRagp6fEAxaV_7eX-OwJBhZRUCxKdCv5XlICqGn0JIFDB8bgC41B27Okt8aP8TBTHxhxdnhupTKZc7Dj0wyHRLOq2CjOwFoTouo-TCarLs9fOw");

        sodium_free(encoded);
        sodium_free(encrypted);
    } else {
        std::cout << "Libsodium was not init" << std::endl;
        GTEST_FAIL();
    }
}

TEST(peerTest, decode_decrypt_libsodium_example) {
    libSodiumEncryptionService encryption_service;
    if(encryption_service.initLibSodium() != -1) {
        const unsigned char key[crypto_aead_chacha20poly1305_ietf_KEYBYTES]
                = {
                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
                };
        const unsigned char nonce[crypto_aead_chacha20poly1305_ietf_NPUBBYTES]
                = { 0x07, 0x00, 0x00, 0x00,
                    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };


        std::string encodedString = "NBKPNhvGlKwzbK8R4poPiI6z2vHZyEASeEmmTm82XUJVzfH9LwmjGqFEf7e-cX9yzRagp6fEAxaV_7eX-OwJBhZRUCxKdCv5XlICqGn0JIFDB8bgC41B27Okt8aP8TBTHxhxdnhupTKZc7Dj0wyHRLOq2CjOwFoTouo-TCarLs9fOw";
        std::string key_string(reinterpret_cast<const char *>(key));
        encryption_service.setNonce(reinterpret_cast<const char *>(nonce));

        unsigned char * decoded = (unsigned char *) sodium_malloc(114U + crypto_aead_chacha20poly1305_ietf_ABYTES);
        unsigned char * decrypted = (unsigned char *) sodium_malloc(114U);
        std::cout << "encoded string length " << encodedString << std::endl;

        std::string decoded_string = encryption_service.decode(encodedString, decoded, 114U + crypto_aead_chacha20poly1305_ietf_ABYTES);
        std::cout << "decoded string: " <<  decoded_string << std::endl;
        std::cout << "decoded hex: " << encryption_service.toHexString(decoded_string) << std::endl;

        //std::regex re("(ff*)7f$");
        //const std::string &regex_replaced_decoded_vote = std::regex_replace(decoded_hex_string, re, "");

        std::string decrypted_string = reinterpret_cast<const char *>(encryption_service.decryptBasic(decoded,
                                                                                                      const_cast<unsigned char *>(key),
                                                                                                      decrypted));
        std::cout << "Decrypted: " <<  decrypted_string << std::endl;
        EXPECT_EQ("","");
        sodium_free(decoded);
        sodium_free(decrypted);
    } else {
        std::cout << "Cout not init libsodium" << std::endl;
        GTEST_FAIL();
    }
    GTEST_FAIL();
}

TEST(peerTest, decode_decrypt_with_box_libsodium_example) {
    libSodiumEncryptionService encryption_service;
    #define MESSAGE_LEN 7
    #define CIPHERTEXT_LEN (crypto_box_SEALBYTES + MESSAGE_LEN)

    unsigned char recipient_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char recipient_sk[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(recipient_pk, recipient_sk);

    std::string encodedString = "QA9ag3_tKLj6aizMUXKFRzwx1oZvlOLEJf6QG1-cVnM4xUGCfGIKZv014k6ehQ83r0xB53ciyg";

    unsigned char * decoded = (unsigned char *) sodium_malloc(CIPHERTEXT_LEN);

    std::string decodedResult = encryption_service.decode(encodedString, decoded, CIPHERTEXT_LEN);

/* Recipient decrypts the ciphertext */
    unsigned char decrypted[MESSAGE_LEN];
    int counter = 0;
    while(crypto_box_seal_open(decrypted, reinterpret_cast<const unsigned char *>(decodedResult.c_str()), CIPHERTEXT_LEN,
                               recipient_pk, recipient_sk) == 0 || counter < 10)
    {
        counter++;
        std::cout << decrypted << std::endl;
        /* message corrupted or not intended for this recipient */
    }
    std::string result(reinterpret_cast<const char *>(decrypted));
    EXPECT_EQ(result, "Message");
}

TEST(peerTest, easy_secret_box) {
    libSodiumEncryptionService encryption_service;
    if(encryption_service.initLibSodium() != -1) {
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4
#define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

        unsigned char alice_publickey[crypto_box_PUBLICKEYBYTES];
        unsigned char alice_secretkey[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(alice_publickey, alice_secretkey);

        char encoded_alice_pubkey[30];
        char encoded_alice_secretkey[30];
        std::string alicePublickey_string(reinterpret_cast<const char *>(alice_publickey));
        std::string aliceSecretkey_string(reinterpret_cast<const char *>(alice_secretkey));

        unsigned char bob_publickey[crypto_box_PUBLICKEYBYTES];
        unsigned char bob_secretkey[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(bob_publickey, bob_secretkey);

        char encoded_bob_pubkey[30];
        char encoded_bob_secretkey[30];
        std::string bobPublickey_string(reinterpret_cast<const char *>(bob_publickey));
        std::string bobSecretkey_string(reinterpret_cast<const char *>(bob_secretkey));

        std::cout << "public encoded len: " << sodium_base64_ENCODED_LEN(crypto_box_PUBLICKEYBYTES, sodium_base64_VARIANT_URLSAFE) << std::endl;

        std::string encoded_alice_pubkey_string = encryption_service.encode(alicePublickey_string, encoded_alice_pubkey, 100);
        std::string encoded_alice_secret_string = encryption_service.encode(aliceSecretkey_string, encoded_alice_secretkey, 100);

        std::cout << "alice public: " << encoded_alice_pubkey_string << std::endl;
        std::cout << "alice priv: " << encoded_alice_secret_string << std::endl;

        std::string encoded_bob_pubkey_string = encryption_service.encode(bobPublickey_string, encoded_bob_pubkey, 100);
        std::string encoded_bop_secret_string = encryption_service.encode(bobSecretkey_string, encoded_bob_secretkey, 100);

        std::cout << "bob public: " << encoded_bob_pubkey_string << std::endl;
        std::cout << "bob priv: " << encoded_bop_secret_string << std::endl;

        unsigned char * nonce = (unsigned char *) "asdgghjghj";
        unsigned char ciphertext[CIPHERTEXT_LEN];
        //randombytes_buf(nonce, sizeof nonce);

        if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce,
                            bob_publickey, alice_secretkey) != 0) {
            /* error */
            std::cout << "Error on encryption" << std::endl;
        }

        std::cout << "ciphertext" << ciphertext << std::endl;

        char * encoded = (char *) sodium_malloc(sodium_base64_ENCODED_LEN(CIPHERTEXT_LEN, sodium_base64_VARIANT_URLSAFE));


        std::string ciphertext_string(reinterpret_cast<const char *>(ciphertext));
        std::string encodedResult = encryption_service.encode(ciphertext_string, encoded, 33);
        std::cout << "encded result: " << encodedResult << std::endl;

        unsigned char * decoded = (unsigned char *) sodium_malloc(CIPHERTEXT_LEN);

        std::string decodedResult = encryption_service.decode(encodedResult, decoded, CIPHERTEXT_LEN);

        std::cout << "Ciphertext: " << ciphertext_string << std::endl;
        std::cout << "Decoded result: " << decodedResult << std::endl;

        unsigned char decrypted[MESSAGE_LEN];
        if (crypto_box_open_easy(decrypted, reinterpret_cast<const unsigned char *>(decodedResult.c_str()), CIPHERTEXT_LEN, nonce,
                                 alice_publickey, bob_secretkey) != 0) {
            /* message for Bob pretending to be from Alice has been forged! */
        }
        std::string result(reinterpret_cast<const char *>(decrypted));
        EXPECT_EQ(result.substr(0, MESSAGE_LEN), "test");
    } else {
        std::cout << "Could not init libsodium" << std::endl;
        GTEST_FAIL();
    }
}

TEST(peerTest, easy_secret_encode_decode_keys_box) {
    libSodiumEncryptionService encryption_service;
    if(encryption_service.initLibSodium() != -1) {
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4
#define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

        unsigned char alice_publickey[crypto_box_PUBLICKEYBYTES];
        unsigned char alice_secretkey[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(alice_publickey, alice_secretkey);

        char encoded_alice_pubkey[30];
        char encoded_alice_secretkey[30];
        std::string alicePublickey_string(reinterpret_cast<const char *>(alice_publickey));
        std::string aliceSecretkey_string(reinterpret_cast<const char *>(alice_secretkey));

        unsigned char bob_publickey[crypto_box_PUBLICKEYBYTES];
        unsigned char bob_secretkey[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(bob_publickey, bob_secretkey);

        char encoded_bob_pubkey[30];
        char encoded_bob_secretkey[30];
        std::string bobPublickey_string(reinterpret_cast<const char *>(bob_publickey));
        std::string bobSecretkey_string(reinterpret_cast<const char *>(bob_secretkey));

        std::cout << "public encoded len: " << sodium_base64_ENCODED_LEN(crypto_box_PUBLICKEYBYTES, sodium_base64_VARIANT_URLSAFE) << std::endl;

        std::string encoded_alice_pubkey_string = encryption_service.encode(alicePublickey_string, encoded_alice_pubkey, 100);
        std::string encoded_alice_secret_string = encryption_service.encode(aliceSecretkey_string, encoded_alice_secretkey, 100);

        std::cout << "alice public: " << encoded_alice_pubkey_string << std::endl;
        std::cout << "alice priv: " << encoded_alice_secret_string << std::endl;

        std::string encoded_bob_pubkey_string = encryption_service.encode(bobPublickey_string, encoded_bob_pubkey, 100);
        std::string encoded_bop_secret_string = encryption_service.encode(bobSecretkey_string, encoded_bob_secretkey, 100);

        std::cout << "bob public: " << encoded_bob_pubkey_string << std::endl;
        std::cout << "bob priv: " << encoded_bop_secret_string << std::endl;

        unsigned char * nonce = (unsigned char *) "asdgghjghj";
        unsigned char ciphertext[CIPHERTEXT_LEN];
        //randombytes_buf(nonce, sizeof nonce);

        if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce,
                            bob_publickey, alice_secretkey) != 0) {
            /* error */
            std::cout << "Error on encryption" << std::endl;
        }

        std::cout << "ciphertext" << ciphertext << std::endl;

        char * encoded = (char *) sodium_malloc(sodium_base64_ENCODED_LEN(CIPHERTEXT_LEN, sodium_base64_VARIANT_URLSAFE));


        std::string ciphertext_string(reinterpret_cast<const char *>(ciphertext));
        std::string encodedResult = encryption_service.encode(ciphertext_string, encoded, 33);
        std::cout << "encded result: " << encodedResult << std::endl;

        unsigned char * decoded = (unsigned char *) sodium_malloc(CIPHERTEXT_LEN);

        std::string decodedResult = encryption_service.decode(encodedResult, decoded, CIPHERTEXT_LEN);

        std::cout << "Ciphertext: " << ciphertext_string << std::endl;
        std::cout << "Decoded result: " << decodedResult << std::endl;

        unsigned char decodedAlicePublicKey_pointer[crypto_box_PUBLICKEYBYTES];
        unsigned char decodedBobSecretKey_pointer[crypto_box_SECRETKEYBYTES];

        std::string decodedPublickeyAlice = encryption_service.decode(encoded_alice_pubkey_string, decodedAlicePublicKey_pointer, crypto_box_PUBLICKEYBYTES);
        std::string decodedsecretBob = encryption_service.decode(encoded_bop_secret_string, decodedBobSecretKey_pointer, crypto_box_SECRETKEYBYTES);

        unsigned char decrypted[MESSAGE_LEN];
        if (crypto_box_open_easy(decrypted, reinterpret_cast<const unsigned char *>(decodedResult.c_str()), CIPHERTEXT_LEN, nonce,
                                 reinterpret_cast<const unsigned char *>(decodedPublickeyAlice.c_str()),
                                 reinterpret_cast<const unsigned char *>(decodedsecretBob.c_str())) != 0) {
            /* message for Bob pretending to be from Alice has been forged! */
        }
        std::string result(reinterpret_cast<const char *>(decrypted));
        std::cout << "Result String: " <<  result << std::endl;
        EXPECT_EQ(result.substr(0, MESSAGE_LEN), "test");
    } else {
        std::cout << "Could not init libsodium" << std::endl;
        GTEST_FAIL();
    }
}

TEST(peerTest, easy_secret_box_not_generated_keys) {
    libSodiumEncryptionService encryption_service;
    if(encryption_service.initLibSodium() != -1) {
#define MESSAGE (const unsigned char *) "test"
#define MESSAGE_LEN 4
#define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

        std::string alicePublickey_string = "1Dw5IhcgD3M7YKZ1A29pgIb8kBlGuYF9PBzVHslEuyPtXlbCEDK5tzq-FKfdk7Z8j6NfEWkd_I_OFgnkAYC7EWDGLJDaVQ";
        std::string aliceSecretkey_string = "7V5WwhAyubc6vhSn3ZO2fI-jXxFpHfyPzhYJ5AGAuxFgxiyQ2lU";

        unsigned char decoded_alice_pubkey[crypto_box_PUBLICKEYBYTES];
        unsigned char decoded_alice_secretkey[crypto_box_SECRETKEYBYTES];

        std::string decoded_alice_pubkey_string = encryption_service.decode(alicePublickey_string, decoded_alice_pubkey, crypto_box_PUBLICKEYBYTES);
        std::string decoded_alice_secret_string = encryption_service.decode(alicePublickey_string, decoded_alice_secretkey, crypto_box_SECRETKEYBYTES);

        std::string bobPublickey_string = "cJATajiFqj2ILo79Cnh5wUvKfnPbAuJ2q65pDSoiAiXFLEwS2Rx6dZsO8w6b1f8P8Erc9FqM1sfUHLmoWiCmncDdLJDaVQ";
        std::string bobSecretkey_string = "xSxMEtkcenWbDvMOm9X_D_BK3PRajNbH1By5qFogpp3A3SyQ2lU";
        unsigned char decoded_bob_pubkey[crypto_box_PUBLICKEYBYTES];
        unsigned char decoded_bob_secretkey[crypto_box_SECRETKEYBYTES];

        std::string decoded_bob_pubkey_string = encryption_service.decode(bobPublickey_string, decoded_bob_pubkey, crypto_box_PUBLICKEYBYTES);
        std::string decoded_bob_secret_string = encryption_service.decode(bobPublickey_string, decoded_bob_secretkey, crypto_box_SECRETKEYBYTES);


        unsigned char * nonce = (unsigned char *) "asdgghjghj";
        unsigned char ciphertext[CIPHERTEXT_LEN];
        //randombytes_buf(nonce, sizeof nonce);

        if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce,
                            reinterpret_cast<const unsigned char *>(decoded_bob_pubkey_string.c_str()),
                            reinterpret_cast<const unsigned char *>(decoded_alice_secret_string.c_str())) != 0) {
            /* error */
            std::cout << "Error on encryption" << std::endl;
        }

        std::cout << "ciphertext" << ciphertext << std::endl;

        char * encoded = (char *) sodium_malloc(sodium_base64_ENCODED_LEN(CIPHERTEXT_LEN, sodium_base64_VARIANT_URLSAFE));


        std::string ciphertext_string(reinterpret_cast<const char *>(ciphertext));
        std::string encodedResult = encryption_service.encode(ciphertext_string, encoded, 33);
        std::cout << "encded result: " << encodedResult << std::endl;

        unsigned char * decoded = (unsigned char *) sodium_malloc(CIPHERTEXT_LEN);

        std::string decodedResult = encryption_service.decode(encodedResult, decoded, CIPHERTEXT_LEN);

        std::cout << "Ciphertext: " << ciphertext_string << std::endl;
        std::cout << "Decoded result: " << decodedResult << std::endl;

        unsigned char decrypted[MESSAGE_LEN];
        if (crypto_box_open_easy(decrypted, reinterpret_cast<const unsigned char *>(decodedResult.c_str()), CIPHERTEXT_LEN, nonce,
                                 reinterpret_cast<const unsigned char *>(decoded_alice_pubkey_string.c_str()),
                                 reinterpret_cast<const unsigned char *>(decoded_bob_secret_string.c_str())) != 0) {
            /* message for Bob pretending to be from Alice has been forged! */
        }
        std::string result(reinterpret_cast<const char *>(decrypted));
        EXPECT_EQ(result.substr(0, MESSAGE_LEN), "test");
    } else {
        std::cout << "Could not init libsodium" << std::endl;
        GTEST_FAIL();
    }
}
