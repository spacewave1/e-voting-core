//
// Created by wnabo on 03.01.2022.
//

#include "zmqSocketAdapter.h"
#include "../evoting/interruptException.h"
#include <iostream>

void zmqSocketAdapter::send(std::string payload) {
    socket.send(zmq::buffer(payload));
}

socketMessage zmqSocketAdapter::recv() {
    zmq::message_t message;
    socket.recv(message);
    _logger.log(message.to_string(), message.gets("Peer-Address"));
    return { message.to_string(), message.gets("Peer-Address") };
}

socketMessage zmqSocketAdapter::interruptableRecv(bool& is_interrupt) {
    zmq::message_t message;
    while(!is_interrupt) {
        socket.recv(message, zmq::recv_flags::dontwait);
        //log(message.gets("Peer-Address"), message.to_string());
        if(!message.empty()) {
            return { message.to_string(),message.gets("Peer-Address") };
        }
    }
    throw interruptException();
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

void zmqSocketAdapter::connect(std::string protocol, std::string address, size_t port) {
    if(protocol == "tcp"){
        if(port == 0) {
            throw std::invalid_argument("port number can't be 0 when using the tcp protocol");
        }
        socket.connect(protocol + "://" + address + ":" + std::to_string(port));
    } else if(protocol == "inproc") {
        socket.connect(protocol + "://" + address);
    }
}

void zmqSocketAdapter::bind(std::string protocol, std::string address, size_t port) {
    if(protocol == "tcp"){
        if(port == 0) {
            throw std::invalid_argument("port number can't be 0 when using the tcp protocol");
        }
        socket.bind(protocol + "://" + address + ":" + std::to_string(port));
        is_bound = true;
    } else if(protocol == "inproc") {
        socket.bind(protocol + "://" + address);
        is_bound = true;
    }
}

zmqSocketAdapter::zmqSocketAdapter(zmq::socket_t &socket) : socket(socket) {

}

void zmqSocketAdapter::unbind(std::string protocol, std::string address, size_t port) {
    if(protocol == "tcp"){
        if(port == 0) {
            throw std::invalid_argument("port number can't be 0 when using the tcp protocol");
        }
        socket.unbind(protocol + "://" + address + ":" + std::to_string(port));
        is_bound = true;
    } else if(protocol == "inproc") {
        socket.unbind(protocol + "://" + address);
        is_bound = true;
    }
}

void zmqSocketAdapter::disconnect(std::string protocol, std::string address, size_t port) {
    if(protocol == "tcp") {
        socket.disconnect(protocol + "://" + address + ":" + std::to_string(port));
    } else if(protocol == "inproc") {
        socket.disconnect(protocol + "://" + address);
    }
}

void zmqSocketAdapter::close() {
    socket.close();
}

void zmqSocketAdapter::printOptions() {
    _logger.displayData("ipv6: "  + std::to_string(socket.get(zmq::sockopt::ipv6)) );
    _logger.displayData("affinity: " +  std::to_string(socket.get(zmq::sockopt::affinity)) );
    _logger.displayData("backlog: " + std::to_string(socket.get(zmq::sockopt::backlog)) );
    _logger.displayData("events: " +  std::to_string(socket.get(zmq::sockopt::events)) );
    _logger.displayData("handshake_ivl: " +  std::to_string(socket.get(zmq::sockopt::handshake_ivl)) );
    _logger.displayData("last_endpoint: " + socket.get(zmq::sockopt::last_endpoint) );
    _logger.displayData("linger: " +  std::to_string(socket.get(zmq::sockopt::linger)) );
    _logger.displayData("maxmsgsize: " +  std::to_string(socket.get(zmq::sockopt::maxmsgsize)) );
    _logger.displayData("mechanism: " +  std::to_string(socket.get(zmq::sockopt::mechanism)) );

}

void zmqSocketAdapter::listen() {
    // TODO:
}

void zmqSocketAdapter::setupSocket(std::string localAddress, size_t port) {
    // TODO:
}
