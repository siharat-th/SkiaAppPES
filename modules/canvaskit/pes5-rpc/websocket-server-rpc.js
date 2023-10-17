'use strict';

var path = require('path');
var url = require('url');
var WebSocket = require('ws');

const wssServ = new WebSocket.Server({ noServer: true });
const wssCall = new WebSocket.Server({ noServer: true });

let calls = 0;
let callsstat = 0;
let getcalls = () => {
  const c = calls;
  callsstat = (callsstat + c) / 2;
  calls -= c;
  return Math.round(callsstat);
}

let cancels = 0;
let cancelsstat = 0;
let getcancels = () => {
  const c = cancels;
  cancelsstat = (cancelsstat + c) / 2;
  cancels -= c;
  return Math.round(cancelsstat);
}

setInterval(()=> console.log('\n     call:', wssCall.clients.size, '\n     serv:', wssServ.clients.size, '\n     idle:', idleServs.length, '\n   call/s:', getcalls(), '\n cancel/s:', getcancels(), '\n'), 1000);


// Simple 1 byte protocol
// 
// Server <==> Clients
//
// Using 4 in 32 of ASCII control characters
//
// clients ==> server ==> clients
// message start from clients req to server then server can forward or broadcast to other
// start session  
const STX = "\x02";
const nSTX = 0x02;
// end session
const ETX = "\x03";
const nETX = 0x03;
//
// server ==> clients 
// response message, server reply the message relate req from clients
// acknowledge
const ACK = "\x06";
const nACK = 0x06;
// cancel
const CAN = "\x18";
const nCAN = 0x18;

// error messages
const errTargetAbort = '{ "type": "err", "value": "target abort" }';
const errTargetBuzy  = '{ "type": "err", "value": "target buzy" }';
const errTargetExist = '{ "type": "err", "value": "target exist" }';

// FIFO servs 
const idleServs = [];

//function existElement(arr, elm) { 
//  return arr.findIndex(d => d === elm) > -1;
//}
//
//function notexistElement(arr, elm) { 
//  return arr.findIndex(d => d === elm) < 0;
//}

function appndElement(arr, elm) { 
  return arr.findIndex(d => d === elm) < 0 ? arr.push(elm) : arr.length;
}

function removeElement(arr, elm) { 
  let i = -1;
  return (i = arr.findIndex(d => d === elm), i < 0 ? null : arr.splice(i, 1));
}

function startSession(call) {
  if(call) {
    if(call.info && call.info.type == 'call') {
      let target = call.info.target;
      if(target) {
        if(call.readyState === WebSocket.OPEN) {
          //call.send(errTargetExist);
          call.send(CAN);
        }
        cancels++;
        //console.log('reply CAN (cause: session exist)');
        return;
      } 
      else {
        // shift serv from FIFO servs 
        let serv = idleServs.shift();
        //console.log('startSession:', serv.info.id);
        if(serv) {
            if(serv.readyState === WebSocket.OPEN && call.readyState === WebSocket.OPEN) {
              serv.info.target = call;
              serv.send(STX);
              call.info.target = serv;
              call.send(ACK);
              //console.log('ACK');
              return;
            } 
            else {
              // push serv back to FIFO servs
              appndElement(idleServs, serv);
            }
            
            if(call.readyState === WebSocket.OPEN) {
              //call.send(errTargetBuzy);
              call.send(CAN);
            }            
            cancels++;
            //console.log('reply CAN (cause: serv not alive)', serv.info.id);
            return;
        }
        
        if(call.readyState === WebSocket.OPEN) {
          //call.send(errTargetBuzy);
          call.send(CAN);
        }
        cancels++;
        //console.log('reply CAN (cause: serv buzy)');
      }
    }
  }
}

function endSession(source) {
  if(source) {
    if(source.info) {
      let dest = source.info.target;
      //console.log('endSession:', source.info.target);
      source.info.target = null;
      // has session?
      if(dest) {
        if(source.info.type == 'serv') {
          // push serv to FIFO servs 
          appndElement(idleServs, source);
        }      
        if(dest.info) {
          dest.info.target = null;
          if(dest.info.type == 'serv') {
            // push serv to FIFO servs 
            appndElement(idleServs, dest);
          }
          if (dest.readyState === WebSocket.OPEN) {
            dest.send(ETX);
          }            
        }
      }          
    }
  }    
}

