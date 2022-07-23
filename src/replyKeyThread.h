//
// Created by wnabo on 09.05.2022.
//

#ifndef VOTE_P2P_REPLYKEYTHREAD_H
#define VOTE_P2P_REPLYKEYTHREAD_H


#include "abstractThread.h"
#include "logger.h"
#include <map>
#include <set>
#include <queue>
#include <memory>

class replyKeyThread : public abstractThread {
private:
    logger _logger = logger::Instance();
    void InternalThreadEntry() override;
    void* arg;
    bool is_running = false;
    bool is_interrupted = false;
    bool is_verbose = true;
    std::shared_ptr<std::map<size_t, std::queue<std::string>>> prepared_election_keys;
public:
    void setParams(void *p_void, const std::map<size_t, std::queue<std::string>>& election_keys_queue);
    bool isRunning();
    replyKeyThread();
    void setIsRunning(bool is_running);
    void set_election_keys_queue(const std::map<size_t, std::queue<std::string>>& election_keys_queue);
};


#endif //VOTE_P2P_REPLYKEYTHREAD_H
