//
// Created by wld on 09.03.23.
//

extern "C" {
// Get declaration for f(int i, char c, float x)
    #include "../../crypt/sha.h"
}

#include "hashService.h"
#include <cstring>

#define SHA256_SEED "\xf4\x1e\xce\x26\x13\xe4\x57\x39\x15\x69\x6b" \
  "\x5a\xdc\xd5\x1c\xa3\x28\xbe\x3b\xf5\x66\xa9\xca\x99\xc9\xce\xb0" \
  "\x27\x9c\x1c\xb0\xa7"

std::string hashService::hashMessage(std::string message) {
    USHAContext sha;
    //const char *testarray = message.c_str();
    int length = message.length();
    int repeatcount = 1;
    int err, i;
    int extrabits = 0;
    int numberExtrabits = 0;
    uint8_t Message_Digest_Buf[32];
    uint8_t *Message_Digest = Message_Digest_Buf;
    std::string outputString;
    char buf[20];
    memset(&sha, '\343', sizeof(sha)); /* force bad data into struct */
    USHAReset(&sha, SHA256);

    for (i = 0; i < repeatcount; ++i) {
        err = USHAInput(&sha, (const uint8_t *) message.c_str(),length);
        if (err != shaSuccess) {
            fprintf(stderr, "hash(): %sInput Error %d.\n","sha", err);
            //throw err;
            return "error";
        }
    }
    if (numberExtrabits > 0) {
        err = USHAFinalBits(&sha, (uint8_t) extrabits,
                            numberExtrabits);
        if (err != shaSuccess) {
            fprintf(stderr, "hash(): %sFinalBits Error %d.\n", "sha", err);
            return "error";
            //return err;
        }
    }
    err = USHAResult(&sha, Message_Digest);
    if (err != shaSuccess) {
        fprintf(stderr, "hash(): %s Result Error %d, could not compute "
                        "message digest.\n", "sha", err);
        //return err;
        return "error";
    }

    int hashsize = 32;
    unsigned char b;

    char *sm = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
               "0123456789+/";
    for (i = 0; i < hashsize; i += 3) {
        outputString += sm[Message_Digest[i] >> 2];
        b = (Message_Digest[i] & 0x03) << 4;
        if (i+1 < hashsize) b |= Message_Digest[i+1] >> 4;
        outputString += sm[b];
        if (i+1 < hashsize) {
            b = (Message_Digest[i+1] & 0x0f) << 2;
            if (i+2 < hashsize) b |= Message_Digest[i+2] >> 6;
            outputString += sm[b];
        } else outputString += '=';
        if (i+2 < hashsize) outputString += sm[Message_Digest[i+2] & 0x3f];
        else outputString += '=';

    }

    return outputString;
}
