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

TEST(inMemoryStorage, didExistsVersionAgnostic) {
    inMemoryStorage storage;
    identityService id_service;

    did id_1 = did{"pvote","abcAddress123Timestamp123Nonce123"};
    did id_2 = did{"pvote","bcdAddress123Timestamp123Nonce123"};
    did id_3 = did{"pvote","qweAddress123Timestamp123Nonce123"};

    didDocument didDocument = id_service.createDidDocument(id_1, id_2);

    int result_2 = storage.addDocument(id_1.withVersion(2), didDocument);

    ASSERT_TRUE(storage.existDIDInAnyVersion(id_1));
    ASSERT_FALSE(storage.existDIDInAnyVersion(id_2));
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

TEST(inMemoryStorage, testGetChainUp) {
    inMemoryStorage storage;
    identityService id_service;
    
    did id_1{"pvote","abcd"};
    did id_2{"pvote","cdef"};
    
    const didDocument &document_a = id_service.createDidDocument(id_1, id_2);
    storage.addDocument(id_1, document_a);
    did id_3{"pvote","efgh"};
    
    const didDocument &document_b = id_service.createDidDocument(id_2, id_3);
    storage.addDocument(id_2, document_b);
    did id_4{"pvote","ghij"};
    
    const didDocument &document_c = id_service.createDidDocument(id_3, id_4);
    storage.addDocument(id_3, document_c);

    const std::map<did, did> &result_map = storage.getDIDChainUp(id_1);
    ASSERT_EQ(result_map.size(), 3);
}

TEST(inMemoryStorage, getChainDown) {
    inMemoryStorage storage;
    identityService id_service;

    did id_1{"did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_2{"did:pvote:123IyM3iW5Ue6RJoNIBmA+X+Cvl0/R8qAqtX0XmhZkUEnA=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_3{"did:pvote:123p1IcVjNmLrf2nz5yxTsNH2vDCjGC82CxV3i6s0yZWYw=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};

    const didDocument &document_a = id_service.createDidDocument(id_1, id_2);
    const didDocument &document_b = id_service.createDidDocument(id_2, id_3);
    const didDocument &document_c = id_service.createDidDocument(id_3, id_3);

    storage.addDocument(id_1, document_a);
    storage.addDocument(id_2, document_b);
    storage.addDocument(id_3, document_c);

    const std::map<did, did> &result_map = storage.getDIDChainDown();
    ASSERT_EQ(result_map.size(), 2);
    ASSERT_TRUE(result_map.at(id_3) == id_2);
    ASSERT_TRUE(result_map.at(id_2) == id_1);
}

TEST(inMemoryStorage, hasIdDown) {
    inMemoryStorage storage;
    identityService id_service;

    did id_1{"did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_2{"did:pvote:123IyM3iW5Ue6RJoNIBmA+X+Cvl0/R8qAqtX0XmhZkUEnA=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_3{"did:pvote:123p1IcVjNmLrf2nz5yxTsNH2vDCjGC82CxV3i6s0yZWYw=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};

    const didDocument &document_a = id_service.createDidDocument(id_1, id_2);
    const didDocument &document_b = id_service.createDidDocument(id_2, id_3);
    const didDocument &document_c = id_service.createDidDocument(id_3, id_3);

    storage.addDocument(id_1, document_a);
    storage.addDocument(id_2, document_b);
    storage.addDocument(id_3, document_c);

    bool result1 = storage.hasIdDown(id_1);
    ASSERT_FALSE(result1);
    bool result2 = storage.hasIdDown(id_2);
    ASSERT_TRUE(result2);
    bool result3 = storage.hasIdDown(id_3);
    ASSERT_TRUE(result3);
}

TEST(inMemoryStorage, getChain_with_circle) {
    inMemoryStorage storage;
    identityService id_service;

    did id_1{"did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_2{"did:pvote:123IyM3iW5Ue6RJoNIBmA+X+Cvl0/R8qAqtX0XmhZkUEnA=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};
    did id_3{"did:pvote:123p1IcVjNmLrf2nz5yxTsNH2vDCjGC82CxV3i6s0yZWYw=y9xrFMC8qIuI6zmJAAWl2B1Zm0/g0t2h/PXy6TnJrng=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="};

    const didDocument &document_a = id_service.createDidDocument(id_1, id_2);
    const didDocument &document_b = id_service.createDidDocument(id_2, id_3);
    const didDocument &document_c = id_service.createDidDocument(id_3, id_3);

    storage.addDocument(id_1, document_a);
    storage.addDocument(id_2, document_b);
    storage.addDocument(id_3, document_c);

    const std::map<did, did> &result_map = storage.getDIDChainUp(id_1);
    ASSERT_EQ(result_map.size(), 2);
}

TEST(inMemoryStorage, getAllDidVersions) {
    inMemoryStorage storage;
    identityService id_service;
    did id_base{"did:pvote:123"};

    did id_1 = id_base.withVersion(1);
    did id_2 = id_base.withVersion(2);
    did id_3 = id_base.withVersion(3);
    did id_other{"did:pvote:2222"};

    const didDocument &document_1 = id_service.createDidDocument(id_1, id_1);
    const didDocument &document_2 = id_service.createDidDocument(id_2, id_2);
    const didDocument &document_3 = id_service.createDidDocument(id_3, id_3);
    const didDocument &document_other = id_service.createDidDocument(id_other, id_other);

    storage.addDocument(id_1, document_1);
    storage.addDocument(id_2, document_2);
    storage.addDocument(id_3, document_3);
    storage.addDocument(id_other, document_other);

    const std::set<did> result_1 = storage.findAllDIDVersions(id_1);
    const std::set<did> result_2 = storage.findAllDIDVersions(id_2);
    const std::set<did> result_base = storage.findAllDIDVersions(id_base);
    const std::set<did> result_other = storage.findAllDIDVersions(id_other);
    const std::set<did> result_empty = storage.findAllDIDVersions({"pvote","ascy"});

    ASSERT_EQ(result_base.size(), 3);
    ASSERT_EQ(result_1.size(), 3);
    ASSERT_EQ(result_2.size(), 3);
    ASSERT_EQ(result_other.size(), 1);
    ASSERT_TRUE(result_empty.empty());
}

TEST(inMemoryStorage, getMaxVersion) {
    inMemoryStorage storage;
    identityService id_service;
    did id_base{"did:pvote:123"};

    did id_1 = id_base.withVersion(1);
    did id_2 = id_base.withVersion(2);

    const didDocument &document_1 = id_service.createDidDocument(id_1, id_1);
    const didDocument &document_2 = id_service.createDidDocument(id_2, id_2);

    storage.addDocument(id_1, document_1);
    storage.addDocument(id_2, document_2);

    const did result = storage.getLatest(id_base);

    ASSERT_EQ(result, id_2);
    try {
        did invalid_did{"pvote","bla"};
        ASSERT_THROW(storage.getLatest(invalid_did), std::invalid_argument);
    } catch(std::exception ex){};
}

//TEST(inMemoryStorage, addResource) {
//    ASSERT_TRUE(storage.fetchResource(did{"pvote","abcAddress123Timestamp123Nonce123"}) == "12.12.12.12");
//}
