#!/bin/bash

set -e

pip install -r requirements.txt
echo $TRAVIS_BRANCH;
if [ "$TRAVIS_BRANCH" != "master" ]; then
    echo $TRAVIS_BRANCH;
    git clone -b dev https://github.com/rdaly525/coreir.git;
    cd coreir;
    make -j
    sudo make -j install
    cd ..
    pip install git+git://github.com/leonardt/pycoreir.git@dev;
else
    wget https://github.com/rdaly525/coreir/releases/download/v0.0.9/coreir.tar.gz
    mkdir coreir_release 
    tar -xf coreir.tar.gz -C coreir_release --strip-components 1;
    cd coreir_release;
    sudo make install
    cd ..
    pip install coreir;
fi
