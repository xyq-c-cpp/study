#/bin/bash

mkdir -p $(pwd)/error
make 2>$(pwd)/error/error.log