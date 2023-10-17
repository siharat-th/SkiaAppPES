#!/bin/bash

# check if running on MacOS
if [[ "$(uname)" == "Darwin" ]]; then
    # play sound file
    afplay "$1"
else
    echo "This script is for MacOS only"
fi
