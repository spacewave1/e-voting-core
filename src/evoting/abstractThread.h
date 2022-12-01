//
// Created by wnabo on 09.11.2021.
//

#include <pthread.h>

#ifndef VOTE_P2P_ABSTRACTTHREAD_H
#define VOTE_P2P_ABSTRACTTHREAD_H


// see: https://stackoverflow.com/questions/1151582/pthread-function-from-a-class

class abstractThread {
public:
    abstractThread() {/* empty */}
    /** Will not return until the internal thread has exited. */
    void WaitForInternalThreadToExit() { (void) pthread_join(_thread, NULL); }
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread() { return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0); }

    virtual ~abstractThread() {/* empty */}

protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void InternalThreadEntry() = 0;

private:
    static void *InternalThreadEntryFunc(void *This) {
        ((abstractThread *) This)->InternalThreadEntry();
        return NULL;
    }

    pthread_t _thread;
};


#endif //VOTE_P2P_ABSTRACTTHREAD_H
