#!/usr/bin/env bash

CWD=`pwd`

SCRIPT_PATH="$(realpath $( dirname ${BASH_SOURCE[0]} ))"
echo "SCRIPT_PATH: \"$SCRIPT_PATH\""

cd $SCRIPT_PATH

#cp -f web-server.js web-server.cjs
#cp -f websocket-server-rpc.js websocket-server-rpc.cjs

chmod +x dev
chmod +x server
chmod +x server.js
chmod +x web-server.js
#chmod +x web-server.cjs
chmod +x genkeycert.sh

./genkeycert.sh
npm install

run="node server.js ../ -S -i false -p 44380 --demo --serv --call"
exec $run

cd $CWD


