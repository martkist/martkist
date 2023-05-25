# aqt needs python >= 3.7.5 so we start with ubuntu 20.04 
# and switch to trusty afterwards
FROM ubuntu:20.04 as qt
RUN mkdir /opt/qt
WORKDIR /opt/qt
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y python3 python3-pip curl p7zip-full
RUN pip3 install aqtinstall
RUN aqt install-qt linux desktop 5.14.2 gcc_64
RUN aqt install-qt windows desktop 5.14.2 win64_mingw73
RUN curl -O -L https://github.com/qtwebkit/qtwebkit/releases/download/qtwebkit-5.212.0-alpha4/qtwebkit-Windows-Windows_10-Mingw73-Windows-Windows_10-X86_64.7z
ENV QTDIR=/opt/qt/5.14.2/mingw73_64
RUN 7z x qtwebkit-Windows-Windows_10-Mingw73-Windows-Windows_10-X86_64.7z -o${QTDIR}
ADD *.pc ${QTDIR}/lib/pkgconfig/

FROM ubuntu:20.04 as build
LABEL maintainer="Martkist Developers"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Keep on separate lines to simplify package changes
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y \
    autoconf \
    automake \
    autotools-dev \
    build-essential \
    bsdmainutils \
    ca-certificates \
    curl \
    faketime \
    g++ \
    g++-mingw-w64 \
    g++-mingw-w64-x86-64 \
    git \
    libtool \
    mingw-w64 \
    nsis \
    pkg-config \
    python \
    rename \
    zip

WORKDIR /
ARG TAG
ADD https://api.github.com/repos/martkist/martkist/git/refs/tags/${TAG} version.json
RUN git clone https://github.com/martkist/martkist.git martkist
WORKDIR /martkist
RUN git checkout ${TAG}

ENV BASEPREFIX=/martkist/depends
ENV HOST=x86_64-w64-mingw32

WORKDIR ${BASEPREFIX}
# trusty certs have expired, but we check input hashes so curl insecure is OK
RUN echo insecure >> $HOME/.curlrc
ARG MAKEOPTS
RUN make HOST=$HOST ${MAKEOPTS}
ENV HOSTPREFIX=${BASEPREFIX}/${HOST}

WORKDIR ${HOSTPREFIX}/bin
RUN curl -O -L https://github.com/martkist/freech-core/releases/download/v0.9.36/freech-core-v0.9.36-x86_64-w64-mingw32.tar.gz
RUN tar -xf freech-core-v0.9.36-x86_64-w64-mingw32.tar.gz

COPY --from=qt /opt/qt/5.14.2 /opt/qt/5.14.2
ENV QTNATIVE=/opt/qt/5.14.2/gcc_64
ENV QTDIR=/opt/qt/5.14.2/mingw73_64
RUN cp -r ${QTDIR}/lib/pkgconfig ${HOSTPREFIX}/lib

WORKDIR /martkist
RUN git submodule update --init

ENV OUTDIR=/outputs
RUN mkdir ${OUTDIR}
RUN update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix
RUN releases/win/builder.sh