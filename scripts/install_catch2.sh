#!/bin/bash
#
# install_catch2.sh
#
# Travis CI helper script.
# This installs Catch2 into the VM.
#
# Arguments:
# 1 -- name of the OS.

set -exv # exit immediatly after command fails,
         # add '+' in front of output,
         # print lines before executing them.

VERSION=2.13.4

TRAVIS_OS_NAME=$1
# Install Catch2 from sources
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    wget https://github.com/catchorg/Catch2/archive/v${VERSION}.tar.gz
    # CMake bin is installed in a strange place where
    # sudo can not find by default.
    sudo env "PATH=$PATH"
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    curl -L -O https://github.com/catchorg/Catch2/archive/v${VERSION}.tar.gz
fi

tar -xf v${VERSION}.tar.gz
cd Catch2-${VERSION}/
cmake -Bbuild -H. -DBUILD_TESTING=OFF

cmake --build build/ --target install
