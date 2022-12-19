//
// Created by wnabo on 09.05.2022.
//

#include <zmq.hpp>
#include "replyKeyThread.h"
#include "interruptException.h"

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

    _logger.log("Start thread");
    is_running = true;

    while (!is_interrupted && is_running) {
        bool isSocketBind = false;
        while (!isSocketBind) {
            try {
                socket.bind("tcp","*", 50061);
                isSocketBind = true;
            }
            catch (zmq::error_t &e) {
                _logger.log(("couldn't bind to socket: " + std::string(e.what())), "localhost", "distribute");
                zmq_sleep(1);
            }
        }

        zmq::message_t message;
        try {
            std::string result = socket.interruptableRecv(is_interrupted).payload;
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

                socket.send(data);
                prepared_election_keys->at(election_id).pop();

                std::string message2 = socket.interruptableRecv(is_interrupted).payload;

                _logger.log(message.to_string());
                socket.close();
            } catch (const std::invalid_argument& ia) {
                _logger.error("Invalid id");
                is_running = false;
            }
        } catch (interruptException ex) {
            _logger.log("interrupt exception");
        }
    }
}

replyKeyThread::replyKeyThread(zmqSocketAdapter &socket) : socket(socket) {

}

bool replyKeyThread::isRunning() {
    return is_running;
}

void replyKeyThread::setIsRunning(bool is_running) {
    this->is_running = is_running;
}

void replyKeyThread::interrupt() {
    this->is_interrupted = true;
}
