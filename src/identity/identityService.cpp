//
// Created by wld on 01.12.22.
//

#include <fstream>
#include <ctime>
#include <nlohmann/json.hpp>
#include "identityService.h"

did identityService::createLocalDid(time_t timestamp, std::string node_address, std::string nonce) {
    std::stringstream sstream;
    std::string timetamp_str = std::ctime(&timestamp);
    const std::string &timestamp_hash = hash_service.hashMessage(timetamp_str);
    const std::string node_address_hash = hash_service.hashMessage(node_address);
    std::string nonce_hash = hash_service.hashMessage(nonce);

    //TODO: generate random permuation

    return did{ "pvote", "123" + node_address_hash + timestamp_hash + nonce_hash};
}

didDocument identityService::createDidDocument(did identifier, did controller) {
    didDocument document;
    document.controllers = std::set<did>{ controller };
    document.id = std::move(identifier);
    return document;
}

didDocument identityService::deserializeString(std::string document_string) {
    nlohmann::json json = nlohmann::json::parse(document_string);
    didDocument doc;
    std::string id_string = json["id"].dump();
    doc.id = did(id_string);
    if(json["authentication"].is_object()){
        nlohmann::json json_authentication = json["authentication"];
        didAuthentication authentication;
        try {
            authentication.id = did(json["authentication"].at("id").dump());
            try { authentication.type = json["authentication"].at("type").dump(); }
                catch(std::exception ex) { std::cout << ex.what() << std::endl;}
            try { authentication.controller = did(json["authentication"].at("controller").dump());}
                catch(std::exception ex) { std::cout << ex.what() << std::endl;}
            try { authentication.publicKeyMultibase = json["authentication"].at("publicKeyMultibase").dump(); }
                catch(std::exception ex) { std::cout << ex.what() << std::endl;}
        } catch(std::exception ex) { std::cout << ex.what() << std::endl;}
    }

    //doc.controller = did(json["controller"].dump());

    nlohmann::json controllers = json["controllers"];

    std::cout << "controllers size: " << controllers.size() << std::endl;

    //std::for_each(controllers.begin(), controllers.end(), [](std::unordered_map<std::string, std::string> controller){
    //    std::for_each(controller.begin(), controller.end(), [](std::pair<std::string, std::string> pair){
    //        std::cout << pair.first << ": " << pair.second << std::endl;
    //    });
    //});

    return doc;
}
