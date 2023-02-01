//
// Created by wld on 31.01.23.
//

#include <nlohmann/json.hpp>
#include "distributionService.h"

void distributionService::receiveInitial3PResponse(abstractSocket* socket, std::string address){
    socketMessage reply = socket->recv();

    _logger.log(reply.payload, reply.addressFrom, "distribute");
    socket->disconnect("tcp", address, 5049);
}

void distributionService::sendInitial3PRequest(abstractSocket* socket, size_t node_position, std::string address_up, std::string address_down) {
    int subscribe_port = 5051;
    int publish_port = 5050;

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;
    sendJson["origin_publish_port"] = publish_port;

    if (!address_up.empty()) {
        socket->connect("tcp", address_up, 5049);
        socket->send(sendJson.dump());
    }

    if (!address_down.empty()) {
        socket->connect("tcp", address_down, 5049);
        socket->send(sendJson.dump());
    }

    _logger.log("Has setup from active Distribution");
}

void distributionService::sendDirectionRequest(abstractSocket* forward_request_socket,
                                            nlohmann::json receiveJson,
                                            size_t node_position,
                                            std::string address_down,
                                            std::string address_up) {
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    int subscribe_port = std::stoi(receiveJson["origin_publish_port"].dump());

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;
    int publish_port;

    if (originPosition < node_position) {
        _logger.log("Set subscribe socket for upwards", "localhost", "distribute");

        if (!address_down.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request_socket->connect("tcp", address_down, 5049);
            forward_request_socket->send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    } else if (originPosition > node_position) {
        _logger.log("Set subscribe socket for downwards", "localhost", "distribute");

        if (!address_up.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request_socket->connect("tcp", address_up, 5049);
            forward_request_socket->send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    }
}

void distributionService::receiveDirectionResponse(abstractSocket* forward_request_socket, std::string address){
    socketMessage response = forward_request_socket->recv();
    _logger.log(response.payload, response.addressFrom, "distribute");
    forward_request_socket->disconnect("tcp", address, 5049);
    forward_request_socket->close();
}
