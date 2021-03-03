#!/bin/bash
#
# install_catch2.sh
#
# Travis CI helper script.
# This installs Catch2 into the VM.
#

set -ex # exit immediatly after command fails, add '+' in front of output.

VERSION=2.13.4

# Install Catch2 from sources
echo test env var from travis: ${TRAVIS_OS_NAME}
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    wget https://github.com/catchorg/Catch2/archive/v${VERSION}.tar.gz
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    curl -L -O https://github.com/catchorg/Catch2/archive/v${VERSION}.tar.gz
fi

tar -xf v${VERSION}.tar.gz
cd Catch2-${VERSION}/
cmake -Bbuild -H. -DBUILD_TESTING=OFF

cmake --build build/ --target install
