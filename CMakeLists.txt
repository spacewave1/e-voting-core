cmake_minimum_required(VERSION 3.17)
project(vote_p2p)

set(nlohmann_json_DIR ~/json-3.10.4/build)
set(CMAKE_CXX_STANDARD 20)
set(cppzmq_DIR ~/.libs/cppzmq-4.9.0/build)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(FetchContent)

find_package(Threads REQUIRED)
find_package(nlohmann_json 3.11.0 REQUIRED)
find_package(cppzmq REQUIRED)

include_directories(./ngraph)

add_subdirectory(googletest)

set(SOURCE_FILES
        src/main.cpp
        src/evoting/peer.cpp
        src/evoting/peer.h
        src/evoting/abstractThread.h
        src/evoting/straightLineSyncThread.cpp
        src/evoting/straightLineSyncThread.h
        src/evoting/model/election.h
        src/evoting/model/election.cpp
        src/evoting/straightLineDistributeThread.cpp
        src/evoting/straightLineDistributeThread.h
        src/network/zmqSocketAdapter.h
        src/network/zmqSocketAdapter.cpp
        src/evoting/model/electionBuilder.cpp
        src/evoting/model/electionBuilder.h
        src/evoting/electionPrototype.h
        src/evoting/inprocElectionboxThread.cpp
        src/evoting/inprocElectionboxThread.h
        src/evoting/logger.h
        src/evoting/replyKeyThread.cpp
        src/evoting/replyKeyThread.h
        src/evoting/service/hillEncryptionService.cpp
        src/evoting/service/hillEncryptionService.h
        src/evoting/hillLengthException.h
        src/evoting/interruptException.h
        src/network/abstractSocket.h
        src/network/connectionService.cpp
        src/network/connectionService.h
        src/network/socketMessage.h
        src/identity/identityService.cpp
        src/identity/identityService.h
        src/identity/identityGraph.cpp
        src/identity/identityGraph.h
        src/network/syncService.cpp
        src/network/syncService.h
        src/evoting/service/distributionService.cpp
        src/evoting/service/distributionService.h
        src/evoting/service/tallyService.cpp
        src/evoting/service/tallyService.h
        src/evoting/service/electionService.cpp
        src/evoting/service/electionService.h
        src/evoting/model/decentralElection.cpp
        src/evoting/model/decentralElection.h
        src/evoting/model/decentralNode.h
        src/evoting/service/hashService.cpp
        src/evoting/service/hashService.h
        src/datasource/inMemoryStorage.h
        src/datasource/inMemoryStorage.cpp
        src/crypt/sha.h
        src/crypt/sha224-256.c
        src/crypt/sha-private.h
        src/crypt/sha1.c
        src/crypt/usha.c
        src/crypt/hmac.c
        src/crypt/hkdf.c
        src/crypt/sha384-512.c
        )

set(CRYPT_SRC_FILES
        src/crypt/sha.h
        src/crypt/sha224-256.c
        src/crypt/sha-private.h
        src/crypt/sha1.c
        src/crypt/usha.c
        src/crypt/hmac.c
        src/crypt/hkdf.c
        src/crypt/sha384-512.c
        )

set(IDENTITY_SRC_FILES
        ngraph/ngraph.hpp
        src/identity/identityGraph.cpp
        src/identity/identityGraph.h
        src/identity/identityService.cpp
        src/identity/identityService.h
        ../did/src/didPVoteResolver.cpp
        ../did/src/didPVoteResolver.h
        src/identity/graphService.h
        src/identity/graphService.cpp
        src/identity/didConnectionService.cpp
        src/identity/didConnectionService.h
        src/identity/didSyncService.cpp
        src/identity/didSyncService.h
        )

enable_testing()

add_executable(vote_app ${SOURCE_FILES} ${IDENTITY_SRC_FILES})
add_executable(identity_test ${IDENTITY_SRC_FILES} src/identity/identityTest.cpp)
add_library(vote_core SHARED ${SOURCE_FILES} ${IDENTITY_SRC_FILES})
add_library(crypt STATIC ${CRYPT_SRC_FILES})

add_subdirectory(test)

#target_link_libraries(identity_test)
target_link_libraries(vote_app PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)
target_link_libraries(vote_core PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)