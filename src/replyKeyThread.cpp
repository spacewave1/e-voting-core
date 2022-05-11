//
// Created by wnabo on 09.05.2022.
//

#include <zmq.hpp>
#include "replyKeyThread.h"

void replyKeyThread::setParams(void *p_void, const std::map<size_t, std::queue<std::string>>& election_keys_queue) {
    this->arg = p_void;
    std::cout << "keys: ";
    std::for_each(election_keys_queue.begin(), election_keys_queue.end(), [](std::pair<size_t, std::queue<std::string>> idKeys){
        std::cout << idKeys.first;
        std::cout << "";
        std::cout << idKeys.second.size();
    });
    std::cout << std::endl << std::endl;
    std::cout << "keys after set: ";
    this->prepared_election_keys = std::make_shared<std::map<size_t, std::queue<std::string>>>(election_keys_queue);
    std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [](std::pair<size_t, std::queue<std::string>> idKeys){
        std::cout << idKeys.first;
        std::cout << idKeys.second.size();
    });
    std::cout << std::endl;
}

void replyKeyThread::set_election_keys_queue(const std::map<size_t, std::queue<std::string>>& election_keys_queue) {
    std::cout << "keys: ";
    std::for_each(election_keys_queue.begin(), election_keys_queue.end(), [](std::pair<size_t, std::queue<std::string>> idKeys){
        std::cout << idKeys.first;
        std::cout << "";
        std::cout << idKeys.second.size();
    });
    std::cout << std::endl << std::endl;
    std::cout << "keys after set: ";
    this->prepared_election_keys = std::make_shared<std::map<size_t, std::queue<std::string>>>(election_keys_queue);
    std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [](std::pair<size_t, std::queue<std::string>> idKeys){
        std::cout << idKeys.first;
        std::cout << idKeys.second.size();
    });
    std::cout << std::endl;
}

void replyKeyThread::InternalThreadEntry() {
    _logger.log("Create Reply Socket for keys");
    auto* context = (zmq::context_t*) arg;

    _logger.log("Start thread");
    is_interrupted = false;
    is_running = true;

    while (!is_interrupted && is_running) {
        zmq_sleep(1);
        zmq::socket_t key_reply_socket = zmq::socket_t(*context, zmq::socket_type::rep);
        bool isSocketBind = false;
        while (!isSocketBind) {
            try {
                key_reply_socket.bind("tcp://*:50061");
                isSocketBind = true;
            }
            catch (zmq::error_t &e) {
                _logger.log(("couldn't bind to socket: " + std::string(e.what())), "localhost", "distribute");
                zmq_sleep(1);
            }
        }

        zmq::message_t message;
        key_reply_socket.recv(message);

        _logger.log("Received: " + message.to_string());
        try {
            size_t election_id = std::stoi(message.to_string());
            std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [](std::pair<size_t, std::queue<std::string>> idKeys){
                std::cout << idKeys.first;
                std::cout << idKeys.second.size();
            });

            std::basic_string<char> &data = prepared_election_keys->at(election_id).front();
            _logger.log("Reply Key: " + data);

            key_reply_socket.send(zmq::buffer(data));
            prepared_election_keys->at(election_id).pop();

            key_reply_socket.recv(message);

            _logger.log(message.to_string());
            key_reply_socket.close();
        } catch (const std::invalid_argument& ia) {
            std::cout << "Invalid id" << std::endl;
            is_running = false;
        }
    }
}

replyKeyThread::replyKeyThread(){

}

bool replyKeyThread::isRunning() {
    return is_running;
}

void replyKeyThread::setIsRunning(bool is_running) {
    this->is_running = is_running;
}
