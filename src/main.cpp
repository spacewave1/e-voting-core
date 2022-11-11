#include <iostream>
#include <unistd.h>
#include <zmq.hpp>
#include <regex>
#include "zmqSocketAdapter.h"
#include <nlohmann/json.hpp>
#include <pthread.h>
#include <fstream>
#include "peer.h"
#include "replyKeyThread.h"

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

    nlohmann::json current_poll_json = nlohmann::json::parse(request.to_string());
    current_poll = current_poll_json.get<std::map<std::string, size_t>>();

    std::for_each(current_poll.begin(), current_poll.end(), [](std::pair<std::string, size_t> optionVotes) {
        std::cout << optionVotes.first << ": " << optionVotes.second << std::endl;
    });
}

std::string importIdentity(const std::string filePath = "./") {
    std::ifstream importStream;
    std::string line;

    std::cout << "is importing peers file from " << filePath + "id.dat" << std::endl;

    // File Open in the Read Mode
    importStream.open(filePath + "id.dat");

    if (importStream.is_open()) {
        if (getline(importStream, line)) {
            std::cout << "File contents: " << std::endl;
            std::cout << std::endl << line << std::endl << std::endl;
            return line;
        };
        // File Close
        importStream.close();
        std::cout << "Could not return line" << std::endl;
    } else {
        std::cout << "Unable to open the file!" << std::endl;
        return "id1";
    }
}

