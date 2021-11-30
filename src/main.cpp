#include <iostream>
#include <unistd.h>
#include <sstream>
#include <zmq.hpp>
#include <regex>
#include <set>
#include <nlohmann/json.hpp>
#include <pthread.h>
#include <fstream>
#include "peer.h"

std::map<std::string, size_t> current_poll;
std::string first_peer;

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

    std::for_each(current_poll.begin(), current_poll.end(), [](std::pair<std::string, size_t> optionVotes) {
        std::cout << optionVotes.first << ": " << optionVotes.second << std::endl;
    });
}

std::string importIdentity(const std::string filePath = "./"){
    std::ifstream importStream;
    std::string line;

    std::cout << "is importing peers file from " << filePath + "id.dat" << std::endl;

    // File Open in the Read Mode
    importStream.open(filePath + "id.dat");

    if(importStream.is_open())
    {
        if(getline(importStream, line)) {
            std::cout << line << std::endl;

            std::cout << "File contents: " << std::endl;
            std::cout << line << std::endl;
            return line;

        };
        // File Close
        importStream.close();
        std::cout << "Could not return line" << std::endl;
    }
    else
    {
        std::cout << "Unable to open the file!" << std::endl;
        return "id1";
    }
}

int main(int argc, char **argv) {
    std::cout << std::to_string(argc) << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
              << std::endl;

    zmq::context_t context(1);
    std::string input;
    peer local_peer;

    if(argc == 2){
        if(std::string(argv[1]).find("import_id") != -1){
            local_peer.setIdentity(importIdentity());
        }
    }

    straightLineSyncThread straight_line_sync_thread;
    straightLineDistributeThread straight_line_distribute_thread;


    auto straight_line_topology = straightLineTopology(straight_line_sync_thread, straight_line_distribute_thread);
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
        if (input.find("distribute_first_election") != -1) {
            local_peer.distribute_election(&context,straight_line_distribute_thread);
        }
        if (input.find("receive_poll") != -1) {
            receivePoll(&context);
        }
        if (input.find("place_vote") != -1) {
            local_peer.vote();
        }
        if (input.find("create_election") != -1) {
            local_peer.createElection();
        }
        if (input.find("fetch") != -1) {
            pthread_t pollFetchWorker;
            pthread_create(&pollFetchWorker, NULL, receivePoll, (void *) &context);
        }
        if (input.find("print_connections") != -1) {
            local_peer.printConnections();
        }
        if (input.find("print_elections") != -1) {
            local_peer.dumpElectionBox();
        }
        if (input.find("forward_sync") != -1) {
            local_peer.initSyncThread(&context, straight_line_sync_thread);
        }
        if (input.find("exit_sync") != -1) {
            pthread_exit(&syncWorker);
        }
        if (input.find("import_peer_connections") != -1) {
            std::cout << "importing peers connections" << std::endl;
            local_peer.importPeerConnections();
        }
        if (input.find("import_peers_list") != -1) {
            std::cout << "importing peers list" << std::endl;
            local_peer.importPeersList();
        }
        if (input.find("export_peer_connections") != -1) {
            std::cout << "is export peers connections" << std::endl;
            local_peer.exportPeerConnections();
        }
        if (input.find("export_peers_list") != -1) {
            std::cout << "is export peers list" << std::endl;
            local_peer.exportPeersList();
        }
        if (input.find("init_sync") != -1) {
            std::cout << "is connecting to " << input << std::endl;
            const std::string delimiter = " ";
            size_t positionOfWhitespace = input.find(delimiter);
            auto address = input.substr(positionOfWhitespace + delimiter.size(), input.size() - positionOfWhitespace);
            local_peer.initSyncThread(&context, straight_line_sync_thread, address);
        }
        if (input.find("update_net") != -1) {
            std::cout << "update net and joined sync thread" << std::endl;
            local_peer.setConnectionTable(straight_line_sync_thread.getConnectionTable());
            local_peer.setKnownPeerAddresses(straight_line_sync_thread.getPeers());
            straight_line_sync_thread.WaitForInternalThreadToExit();
        }
        if (input.find("cancel_sync") != -1) {
            straight_line_sync_thread.WaitForInternalThreadToExit();
        }
        if (input.find("quit") != -1) {
        } else {
            input.clear();
        }
    }
    return EXIT_SUCCESS;
}
