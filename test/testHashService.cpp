#include "gtest/gtest.h"
#include "../src/evoting/service/hashService.h"

TEST(sha256test, on_test) {
    hashService service;
    std::string str = "test";
    std::string hashOutput = service.hashMessage(str.c_str());
    ASSERT_EQ(hashOutput, "n4bQgYhMfWWaL+qgxVrQFaO/TxsrC4Is0V1sFbDwCgg=");
}