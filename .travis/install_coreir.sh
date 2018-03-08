#!/bin/bash

set -e

if [ "$TRAVIS_BRANCH" != "master" ]; then
    pip install git+git://github.com/leonardt/pycoreir.git@dev;
else
    pip install coreir;
fi
