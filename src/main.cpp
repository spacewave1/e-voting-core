#include <iostream>
#include <unistd.h>
#include <zmq.hpp>
#include <regex>
#include "network/zmqSocketAdapter.h"
#include <nlohmann/json.hpp>
#include <pthread.h>
#include <fstream>
#include "evoting/peer.h"
#include "evoting/replyKeyThread.h"
#include "network/connectionService.h"
#include "identity/graphService.h"

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

int main(int argc, char **argv) {
    logger _logger = logger::Instance();
    std::cout << std::to_string(argc) << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
              << std::endl;

    hillEncryptionService encryption_service;
    zmq::context_t context = zmq::context_t(1);
    std::string input;

    connectionService connection_service;
    graphService graph_service;

    peer local_peer;

    std::set<std::string> imported_peer_list;
    std::map<std::string, std::string> imported_peer_connections;

    local_peer.setIdentity(graph_service.importPeerIdentity());
    std::set<std::string>& addresses = connection_service.importPeersList(imported_peer_list);
    local_peer.setKnownPeerAddresses(addresses);

    std::map<std::string, std::string> &table = connection_service.importPeerConnections(imported_peer_connections);
    local_peer.setConnectionTable(table);

    zmq::socket_t connection_socket = zmq::socket_t(context, zmq::socket_type::req);
    zmq::socket_t onboard_socket = zmq::socket_t(context, zmq::socket_type::rep);

    zmq::socket_t inproc_socket = zmq::socket_t(context, zmq::socket_type::sub);
    inproc_socket.set(zmq::sockopt::subscribe, "");

    zmq::socket_t key_reply_socket = zmq::socket_t(context, zmq::socket_type::rep);
    zmq::socket_t receive_distribute_socket = zmq::socket_t(context, zmq::socket_type::rep);


    zmqSocketAdapter connection_socket_adapter(connection_socket);
    zmqSocketAdapter onboard_socket_adapter(connection_socket);
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
            local_peer.reply_keys (&context, reply_keys_thread);
        }
        if (input.size() == 0) {
            std::getline(std::cin, input);
        }
        if (input.find("connect_to") != -1) {
            connection_service.connect(input, &context, local_peer.getKnownPeerAddresses(), local_peer.getConnectionTable(), local_peer.getIdentity());
        }
        if (input.find("receive_connection") != -1) {
            connection_service.receive(&context, local_peer.getKnownPeerAddresses(), local_peer.getConnectionTable());
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
            //std::cout << election.getPollId() << ":" << election.getElectionOptionsJson() << std::endl;
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
            //local_peer.importPeerConnections();
            //local_peer.importPeersList();
            local_peer.updateDistributionThread(&straight_line_distribute_thread);
        }
        if (input.find("export_peer_connections") != -1) {
            std::cout << "is export peers connections" << std::endl;
            //local_peer.exportPeerConnections();
        }
        if (input.find("export_peers_list") != -1) {
            std::cout << "is export peers list" << std::endl;
            //local_peer.exportPeersList();
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