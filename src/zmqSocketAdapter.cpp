//
// Created by wnabo on 03.01.2022.
//

#include "zmqSocketAdapter.h"
#include <iostream>

void zmqSocketAdapter::send(std::string payload) {
    socket.send(zmq::buffer(payload));
}

std::string zmqSocketAdapter::recv() {
    zmq::message_t message;
    socket.recv(message);
    log(message.gets("Peer-Address"), message.to_string());
    return message.to_string();
}

void zmqSocketAdapter::log(std::string address, std::string content) {
    std::cout << "[" << address << "]" << (address.length() < 11 ? "\t\t" : "\t") << content << std::endl;
}

void zmqSocketAdapter::setSocket(zmq::socket_t& new_socket) {
    this->socket = std::move(new_socket);
}

void zmqSocketAdapter::setSocketPort(size_t socketPort) {
    this->socket_port = socketPort;
}

void zmqSocketAdapter::setSocketAddress(std::string socketAddress) {
    this->socket_address = std::move(socketAddress);
}

void zmqSocketAdapter::connect(std::string address, size_t port) {
    socket.connect("tcp://" + address + ":" + std::to_string(port));
}

void zmqSocketAdapter::bind(std::string address, size_t port) {
    socket.bind("tcp://" + address + ":" + std::to_string(port));
    is_bound = true;
}

zmqSocketAdapter::zmqSocketAdapter(zmq::socket_t &socket) : socket(socket) {

}

void zmqSocketAdapter::unbind(std::string adddress, size_t port) {
    socket.unbind("tcp://" + adddress + ":" + std::to_string(port));
}

bool zmqSocketAdapter::isBound() {
    return is_bound;
}
