//
// Created by wld on 23.03.23.
//
#include "gtest/gtest.h"
#include "../src/datasource/inMemoryStorage.h"
#include "../src/identity/identityService.h"

TEST(inMemoryStorage, addDocument) {
    inMemoryStorage storage;
    identityService id_service;

    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};
    did controller_id = did{"pvote","abcAddress234Timestamp234Nonce124"};
    didDocument didDocument = id_service.createDidDocument(id, controller_id);

    int result = storage.addDocument(id, didDocument);
    std::cout << didDocument << std::endl;
    ASSERT_EQ(result, 0);
}

TEST(inMemoryStorage, getAllDids) {
    inMemoryStorage storage;
    identityService id_service;

    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id, id);

    int result = storage.addDocument(id, didDocument);
    const std::set<did> &set = storage.getAllDIDs();

    ASSERT_EQ(result, 0);
    ASSERT_TRUE(set.size() == 1);
}

TEST(inMemoryStorage, didExists) {
    inMemoryStorage storage;
    identityService id_service;

    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id, id);

    int result = storage.addDocument(id, didDocument);

    ASSERT_EQ(result, 0);
    ASSERT_TRUE(storage.existDID(did{"pvote","abcAddress123Timestamp123Nonce123"}));
}

TEST(inMemoryStorage, addResource) {
    inMemoryStorage storage;
    identityService id_service;

    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id, id);

    bool add_resource_result = storage.addResource(id, "12.12.12.12");
    ASSERT_TRUE(add_resource_result);
}

TEST(inMemoryStorage, fetchResource) {
    inMemoryStorage storage;
    identityService id_service;

    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id, id);
    bool add_resource_result = storage.addResource(id, "12.12.12.12");
    ASSERT_TRUE(add_resource_result);
    
    std::string get_resource_result = storage.fetchResource(did{"pvote","abcAddress123Timestamp123Nonce123"});
    ASSERT_EQ(get_resource_result, "12.12.12.12");
}

TEST(inMemoryStorage, findAddressMatch) {
    inMemoryStorage storage;
    identityService id_service;
    did id = did{"pvote","abcAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id, id);
    int result = storage.addDocument(id, didDocument);
    ASSERT_EQ(result, 0);

    const std::vector<did> &result_vector = storage.findAddressMatch("Address123");
    ASSERT_TRUE(result_vector.size() == 1);
}

//TEST(inMemoryStorage, addResource) {
//    ASSERT_TRUE(storage.fetchResource(did{"pvote","abcAddress123Timestamp123Nonce123"}) == "12.12.12.12");
//}
