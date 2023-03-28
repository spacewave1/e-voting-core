//
// Created by wnabo on 25.04.2022.
//

#include <queue>
#include "gtest/gtest.h"
#include "../src/evoting/model/decentralNode.h"
#include "../src/evoting/service/hashService.h"
#include "../../did/src/didDocument.h"
#include <set>
#include <chrono>

TEST(decentralNode, create) {
    decentralNode node;
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    const auto p0 = std::chrono::time_point<std::chrono::system_clock>{};
    const auto p1 = std::chrono::system_clock::now();

    std::time_t epoch_time = std::chrono::system_clock::to_time_t(p0);
    std::cout << "epoch: " << std::ctime(&epoch_time);
    std::time_t today_time = std::chrono::system_clock::to_time_t(p1);
    std::cout << "today: " << std::ctime(&today_time);
    std::cout << "now: " << now << std::endl;

    decentralNode{ "10.0.0.1", "abcde", std::to_string(now)};
    hashService hash;
    std::cout << hash.hashMessage("10.0.0.1") << std::endl;
    std::cout << hash.hashMessage("abcde") << std::endl;
    std::cout << hash.hashMessage(std::to_string(now)) << std::endl;
}