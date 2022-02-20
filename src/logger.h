//
// Created by wnabo on 15.01.2022.
//

#ifndef VOTE_P2P_LOGGER_H
#define VOTE_P2P_LOGGER_H

#include <iostream>

class logger {
public:
    static logger Instance() {
        static logger loggerInstance;
        return loggerInstance;
    }

    void promptUserInput(std::string content) {
        std::cout << std::endl << "### " << content << " ###" << std::endl << std::endl;
    };

    void displayData(std::string content) {
        std::cout << std::endl << "\t" << content << std::endl << std::endl;
    }

    void log(std::string content, std::string address = "localhost", std::string thread_name = "main") {
        std::cout << "[" << time(NULL) << "]" << "[" << address << "]" << "[" << thread_name << "]" <<
                  (address.length() + thread_name.length() < 32 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 28 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 24 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 20 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 16 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 12 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 8 ? "\t" : "") <<
                  (address.length() + thread_name.length() < 4 ? "\t" : "") <<
                  content << std::endl;
    };

    void warn(std::string content, std::string address = "localhost", std::string thread_name = "main") {
        std::cout << "\033[1;31m" << "[" << time(NULL) << "]" << "[" << address << "]" << "[" << thread_name << "]"
                  << (address.length() + thread_name.length() < 15 ? "\t\t" : "\t")
                  << (address.length() + thread_name.length() >= 15 && address.length() + thread_name.length() < 17
                      ? "\t" : "") << "\033[0m" << content << std::endl;
    }

    void error(std::string content, std::string address = "localhost", std::string thread_name = "main") {
        std::cout << "\033[4;33m" << "[" << time(NULL) << "]" << "[" << address << "]" << "[" << thread_name << "]"
                  << (address.length() + thread_name.length() < 15 ? "\t\t" : "\t")
                  << (address.length() + thread_name.length() >= 15 && address.length() + thread_name.length() < 17
                      ? "\t" : "") << content << "\033[0m" << std::endl;
    }
};

#endif //VOTE_P2P_LOGGER_H
