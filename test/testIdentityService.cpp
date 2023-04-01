//
// Created by wld on 23.03.23.
//


#include <chrono>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "../src/identity/identityService.h"

TEST(identiyService, test_create_did){
    identityService service;
    const auto p0 = std::chrono::time_point<std::chrono::system_clock>{};
    std::time_t epoch_time = std::chrono::system_clock::to_time_t(p0);
    std::cout << "epoch: " << std::ctime(&epoch_time);
    did id = service.createLocalDid(epoch_time,"10.0.0.1","abcdr");

    ASSERT_TRUE(id.methodSpecifierIdentier.find("123") != std::string::npos);
    ASSERT_TRUE(id.methodSpecifierIdentier.find("9QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=") != std::string::npos);
    ASSERT_TRUE(id.methodSpecifierIdentier.find("WJroURP96/lxBfruf6Yxtum7avA4j9uN/bFEB9ax9lg=") != std::string::npos);
    ASSERT_TRUE(id.methodSpecifierIdentier.find("zf+c6xVr9D9CZ7UwiRL6Rfi5QmEEKpJpbfckMAWPpe8=") != std::string::npos);
}

TEST(identiyService, test_create_did_doc){
    identityService service;
    // TODO: Problematisch ist das hier slashes enthalten sind
    did id = did("did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=WJroURP96/lxBfruf6Yxtum7avA4j9uN/bFEB9ax9lg=zf+c6xVr9D9CZ7UwiRL6Rfi5QmEEKpJpbfckMAWPpe8=");
    did controller_id = did{"did:pvote:123controller"};
    std::cout << controller_id << std::endl;
    const didDocument &document = service.createDidDocument(id, controller_id);
    std::cout << document << std::endl;
}

TEST(identityService, createDidDocument) {
    std::string verification_method_controller_id_hash = "abc";
    std::string id_hash = "yxc";
    did decentral_id {"pvote",id_hash};

    did verification_method_id {"pvote",id_hash + "#keys-1"};
    did verification_method_controller_id { "pvote", verification_method_controller_id_hash};

    didDocument document;
    document.id = decentral_id;
    const didVerificationMethod method = didVerificationMethod{
            verification_method_id,
            "Ed25519VerificationKey2020",
            verification_method_controller_id,
            "zH3C2AVvLMv6gmMNam3uVAjZpfkcJCwDwnZn6z3wXmqPV"
    };

    document.verification_method.insert(method);
    std::cout << document << std::endl;
}

TEST(identityService, deserializeIntoDocument) {
    identityService service;

    const didDocument &result_document = service.deserializeString(
            "{\"id\":\"did:pvote:abcAddress123Timestamp123Nonce123\",\"controllers\":[\"did:pvote:123controller\"],\"authentication\":{},\"verificationMethod\":[]}");

    ASSERT_EQ(result_document.id, did("did:pvote:abcAddress123Timestamp123Nonce123"));
}

TEST(identityService, deserializeIntoDocument2) {
    identityService service;

    std::string received_json = "{\"didDocuments\":{\"did:pvote:123BUZBLWG6V/FAy8ViyB7YP7G4quJrV7sawA1e3ceYrA8=AgzMxeuGQxo9dIiv5GVnVmK3KlLHpkQ16l0iiW3dZzw=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q=\":\"{\\\"id\\\":\\\"did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=AgzMxeuGQxo9dIiv5GVnVmK3KlLHpkQ16l0iiW3dZzw=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q=\\\",\\\"controllers\\\":[\\\"did:pvote:123BUZBLWG6V/FAy8ViyB7YP7G4quJrV7sawA1e3ceYrA8=AgzMxeuGQxo9dIiv5GVnVmK3KlLHpkQ16l0iiW3dZzw=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q=\\\"],\\\"authentication\\\":{},\\\"verificationMethod\\\":[]}\"},\"didResources\":{\"did:pvote:1239QRzRBIvDe6ZdLpnYeYca4ZJ4fOWjROmNeu/e+U6Og0=AgzMxeuGQxo9dIiv5GVnVmK3KlLHpkQ16l0iiW3dZzw=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q=\":\"\",\"did:pvote:123BUZBLWG6V/FAy8ViyB7YP7G4quJrV7sawA1e3ceYrA8=AgzMxeuGQxo9dIiv5GVnVmK3KlLHpkQ16l0iiW3dZzw=k4B0rGVAHowi6ga12b3MnERQsxGwgMkpruMuBHY/a/Q=\":\"10.0.0.5\"},\"receiverAddress\":\"\"}";
    nlohmann::json receive_data = nlohmann::json::parse(received_json);
    std::cout << receive_data["didDocuments"].dump() << std::endl;
}

TEST(identityService, testDeserialize) {

}