wssCall.on('connection', function connection(ws, req) {
  //console.debug('ip:', req.socket.remoteAddress);
  const beginLog = '[wssCall]';
  const info = {
    id: Date.now(),
    type: 'call',
    target: null,
  };
  
  ws.send(`{"type": "id", "value": ${info.id}}`);
  ws.info = info;

  console.log(beginLog, 'onconnection:', info);

  ws.on('message', function message(data, isBinary) {
    // note: data type is always Node Buffer
    
    //console.log(beginLog, 'onmessage: [...]', 'len:', data.length, 'isBinary:', isBinary);
    //if(!isBinary) {
    //  data += '';
    //}
    
    // handle protocol
    //if(data.length == 1 && !isBinary) {
    if(data.length == 1) {
      //console.log(beginLog, 'data:', data[0], 'protocol:', data, 'len:', data.length, 'isBinary:', isBinary);
      //data += '';
      switch(data[0]) {
        default:
          break;
        case nSTX:
          //console.log(beginLog, 'startSession');
          startSession(ws);
          break;
        case nETX:
          //console.log(beginLog, 'endSession');
          calls++;
          endSession(ws);
          break;
      }
      return;
    }
    
    if(info.target) {
      //console.log(beginLog, 'onmessage: [...]', 'len:', data.length, 'isBinary:', isBinary);
      info.target.send(data, { binary: isBinary });
    } else {
      console.log(beginLog, 'no target, info:', info);
      //ws.close();
      
    }
  });
  
  ws.on('close', function () { 
    //console.log(beginLog, 'close:', info);
    endSession(ws);
  });
  
});  

wssServ.on('connection', function connection(ws, req) {
  //console.debug('ip:', req.socket.remoteAddress);
  const beginLog = '[wssServ]';
  const info = {
    id: Date.now(),
    type: 'serv',
    target: null,
  };

  ws.send(`{"type": "id", "value": ${info.id}}`);
  ws.info = info;
  
  // push serv to FIFO servs 
  appndElement(idleServs, ws);

  console.log(beginLog, 'onconnection:', info);

  ws.on('message', function message(data, isBinary) {
    // note: data type is always Node Buffer 
    
    //console.log(beginLog, 'onmessage: [...]', 'len:', data.length, 'isBinary:', isBinary);
    //if(!isBinary) {
    //  data += '';
    //}

    // handle protocol
    //if(data.length == 1 && !isBinary) {
    if(data.length == 1) {
      //console.log(beginLog, 'data:', data[0], 'protocol:', data, 'len:', data.length, 'isBinary:', isBinary);
      //data += '';
      switch(data[0]) {
        default:
          break;
        case nSTX:
          //serv type never request to start session.
          //startSession(ws);
          break;
        case nETX:
          //serv type can request to end session for some reason.
          cancels++;
          endSession(ws);
          break;
      }
      return;
    }
          
    if(info.target) {
      //console.log(beginLog, 'onmessage: [...]', 'len:', data.length, 'isBinary:', isBinary);
      info.target.send(data, { binary: isBinary });
    } else {
      console.log(beginLog, 'no target, info:', info);
      ws.close();
    }
  });
  
  ws.on('close', function () { 
    //console.log(beginLog, 'close:', info);  
    endSession(ws);
    // remove from FIFO servs 
    let removed = removeElement(idleServs, ws);
  });
  
});

function onupgrade(request, socket, head) {
  const { pathname } = url.parse(request.url);

  if (pathname === '/rpc/serv') {
    wssServ.handleUpgrade(request, socket, head, function done(ws) {
      wssServ.emit('connection', ws, request);
    });
  } 
  else if (pathname === '/rpc/call') {
    wssCall.handleUpgrade(request, socket, head, function done(ws) {
      wssCall.emit('connection', ws, request);
    });
  } 
  else {
    socket.destroy();
  }
}

module.exports = {
  wssServ,
  wssCall,
  onupgrade
};