int main(int argc, char **argv) {
    logger _logger = _logger.Instance();
    std::cout << std::to_string(argc) << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
              << std::endl;

    basicEncryptionService encryption_service;
    zmq::context_t context = zmq::context_t(1);
    std::string input;

    peer local_peer;

    local_peer.importPeerIdentity();
    local_peer.importPeerConnections();
    local_peer.importPeersList();

    zmq::socket_t inproc_socket = zmq::socket_t(context, zmq::socket_type::sub);
    inproc_socket.set(zmq::sockopt::subscribe, "");

    zmq::socket_t key_reply_socket = zmq::socket_t(context, zmq::socket_type::rep);
    zmq::socket_t receive_distribute_socket = zmq::socket_t(context, zmq::socket_type::rep);

    zmqSocketAdapter inproc_socket_adapter(inproc_socket);
    zmqSocketAdapter key_socket_adapter(key_reply_socket);
    zmqSocketAdapter distribute_data_adapter(receive_distribute_socket);

    straightLineSyncThread straight_line_sync_thread;
    straightLineDistributeThread straight_line_distribute_thread(distribute_data_adapter);
    local_peer.updateDistributionThread(&straight_line_distribute_thread);

    inprocElectionboxThread inproc_electionbox_thread(local_peer.getElectionBox(), inproc_socket_adapter);
    local_peer.startInprocElectionSyncThread(&context, inproc_electionbox_thread);

    replyKeyThread reply_keys_thread(key_socket_adapter);

    // How will this be used ?
    //auto straight_line_topology = straightLineTopology(straight_line_sync_thread, straight_line_distribute_thread);
    //networkPlan plan(straight_line_topology);
    size_t election_id = 0;

    pthread_t syncWorker;
    if (argc < 2) {
        std::getline(std::cin, input);
    } else if (argc == 3) {
        input = std::string(argv[1]) + " " + std::string(argv[2]);
    } else if (argc == 2) {
        input = std::string(argv[1]);
    }

    while (input != "quit") {
        if (!straight_line_distribute_thread.isRunning()) {
            local_peer.passiveDistribution(&context, straight_line_distribute_thread);
        }
        if (!reply_keys_thread.isRunning()) {
            local_peer.reply_keys(&context, reply_keys_thread);
        }
        if (input.size() == 0) {
            std::getline(std::cin, input);
        }
        if (input.find("connect_to") != -1) {
            local_peer.connect(input, &context);
        }
        if (input.find("receive_connection") != -1) {
            local_peer.receive(&context);
        }
        if (input.find("update_election_box") != -1) {
            // TODO: Needs to check id and sequence number
            local_peer.pushBackElection(straight_line_distribute_thread.getElectionSnapshot());
            straight_line_distribute_thread.WaitForInternalThreadToExit();
            std::cout << "added election to distribution box" << std::endl;
        }
        if (input.find("distribute") != -1) {
            if(local_peer.getElectionBox().size() > 0) {
                straight_line_distribute_thread.interruptReceiveRequest();
                local_peer.distributeElection(&context, straight_line_distribute_thread);
            } else {
                _logger.log("No distributable elections found");
            }
        }
        if (input.find("cast_vote") != -1) {
            size_t selected_election = local_peer.selectElection();
            local_peer.vote(encryption_service, selected_election);
        }
        if (input.find("create_poll") != -1) {
            // nextElectionId = networkBuffer.getId()
            const election &election = local_peer.createElection(election_id);
            std::cout << election.getPollId() << ":" << election.getElectionOptionsJson() << std::endl;
            local_peer.pushBackElection(election);
            election_id += 1; // networkBuffer.incrementElectionId
        }
        if (input.find("fetch") != -1) {
            pthread_t pollFetchWorker;
            pthread_create(&pollFetchWorker, NULL, receivePoll, (void *) &context);
        }
        if (input.find("sockets") != -1) {
            _logger.log("print inproc socket options:");
            inproc_socket_adapter.printOptions();
        }
        if (input.find("context") != -1) {
            _logger.log("print context:");
            std::cout << "io_threads: " + std::to_string(context.get(zmq::ctxopt::io_threads)) << std::endl;
            std::cout << "thread_priority: " + std::to_string(context.get(zmq::ctxopt::thread_priority)) << std::endl;
            std::cout << "blocky: " + std::to_string(context.get(zmq::ctxopt::blocky)) << std::endl;
            std::cout << "ipv6: " + std::to_string(context.get(zmq::ctxopt::ipv6)) << std::endl;
            std::cout << "max_sockets: " + std::to_string(context.get(zmq::ctxopt::max_sockets)) << std::endl;
            std::cout << "socket_limit: " + std::to_string(context.get(zmq::ctxopt::socket_limit)) << std::endl;
            std::cout << "msg_t_size: " + std::to_string(context.get(zmq::ctxopt::msg_t_size)) << std::endl;
        }
        if (input.find("print_connections") != -1) {
            local_peer.printConnections();
        }
        if (input.find("print_polls") != -1) {
            local_peer.dumpElectionBox();
        }
        if (input.find("forward_sync") != -1) {
            local_peer.initSyncThread(&context, straight_line_sync_thread);
        }
        if (input.find("exit_sync") != -1) {
            pthread_exit(&syncWorker);
        }
        if (input.find("import") != -1) {
            local_peer.importPeerConnections();
            local_peer.importPeersList();
            local_peer.updateDistributionThread(&straight_line_distribute_thread);
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
        if (input.find("prepare_tallying") != -1) {
            straight_line_distribute_thread.interruptReceiveRequest();
            size_t selected_election = local_peer.selectElection();
            local_peer.eval_votes(reply_keys_thread, encryption_service, selected_election);
            local_peer.distributeElection(&context, straight_line_distribute_thread, selected_election);
        }
        // For testing purposes
        if(input.find("decrypt") != -1) {
            size_t selected_election = local_peer.selectElection();
            local_peer.decrypt_vote(selected_election, encryption_service);
        }
        if (input.find("tally_votes") != -1) {
            local_peer.request_keys((void*)&context);
            straight_line_distribute_thread.interruptReceiveRequest();
            local_peer.countInVotes(&context, straight_line_distribute_thread, encryption_service);
        }
        if (input.find("quit") != -1) {
            straight_line_distribute_thread.interruptReceiveRequest();
            inproc_electionbox_thread.interrupt();
            reply_keys_thread.interrupt();

            inproc_electionbox_thread.WaitForInternalThreadToExit();
            reply_keys_thread.WaitForInternalThreadToExit();
            straight_line_distribute_thread.WaitForInternalThreadToExit();
        } else {
            input.clear();
        }
    }
    return EXIT_SUCCESS;
}