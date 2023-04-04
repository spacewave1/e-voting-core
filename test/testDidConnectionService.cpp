//
// Created by wld on 23.03.23.
//


#include <chrono>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "../src/identity/identityService.h"
#include "../src/identity/didConnectionService.h"

TEST(didConnectionService, testExportRegistry) {
    didConnectionService connection_service;
    identityService id_service;
    inMemoryStorage storage;
    did id_1{"pvote","abcd"};
    did id_2{"pvote","abce"};
    didDocument document = id_service.createDidDocument(id_1, id_2);
    storage.addDocument(id_1, document);
    connection_service.exportDidRegistry("/home/wld/workspace/e-voting/core/test/res/", storage,"didRegistry.json");
}

TEST(didConnectionService, testImportRegistry) {
    didConnectionService connection_service;
    inMemoryStorage storage;
    
    connection_service.importDidRegistry(storage, "/home/wld/workspace/e-voting/core/test/res/");

    const didDocument &result = storage.getDocument(did{"pvote","abcd"});
    std::cout << result << std::endl;
    did controller_id = did{"pvote", "abce"};
    ASSERT_TRUE(result.controller == controller_id);
}

TEST(didConnectionService, testExportResources) {
    didConnectionService service;
    inMemoryStorage storage;
    storage.addResource(did{"pvote","abcd"}, "10.0.0.1");
    storage.addResource(did{"pvote","abce"}, "10.0.0.2");
    service.exportDidResources("/home/wld/workspace/e-voting/core/test/res/",storage,"didResources.json");
}

TEST(didConnectionService, testImportResources) {
    didConnectionService connection_service;
    inMemoryStorage storage;

    connection_service.importDidResources(storage, "/home/wld/workspace/e-voting/core/test/res/");

    const std::string &result = storage.fetchResource(did{"pvote","abcd"});
    std::cout << result << std::endl;
    ASSERT_EQ(result, "10.0.0.1");
}