#include <iostream>
#include <sstream>
#include <zmq.hpp>

void checkAndReceive(const std::string input){
    if(input.find("receive") != -1){
        std::cout << "Is receiving" << std::endl;
        zmq::context_t ctx(2);
        zmq::socket_t sock(ctx, zmq::socket_type::rep);
        sock.bind("tcp://*:5555");

        while(true){
            std::cout << "wait" << std::endl;
            zmq::message_t request;

            sock.recv(request, zmq::recv_flags::none);
            std::cout << "Receive Hallo" << std::endl;

            zmq::message_t reply(5);
            memcpy(reply.data(), "World", 5);
            sock.send(zmq::str_buffer("reply"), zmq::send_flags::none);
        }
    }
}

void checkAndConnect(const std::string input){
    if (input.find("connect") != -1) {

        const std::string delimiter = " ";
        std::cout << "is connecting to " ;
        size_t positionOfWhitespace = input.find(delimiter);
        auto address = input.substr(positionOfWhitespace + delimiter.size(), input.size() - positionOfWhitespace);

        zmq::context_t ctx;
        zmq::socket_t sock(ctx, zmq::socket_type::push);

        std::string bla = "tcp://" + std::string(address) + ":5555";

        sock.bind(bla);
        sock.send(zmq::str_buffer("Hello World"), zmq::send_flags::dontwait);

        std::cout << address << std::endl;
        while(true){}
    }
}

int main(int argc, char** argv) {
    std::cout << std::to_string(argc) << std::endl;
    std::cout << argv[1] << std::endl;
    std::cout << "Usage:\t enter [connect] <address>, to connect to an address that runs this application as well"
         << std::endl;


    std::string input;


    while (input != "quit") {
        if(argc < 2){
            std::getline(std::cin, input);
        } else if(argc == 3){
            input = std::string(argv[1]) + " " + std::string(argv[2]);
        } else if(argc == 2){
            input = std::string(argv[1]);
        }
        checkAndConnect(input);
        checkAndReceive(input);
    }
}
