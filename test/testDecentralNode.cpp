//
// Created by wnabo on 25.04.2022.
//

#include <queue>
#include "gtest/gtest.h"
#include "../src/evoting/model/decentralNode.h"
#include <chrono>

TEST(decentralNode, create) {
    decentralNode node;
    auto now = std::chrono::system_clock::now();
    std::cout << now.time_since_epoch() << std::endl;
}

TEST(decentralNode, verify) {

}