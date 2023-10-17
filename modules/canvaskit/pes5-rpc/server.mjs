#!/usr/bin/env node


// [dependencies]
// - http-server: https://github.com/http-party/http-server#usage
// - ws         : https://github.com/websockets/ws
//
// 
// [install]
// $ npm install
// $ npm install http-server ws
// $ npm install --global http-server ws
//
// 
// [usage]
// $ ./server       [path] [options]
// $ ./server.js    [path] [options]
// $ node server.js [path] [options]
//
// 
// [more options]
// https://github.com/http-party/http-server#usage
//
// 
// [start webserver]
// options: 
// [path]: defaults to ./public if the folder exists, and ./ otherwise.
// note: for every system and in every console should have quote 
//       or double quote around path argument ie. 'path' 
// example:
// default path is cwd ./ or ./public(if exists)
// $ node server.js 
// relative path
// $ node server.js ../
// absolute path
// $ node server.js 'C:\Users\Phruetthiphong\Documents\WebServer' (windows fullpath)
// $ node server.js 'c:\users\phruetthiphong\documents\webserver' (same as above)
// windows-console: path can be unix-like ('/') but root path cannot
// $ node server.js 'C:/Users/Phruetthiphong/Documents/WebServer' (unix-like)
// $ node server.js 'c:/users/phruetthiphong/documents/webserver' (same as above)
// msys-console: path and root path can be unix-like
// $ node server.js '/C/Users/Phruetthiphong/Documents/WebServer' (same as above)
// $ node server.js '/c/users/phruetthiphong/documents/webserver' (same as above)
//
// 
// [start secure webserver]
// options: 
// -S: use tls/ssl
// -C: path-to/cert.pem (default: ./cert.pem)
// -K: path-to/key.pem  (default: ./key.pem)
// example:
// $ node server.js ../ -S
// $ node server.js 'C:\Users\Phruetthiphong\Documents\WebServer' -S
// cert, key from other path
// $ node server.js ../ -S -C path-to/cert.pem -K path-to/key.pem 
//
// 
// [start webserver and launch page]
// open browser window after starting the webserver
// options: 
// -o [path]: default path='/' (see http-server -o option)
// --demo [path]: default path='npm_build/1080_PES5RpcDemo'
// --serv [path]: default path='npm_build/1080_PES5Rpcs.html?type=serv'
// --call [path]: default path='npm_build/1080_PES5Rpcs.html?type=call'
// example:
// start webserver rpc and launch page http[s]://host:port/
// $ node server.js -o 
// start webserver rpc and launch page http[s]://host:port/npm_build/1080_PES5RpcDemo
// $ node server.js ../ -o npm_build/1080_PES5RpcDemo
// start secure webserver rpc and launch page http[s]://host:port/npm_build/1080_PES5RpcDemo
// $ node server.js ../ -o npm_build/1080_PES5RpcDemo -S
// start secure webserver rpc and launch pages
// - http[s]://host:port/npm_build/1080_PES5RpcDemo
// - http[s]://host:port/npm_build/1080_PES5Rpcs.html?type=call
// - http[s]://host:port/npm_build/1080_PES5Rpcs.html?type=serv
// $ node server.js ../ -S --demo --serv --call
//
//
// [shortcuts]
// bash-like console
// $ ./server    ../ -o npm_build/1080_PES5RpcDemo -S -i false -p 44380
// $ ./server.js ../ -o npm_build/1080_PES5RpcDemo -S -i false -p 44380
// $ ./dev --demo
// $ pes5-rpc/dev --serv --call
// 
// 
// [run from other pwd] (if u r install dependencies pkg globally)
// all console
// $ cd..
// $ node pes5-rpc/server.js -S --demo
// $ node pes5-rpc/server.js -S --demo
// bash-like console
// $ cd..
// $ pes5-rpc/server         -S --demo
// $ pes5-rpc/server.js      -S --demo
//

'use strict';

