#include <iostream>
#include <unistd.h>
#include <sstream>
#include <zmq.hpp>
#include <regex>
#include <set>


std::set<std::string> knownIpAddresses;
std::map<std::string, std::string> connectionTable;

void printMetaData(zmq::message_t &msg);

void printConnections(){
    // Iterate through "receiver" nodes
    size_t index = 0;
    std::cout << "Host Ip Addresses" << std::endl;
    std::for_each(knownIpAddresses.begin(), knownIpAddresses.end(),[&index](const std::string ipAddress){
        std::cout << "[" << index << "] " <<  ipAddress << std::endl;
        index++;
    });

    // Iterate through connections
    index = 0;
    std::cout << "Connections" << std::endl;
    std::for_each(connectionTable.begin(), connectionTable.end(),[&index](const std::pair<std::string, std::string> connection){
        std::cout << "[" << index << "] " <<  connection.first << "->" << connection.second << std::endl;
        index++;
    });
}

void receive() {
    zmq::context_t ctx(2);
    zmq::socket_t sock(ctx, zmq::socket_type::rep);

    zmq::version();
    sock.bind("tcp://*:5555");
    zmq::message_t request;

    sock.recv(request, zmq::recv_flags::none);

    if(!request.empty()){
        if(!knownIpAddresses.contains(request.to_string())){
            if(std::regex_match(request.to_string(),std::regex("[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}"))){

                // Add to connection to topology
                knownIpAddresses.insert(request.to_string());
                connectionTable.insert(std::make_pair(std::string(request.gets("Peer-Address")) ,std::string(request.to_string())));

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

void connect(const std::string& input) {
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
    if(!reply.empty()){
        std::cout << reply.to_string() << std::endl;
        if(reply.to_string() == "accepted"){
            printMetaData(reply);
            std::cout << "added ip" << std::endl;

            knownIpAddresses.insert(std::string(address));
        }
    }
}

void printMetaData(zmq::message_t &msg){
    std::cout << std::endl;
    std::cout << "Socket-Type: " ;
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
        if(input.find("receive") != -1) {
            receive();
        }
        if (input.find("poll") != -1) {
            createPoll();
        }
        if(input.find("fetch") != -1){
            receivePoll();
        }
        if(input.find("print") != -1){
            printConnections();
        }
        input.clear();
    }
    return EXIT_SUCCESS;
}
