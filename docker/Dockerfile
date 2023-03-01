ARG TAG

FROM ubuntu:bionic
LABEL maintainer="Martkist Developers"
LABEL tag=${TAG}

RUN apt-get update
RUN apt-get install -y software-properties-common 

RUN add-apt-repository ppa:bitcoin/bitcoin
RUN apt-get update
RUN apt-get install -y git build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3
RUN apt-get install -y libboost-all-dev libdb4.8-dev libdb4.8++-dev libminiupnpc-dev libzmq3-dev libqrencode-dev
RUN apt-get install -y libqt5gui5 libqt5core5a libqt5webenginewidgets5 libqt5dbus5 qttools5-dev qttools5-dev-tools qtwebengine5-dev libprotobuf-dev protobuf-compiler

ADD https://api.github.com/repos/martkist/martkist/git/refs/tags/${TAG} version.json
RUN git clone https://github.com/martkist/martkist
WORKDIR /martkist
RUN ./autogen.sh
RUN ./configure
RUN make -j4 
RUN strip src/martkistd
RUN strip src/martkist-cli

FROM ubuntu:bionic

RUN apt-get update
RUN apt-get install -y software-properties-common 
RUN add-apt-repository ppa:bitcoin/bitcoin
RUN apt-get update
RUN apt-get install -y libzmq5 libboost-system1.65.1 libboost-filesystem1.65.1 libboost-program-options1.65.1 libboost-thread1.65.1 libboost-chrono1.65.1
RUN apt-get install -y libdb4.8 libdb4.8++
RUN apt-get install -y libminiupnpc10 
RUN apt-get install -y libevent-pthreads-2.1 libevent-2.1

COPY --from=0 martkist/src/martkistd .
COPY --from=0 martkist/src/martkist-cli .

ENTRYPOINT ["./martkistd", "-printtoconsole", "-datadir=/data"] 