process.title = 'server.mjs';

// how to mixed modules type: module with type: commonjs
// 1.
// - add "type": "module", to package.json 
// - all CommonJS files inside our pkg will be rename ext-files from *.js to *.cjs 
//   then change require('./file.js') to require('./file.cjs') 
// - use createRequire to construct the require function 
// 2. easy than above
// - change this file to .mjs
// - use createRequire to construct the require function
// - finally: $ node server.mjs ... instead

// to solve the __filename and __dirname is not defined in ES module
//import path from 'path';
//import {fileURLToPath} from 'url';
//const __filename = fileURLToPath(import.meta.url);
////console.log('__filename', __filename)
//const __dirname = path.dirname(__filename);
////console.log('__dirname', __dirname);

// type: module
import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);

var argv = require('minimist')(process.argv.slice(2));

var version = argv.v || argv.version
if (version) {
  console.info('v' + require('./package.json').version);
  process.exit();
}

const opener = require('opener');
function fnLaunchPage(protocal, host, port, path) {
  console.log(protocal, host, port, path);
  const openHost = host === '0.0.0.0' ? '127.0.0.1' : host;
  let openUrl = `${protocol}${openHost}:${port}`;
  if (typeof path === 'string') {
    openUrl += path[0] === '/' ? path : '/' + path;
  }
  console.info('Open: ' + openUrl);
  opener(openUrl);
}

const {
  setTimeout,
} = require('node:timers/promises');

const protocol = argv.S || argv.tls ? 'https://' : 'http://';

const { WebServerPromise } = require('./web-server.js');

const fnStartWebSocketServerRpc = (server) => {
  const { onupgrade } = require('./websocket-server-rpc.js');  
  server.on('upgrade', onupgrade);
}


// sol-1: async-await valid inside async functions (type: commonjs, type: module)
//const ServerPromise = (async ()=>{
//  const httpServer = await WebServerPromise;
//  const server = httpServer.server;
//  fnStartWebSocketServerRpc( server );
//  ...
//  ...
//})();

// sol-2: promise.thenable (type: commonjs, type: module)
//WebServerPromise.then(httpServer => {
//  const server = httpServer.server;
//  fnStartWebSocketServerRpc( server );
//  ..
//  ..
//});

// sol-3: async-await valid on top level (type: module only)
//const httpServer = await WebServerPromise;
//const server = httpServer.server;
//fnStartWebSocketServerRpc( server );


// use sol-3 in esmodule

const httpServer = await WebServerPromise;
const server = httpServer.server;

// Event: 'listening'
// Emitted when the server has been bound after calling server.listen().
// see: https://nodejs.org/dist/latest/docs/api/net.html#event-listening
server.on('listening', async () => {
  
  // server.address()
  // Returns: <Object> | <string> | <null>
  // <Object>: { port: 12346, family: 'IPv4', address: '127.0.0.1' }
  // see: https://nodejs.org/dist/latest/docs/api/net.html#serveraddress
  
  const { port, address } = server.address() || {};
  //console.log(protocol, port, address);
  
  if(port) {
    // --demo [path]: default path='npm_build/1080_PES5RpcDemo.html'
    // --serv [path]: default path='npm_build/1080_PES5Rpcs.html?type=serv'
    // --call [path]: default path='npm_build/1080_PES5Rpcs.html?type=call'  
  
    const defaultPaths = {
      demo: 'npm_build/1080_PES5RpcDemo.html',
      serv: 'npm_build/1080_PES5Rpcs.html?type=serv',
      call: 'npm_build/1080_PES5Rpcs.html?type=call',
    };
    
    for(const k of Object.keys(defaultPaths)) {
      const v = argv[k];
      if(v) {
        const path = typeof v === 'string' ? v: defaultPaths[k];
        await setTimeout(1000, '');
        fnLaunchPage(protocol, address, port, path);
      }
    }
  }
  
});

fnStartWebSocketServerRpc( server );

export default server;
