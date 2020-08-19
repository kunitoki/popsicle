#!/bin/bash

rm -Rf cxxbuild
mkdir -p cxxbuild
pushd cxxbuild
cmake -G "Ninja Multi-Config" ../ # -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
popd
