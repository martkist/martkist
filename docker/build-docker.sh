#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

DOCKER_IMAGE=${DOCKER_IMAGE:-martkist/martkistd}
DOCKER_TAG=${DOCKER_TAG:-`git rev-parse --short HEAD`}

BUILD_DIR=${BUILD_DIR:-.}

docker build --pull --build-arg TAG=$DOCKER_TAG -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
