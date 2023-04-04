//
// Created by wld on 23.03.23.
//

#include <chrono>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "../src/identity/identityService.h"
#include "../src/identity/didConnectionService.h"
#include "../src/identity/didSyncService.h"
#include "MockSocket.h"

using ::testing::Return;

TEST(didSyncService, receiveSyncRequest) {
    didConnectionService connection_service;
    didSyncService sync_service;
    inMemoryStorage input_storage{};
    inMemoryStorage output_storage{};

    connection_service.importDidRegistry(input_storage, "/home/wld/workspace/e-voting/core/test/res/", "receiveDidRegistry.json");
    connection_service.importDidResources(input_storage, "/home/wld/workspace/e-voting/core/test/res/", "receiveDidResources.json");

    nlohmann::json send_json = nlohmann::json();
    std::string initial_receiver_address = input_storage.fetchResource(did{"pvote","123IyM3iW5Ue6RJoNIBmA+X+Cvl0/R8qAqtX0XmhZkUEnA=XgIBCx702Zw/pleBglU5cZwe9XohUx5JimZgs3CaSmU=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="});

    send_json["receiverAddress"] = initial_receiver_address;
    send_json["didDocuments"] = nlohmann::ordered_json(input_storage.getDidStorage());
    send_json["didResources"] = nlohmann::ordered_json(input_storage.getDidResources());

    mockSocket socket_adapter = mockSocket();
    EXPECT_CALL(socket_adapter, recv())
            .WillOnce(Return(socketMessage{send_json.dump(),"10.0.0.5"}));

    sync_service.receiveSyncRequest(socket_adapter, output_storage);
    const std::string &result = output_storage.fetchResource(did{"pvote","123IyM3iW5Ue6RJoNIBmA+X+Cvl0/R8qAqtX0XmhZkUEnA=XgIBCx702Zw/pleBglU5cZwe9XohUx5JimZgs3CaSmU=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q="});
    std::cout << result << std::endl;
    ASSERT_EQ(result, "10.0.0.9");
}