//
// Created by wnabo on 09.05.2022.
//

#include <zmq.hpp>
#include "replyKeyThread.h"

void replyKeyThread::setParams(void *p_void, const std::map<size_t, std::queue<std::string>>& election_keys_queue) {
    this->arg = p_void;
    this->prepared_election_keys = std::make_shared<std::map<size_t, std::queue<std::string>>>(election_keys_queue);

    if(is_verbose) {
        _logger.log("keys: ");
        std::for_each(election_keys_queue.begin(), election_keys_queue.end(), [this](std::pair<size_t, std::queue<std::string>> idKeys){
            std::string id_string = std::to_string(idKeys.first);
            std::string keys_size_string = std::to_string(idKeys.second.size());
            _logger.log(id_string + ": " + keys_size_string);
        });
        _logger.log("keys after set: ");
        std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [this](std::pair<size_t, std::queue<std::string>> idKeys){
            std::string id_string = std::to_string(idKeys.first);
            std::string keys_size_string = std::to_string(idKeys.second.size());
            _logger.log(id_string + ": " + keys_size_string);
        });
    }
}

void replyKeyThread::set_election_keys_queue(const std::map<size_t, std::queue<std::string>>& election_keys_queue) {

    this->prepared_election_keys = std::make_shared<std::map<size_t, std::queue<std::string>>>(election_keys_queue);

    if(is_verbose) {
        _logger.log("keys: ");
        std::for_each(election_keys_queue.begin(), election_keys_queue.end(), [this](std::pair<size_t, std::queue<std::string>> idKeys){
            std::string id_string = std::to_string(idKeys.first);
            std::string keys_size_string = std::to_string(idKeys.second.size());
            _logger.log(id_string + ": " + keys_size_string);
        });
        _logger.log("keys after set: ");
        std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [this](std::pair<size_t, std::queue<std::string>> idKeys){
            std::string id_string = std::to_string(idKeys.first);
            std::string keys_size_string = std::to_string(idKeys.second.size());
            _logger.log(id_string + ": " + keys_size_string);
        });
    }
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
        zmq::recv_result_t result = key_reply_socket.recv(message);

        if(result.has_value()) {
            _logger.log("Received: " + message.to_string());
            try {
                size_t election_id = std::stoi(message.to_string());

                std::basic_string<char> &data = prepared_election_keys->at(election_id).front();
                if(is_verbose) {
                    std::for_each(prepared_election_keys->begin(), prepared_election_keys->end(), [this](std::pair<size_t, std::queue<std::string>> idKeys){
                        std::string idOfElection = std::to_string(idKeys.first);
                        std::string numberOfKeys = std::to_string(idKeys.second.size());
                        _logger.log("id: " + idOfElection);
                        _logger.log("numberOfElections: " + numberOfKeys);
                    });
                    _logger.log("Reply Key: " + data);
                }

                key_reply_socket.send(zmq::buffer(data));
                prepared_election_keys->at(election_id).pop();

                key_reply_socket.recv(message);

                _logger.log(message.to_string());
                key_reply_socket.close();
            } catch (const std::invalid_argument& ia) {
                _logger.error("Invalid id");
                is_running = false;
            }   
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
