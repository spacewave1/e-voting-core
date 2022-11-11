FROM gcc:12.2.0

RUN set -ex;                                                                      \
    apt-get update;                                                               \
    apt-get install -y cmake libzmq3-dev;                                         \
    apt-get install unzip;																													\
    mkdir -p /usr/src;                                                            \
    cd /usr/src;                                                                  \
    curl -L https://github.com/zeromq/cppzmq/archive/v4.9.0.tar.gz | tar -zxf -;  \
    cd /usr/src/cppzmq-4.9.0;                                                     \
    cmake -D CPPZMQ_BUILD_TESTS:BOOL=OFF .; make; make install

RUN wget https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip; \
		unzip v3.11.2.zip; \
		rm v3.11.2.zip; \
		cd json-3.11.2; \
		mkdir build; \
		cd build; \
		cmake .. ; make ; make install ; cd .. 

COPY . /usr/src/example

RUN set -ex;              \
    cd /usr/src/example;  \
    cmake .; make

CMD ["/usr/src/example/vote_p2p"]
