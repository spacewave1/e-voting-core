#include <iostream>
#include <unistd.h>
#include <sstream>
#include <zmq.hpp>
#include <regex>
#include <set>
#include <nlohmann/json.hpp>
#include <pthread.h>
#include "peer.h"

std::map<std::string,size_t> current_poll;
std::string first_peer;

void createPoll() {
    std::cout << "Enter how many possible Options can be elected" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::size_t number_of_options = std::atoi(input.c_str());
    std::vector<std::string> options(number_of_options);
    std::cout << options.size() << std::endl;

    size_t index = 0;
    std::transform(options.begin(), options.end(), options.begin(), [&index](const std::string &option) -> std::string {
        index++;
        std::cout << "Enter Option #" << index << std::endl;
        std::string input;
        std::getline(std::cin, input);
        return input;
    });

    std::for_each(options.begin(), options.end(), [options, &index](const std::string &option) {
        current_poll[option] = 0;
    });

    nlohmann::json current_poll_json = nlohmann::ordered_json(current_poll);
    std::cout << current_poll_json.dump() << std::endl;
}

void distributePoll(void *arg) {
    nlohmann::json current_poll_json = nlohmann::ordered_json(current_poll);
    zmq::context_t *context = (zmq::context_t*)arg;
    zmq::message_t request;
    peer peer;

    zmq::socket_t towards_next_peer(*context, ZMQ_REQ);
    towards_next_peer.connect("tcp://" + first_peer + ":4456");
    towards_next_peer.send(zmq::message_t(current_poll_json.dump()));

    // Get list of connections
    // send out to all connections
}

void *receivePoll(void *arg) {
    // Get list of connections
    // send out to all connections
    zmq::context_t *context = (zmq::context_t *) arg;
    zmq::message_t request;

    zmq::socket_t from_peer(*context, ZMQ_REP);
    from_peer.bind("tcp://*:4456");
    from_peer.recv(request);

    std::cout << request.to_string() << std::endl;
    nlohmann::json current_poll_json = nlohmann::json::parse(request.to_string());
    current_poll = current_poll_json.get<std::map<std::string, size_t>>();

    std::for_each(current_poll.begin(), current_poll.end(), [](std::pair<std::string, size_t> optionVotes){
        std::cout << optionVotes.first << ": " << optionVotes.second << std::endl;
    });
}

int main(int argc, char **argv) {
    std::cout << std::to_string(argc) << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
              << std::endl;

    zmq::context_t context(1);
    std::string input;
    peer local_peer;

    straightLineSyncThread straight_line_sync_thread;


    auto straight_line_topology = straightLineTopology(straight_line_sync_thread);
    networkPlan plan(straight_line_topology);

    pthread_t syncWorker;
    if (argc < 2) {
        std::getline(std::cin, input);
    } else if (argc == 3) {
        input = std::string(argv[1]) + " " + std::string(argv[2]);
    } else if (argc == 2) {
        input = std::string(argv[1]);
    }

    while (input != "quit") {
        if (input.size() == 0) {
            std::getline(std::cin, input);
        }
        if (input.find("connect_to") != -1) {
            local_peer.connect(input, &context);
        }
        if (input.find("receive_connection") != -1) {
            local_peer.receive(&context);
        }
        if (input.find("distribute_poll") != -1){
            distributePoll(&context);
        }
        if (input.find("receive_poll") != -1) {
            receivePoll(&context);
        }
        if (input.find("poll") != -1) {
            createPoll();
        }
        if (input.find("fetch") != -1) {
            pthread_t pollFetchWorker;
            pthread_create(&pollFetchWorker, NULL, receivePoll, (void *) &context);
        }
        if (input.find("print") != -1) {
            local_peer.printConnections();
        }
        if (input.find("forward_sync") != -1) {
            local_peer.initSyncThread(&context, straight_line_sync_thread);
        }
        if (input.find("exit_sync") != -1) {
            pthread_exit(&syncWorker);
        }
        if (input.find("init_sync") != -1) {
            std::cout << "is connecting to " << input << std::endl;
            const std::string delimiter = " ";
            size_t positionOfWhitespace = input.find(delimiter);
            auto address = input.substr(positionOfWhitespace + delimiter.size(), input.size() - positionOfWhitespace);
            local_peer.initSyncThread(&context, straight_line_sync_thread, address);
        }
        if(input.find("cancel_sync") != -1) {
            straight_line_sync_thread.WaitForInternalThreadToExit();
        }
        input.clear();
    }
    return EXIT_SUCCESS;
}
