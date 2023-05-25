# aqt needs python >= 3.7.5 so we start with ubuntu 20.04 
# and switch to trusty afterwards
FROM ubuntu:20.04 as qt
RUN mkdir /opt/qt
WORKDIR /opt/qt
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y python3 python3-pip curl p7zip-full
RUN pip3 install aqtinstall
RUN aqt install-qt linux desktop 5.14.2 gcc_64
RUN aqt install-qt mac desktop 5.14.2 clang_64
RUN curl -O -L https://github.com/qtwebkit/qtwebkit/releases/download/qtwebkit-5.212.0-alpha4/qtwebkit-MacOS-MacOS_10_13-Clang-MacOS-MacOS_10_13-X86_64.7z
ENV QTDIR=/opt/qt/5.14.2/clang_64
RUN 7z x qtwebkit-MacOS-MacOS_10_13-Clang-MacOS-MacOS_10_13-X86_64.7z -o${QTDIR}
ADD *.pc ${QTDIR}/lib/pkgconfig/

FROM ubuntu:trusty as build
ARG TAG
ARG MAKEOPTS
LABEL maintainer="Martkist Developers"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y \
    autoconf \
    automake \
    bsdmainutils \
    ca-certificates \
    cmake \
    curl \
    faketime \
    fonts-tuffy \
    g++ \
    git-core \
    imagemagick \
    libcap-dev \
    librsvg2-bin \
    libtiff-tools \
    libtool \
    libz-dev \
    libbz2-dev \
    pkg-config \
    python \
    python-dev \
    python-setuptools \
    xz-utils

WORKDIR /
ADD https://api.github.com/repos/martkist/martkist/git/refs/tags/${TAG} version.json
RUN git clone https://github.com/martkist/martkist.git martkist
WORKDIR /martkist
RUN git checkout ${TAG}

ENV BASEPREFIX=/martkist/depends
ENV HOST=x86_64-apple-darwin11

WORKDIR ${BASEPREFIX}/SDKs
RUN curl -O -L https://github.com/phracker/MacOSX-SDKs/releases/download/11.3/MacOSX10.11.sdk.tar.xz
RUN tar -xf MacOSX10.11.sdk.tar.xz

WORKDIR ${BASEPREFIX}
# trusty certs have expired, but we check input hashes so curl insecure is OK
RUN echo insecure >> $HOME/.curlrc
RUN make HOST=$HOST ${MAKEOPTS}
ENV HOSTPREFIX=${BASEPREFIX}/${HOST}

WORKDIR ${HOSTPREFIX}/bin
RUN curl -O -L https://github.com/martkist/freech-core/releases/download/v0.9.36/freech-core-v0.9.36-x86_64-apple-darwin16.tar.gz
RUN tar -xf freech-core-v0.9.36-x86_64-apple-darwin16.tar.gz

COPY --from=qt /opt/qt/5.14.2 /opt/qt/5.14.2
ENV QTNATIVE=/opt/qt/5.14.2/gcc_64
ENV QTDIR=/opt/qt/5.14.2/clang_64
RUN cp -r ${QTDIR}/lib/pkgconfig ${HOSTPREFIX}/lib

WORKDIR /martkist
RUN git submodule update --init

ENV OUTDIR=/outputs
RUN mkdir ${OUTDIR}
RUN releases/macos/builder.sh