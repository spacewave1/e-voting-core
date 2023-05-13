//
// Created by wnabo on 25.04.2022.
//

#include <queue>
#include "gtest/gtest.h"

TEST(peer, checkSequence) {

    std::string input = "asdasdxcvxcvxcv####";
    std::string content_str;
    std::transform(input.begin(), input.end(), std::back_inserter(content_str),[](uint8_t d){
        return (char) d;
    });
    std::cout << content_str.size() << std::endl;
    content_str = content_str.substr(0, content_str.find("####"));
    std::cout << content_str.size() << std::endl;
    std::cout << content_str << std::endl;
}