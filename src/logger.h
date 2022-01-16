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
    void log(std::string content, std::string address = "localhost") {
        std::cout << "[" << address << "]" << (address.length() < 11 ? "\t\t" : "\t") << content << std::endl;
    };
};
#endif //VOTE_P2P_LOGGER_H
