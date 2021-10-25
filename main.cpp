#include <iostream>
#include <unistd.h>
#include <sstream>
#include <zmq.hpp>
#include <regex>
#include <set>
#include <nlohmann/json.hpp>
#include <pthread.h>

std::set<std::string> peers;
std::map<std::string, std::string> connectionTable;

void printMetaData(zmq::message_t &msg);

void init_sync(std::string initialReceiverAddress) {
    std::cout << "init sync to" << initialReceiverAddress << std::endl;

    zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, ZMQ_REQ);

    sock.connect("tcp://" + std::string(initialReceiverAddress) + ":5556");

    nlohmann::json sendJson = nlohmann::json();
    sendJson["receiverAddress"] = initialReceiverAddress;
    sendJson["nodes"] = nlohmann::ordered_json(peers);
    sendJson["connections"] = nlohmann::ordered_json(connectionTable);

    sock.send(zmq::message_t(nlohmann::to_string(sendJson)), zmq::send_flags::none);
    std::cout << "Has send json" << std::endl;
    zmq::message_t reply;
    sock.recv(reply, zmq::recv_flags::none);
    std::cout << "Received Confirmation" << std::endl;
    sock = zmq::socket_t(ctx, ZMQ_REP);

    zmq::message_t jsonData;
    sock.bind("tcp://*:5556");
    sock.recv(jsonData, zmq::recv_flags::none);

    nlohmann::json receivedData = nlohmann::json::parse(jsonData.to_string());
    std::map<std::string, std::string> receivedConnections = receivedData["connections"].get<std::map<std::string, std::string>>();
    std::set<std::string> receivedNodes = receivedData["nodes"].get<std::set<std::string>>();

    peers = receivedNodes;
    connectionTable = receivedConnections;

    sock.send(zmq::str_buffer("accepted"));
}

void *sync_routine(void *arg) {
    zmq::context_t *context = (zmq::context_t *) arg;
    zmq::message_t request;

    zmq::socket_t socket(*context, ZMQ_REP);
    socket.bind("tcp://*:5556");
    socket.recv(request, zmq::recv_flags::none);


    socket.send(zmq::message_t("accepted"), zmq::send_flags::none);
    socket.close();

    nlohmann::json receivedData = nlohmann::json::parse(request.to_string());
    std::string fromAddress = std::string(receivedData["receiverAddress"]);

    std::cout << "Received sync request: [" << request.to_string() << "]" << std::endl;

    if (std::regex_match(fromAddress, std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {
        // Merge data here
        std::map<std::string, std::string> receivedConnections = receivedData["connections"].get<std::map<std::string, std::string>>();
        std::set<std::string> receivedNodes = receivedData["nodes"].get<std::set<std::string>>();

        std::for_each(connectionTable.begin(), connectionTable.end(),
                      [&receivedConnections](const std::pair<std::string, std::string> pair) {
                          if(!receivedConnections.contains(pair.first)){
                              receivedConnections[pair.first] = pair.second;
                          }
                      });
        receivedNodes.insert(peers.begin(), peers.end());

        socket = zmq::socket_t(*context, ZMQ_REQ);

        if (connectionTable.contains(fromAddress)) {
            // Forward Data in root direction

            std::string nextReceiver = connectionTable[fromAddress];
            socket.connect("tcp://" + std::string(nextReceiver) + ":5556");

            nlohmann::json sendJson;
            sendJson["nodes"] = nlohmann::ordered_json(peers);
            sendJson["connections"] = nlohmann::ordered_json(connectionTable);

            std::cout << sendJson.dump() << std::endl;

            // Send the next receiver the updated nodes
            socket.send(zmq::message_t(sendJson.dump()), zmq::send_flags::none);

            // Receive the data from root direction
            zmq::message_t reply;
            socket.recv(reply);
            socket.send(zmq::str_buffer("accepted"),zmq::send_flags::none);

            std::cout << "Response: " << reply.to_string() << std::endl;

            nlohmann::json receivedData = nlohmann::json::parse(reply.to_string());
            std::map<std::string, std::string> receivedConnections = receivedData["connections"].get<std::map<std::string, std::string>>();
            std::set<std::string> receivedNodes = receivedData["nodes"].get<std::set<std::string>>();

            connectionTable = receivedConnections;
            peers = receivedNodes;

        } else {
            // Forward to branches
            // TODO: Work over this piece, it is data access in secondary thread
            connectionTable = receivedConnections;
            peers = receivedNodes;
        }

        std::map<std::string, std::set<std::string>> reversedConnectionTable;
        std::for_each(connectionTable.begin(), connectionTable.end(),[&reversedConnectionTable](std::pair<std::string, std::string> pair){
            reversedConnectionTable[pair.second].insert(pair.first);
        });

        nlohmann::json sendJson;
        sendJson["nodes"] = nlohmann::ordered_json(peers);
        sendJson["connections"] = nlohmann::ordered_json(connectionTable);

        std::cout << "send json: " << sendJson.dump() << std::endl;

        std::for_each(reversedConnectionTable[fromAddress].begin(), reversedConnectionTable[fromAddress].end(), [&socket, &sendJson](const std::string peerAddress){
            std::cout << "send data to " << peerAddress << std::endl;
            socket.connect("tcp://" + peerAddress +":5556");
            socket.send(zmq::message_t(sendJson.dump()),zmq::send_flags::none);
            zmq::message_t reply;
            socket.recv(reply,zmq::recv_flags::none);
            std::cout << reply.to_string() << std::endl;
            socket.unbind("tcp://" + peerAddress +":5556");
        });

        // If no more addresses in table the top is reached of the connection hierarchy, its the turning point
        // Merge and send downwards

    }

    return (NULL);
}

void printConnections() {
    // Iterate through "receiver" nodes
    size_t index = 0;
    std::cout << "Host Ip Addresses" << std::endl;
    std::for_each(peers.begin(), peers.end(), [&index](const std::string ipAddress) {
        std::cout << "[" << index << "] " << ipAddress << std::endl;
        index++;
    });

    // Iterate through connections
    index = 0;
    std::cout << "Connections" << std::endl;
    std::for_each(connectionTable.begin(), connectionTable.end(),
                  [&index](const std::pair<std::string, std::string> connection) {
                      std::cout << "[" << index << "] " << connection.first << "->" << connection.second << std::endl;
                      index++;
                  });

    nlohmann::json sendJson = nlohmann::json();
    sendJson["nodes"] = nlohmann::ordered_json(peers);
    sendJson["connections"] = nlohmann::ordered_json(connectionTable);
    std::cout << "As Json:" << std::endl << nlohmann::to_string(sendJson) << std::endl;

}

void receive() {
    zmq::context_t ctx(2);
    zmq::socket_t sock(ctx, zmq::socket_type::rep);

    zmq::version();
    sock.bind("tcp://*:5555");
    zmq::message_t request;

    sock.recv(request, zmq::recv_flags::none);

    if (!request.empty()) {
        if (!peers.contains(request.to_string())) {
            if (std::regex_match(request.to_string(),
                                 std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))) {

                // Add to connection to topology
                peers.insert(request.to_string());
                connectionTable.insert(
                        std::make_pair(std::string(request.gets("Peer-Address")), std::string(request.to_string())));

                printMetaData(request);
                std::cout << "added ip " << request.to_string() << " to list of network" << std::endl;
            } else {
                std::cout << "not an ip4" << std::endl;
            }

            sock.send(zmq::str_buffer("accepted"), zmq::send_flags::none);
        } else {
            std::cout << "already contains ip" << std::endl;
        }
    }

}

void connect(const std::string &input) {
    std::cout << "is connecting to " << input << std::endl;
    const std::string delimiter = " ";
    size_t positionOfWhitespace = input.find(delimiter);
    auto address = input.substr(positionOfWhitespace + delimiter.size(), input.size() - positionOfWhitespace);

    zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, zmq::socket_type::req);

    sock.connect("tcp://" + std::string(address) + ":5555");

    sock.send(zmq::message_t(address), zmq::send_flags::none);

    zmq::message_t reply;
    sock.recv(reply, zmq::recv_flags::none);
    if (!reply.empty()) {
        std::cout << reply.to_string() << std::endl;
        if (reply.to_string() == "accepted") {
            printMetaData(reply);
            std::cout << "added ip" << std::endl;

            peers.insert(std::string(address));
        }
    }
}

