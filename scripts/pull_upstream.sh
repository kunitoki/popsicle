#!/bin/bash

pushd ../
git subtree pull -P JUCE JUCE master --squash
popd