void printMetaData(zmq::message_t &msg) {
    std::cout << std::endl;
    std::cout << "Socket-Type: ";
    try {
        std::cout << msg.gets("Socket-Type") << std::endl;
    } catch (zmq::error_t er) {
        std::cout << "[]" << std::endl;
    }

    std::cout << "Peer-address: ";
    try {
        std::cout << msg.gets("Peer-Address") << std::endl;
    } catch (zmq::error_t er) {
        std::cout << "[]" << std::endl;
    }

    std::cout << "User-Id: ";
    try {
        std::cout << msg.gets("User-Id") << std::endl;
    } catch (zmq::error_t er) {
        std::cout << "[]" << std::endl;
    }

    std::cout << "Routing-Id: ";
    try {
        std::cout << msg.gets("Routing-Id") << std::endl;
    } catch (zmq::error_t er) {
        std::cout << "[]" << std::endl;
    }
    std::cout << std::endl;
}

void createPoll() {
    std::cout << "Enter how many possible Options can be elected" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::size_t numberOfOptions = std::atoi(input.c_str());
    std::vector<std::string> options(numberOfOptions);
    std::cout << options.size() << std::endl;

    std::transform(options.begin(), options.end(), options.begin(), [](const std::string &option) -> std::string {
        std::string input;
        std::getline(std::cin, input);
        return input;
    });

    int index = 0;
    std::cout << "{\n\tpoll: {" << std::endl;
    std::for_each(options.begin(), options.end(), [options, &index](const std::string &option) {
        index++;
        std::cout << "\t\t" << index << ": " << option;
        if (index < options.size()) {
            std::cout << ",";
        }
        std::cout << std::endl;
    });
    std::cout << "\t} \n}" << std::endl;
}

void checkAndSendPoll() {
    // Get list of connections
    // send out to all connections
}

void receivePoll() {
    // Get list of connections
    // send out to all connections
}

int main(int argc, char **argv) {
    std::cout << std::to_string(argc) << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
              << std::endl;

    zmq::context_t context(1);
    std::string input;
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
        if (input.find("connect") != -1) {
            connect(input);
        }
        if (input.find("receive") != -1) {
            receive();
        }
        if (input.find("poll") != -1) {
            createPoll();
        }
        if (input.find("fetch") != -1) {
            receivePoll();
        }
        if (input.find("print") != -1) {
            printConnections();
        }
        if (input.find("forward_sync") != -1) {
            pthread_t syncWorker;
            pthread_create(&syncWorker, NULL, sync_routine, (void *) &context);
        }
        if (input.find("init_sync") != -1) {
            std::cout << "is connecting to " << input << std::endl;
            const std::string delimiter = " ";
            size_t positionOfWhitespace = input.find(delimiter);
            auto address = input.substr(positionOfWhitespace + delimiter.size(), input.size() - positionOfWhitespace);
            init_sync(address);
        }
        input.clear();
    }
    return EXIT_SUCCESS;
}
