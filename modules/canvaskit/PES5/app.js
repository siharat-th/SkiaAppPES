var pesNodeCanvas;
var defaultFont;
var defaultPPEF;
var pesResourceProvider;

function preload() {
  defaultFont = loadFont("./assets/TTF/JS-Boaboon.ttf");
}

function setup() {
  // window.width = js5app.width;
  // window.height = js5app.height;

  const lw = 70;
  const rw = 250;
  let newWidth = window.windowWidth - lw - rw;
  const minCanvasWidth = 800; 
  if(newWidth>minCanvasWidth){
    resizeCanvas(newWidth, 720);
  }

  window.width = js5app.width;
  window.height = js5app.height;

  frameRate(60);
  background( 128 );
  rectMode( CENTER );

  textFont( defaultFont );
  textSize( 80 );

  // insertHtmlOptionElements();
  pesNodeCanvas = new PesNodeCanvas(CanvasKit.pesDocument.getInstance());
  pesResourceProvider = new PesResourceProvider();
  pesResourceProvider.loadResource('PPEF/Thai001.ppef', './assets/PPEF/Thai001.ppef', 
                      function(buffer){
                        var uints = new Uint8Array(buffer);
                        defaultPPEF = new SQLJS();
                        defaultPPEF.openPPEF(uints);
                        defaultPPEF.readPPEFConfig();
                      });
    insertHtmlOptionElements();
}

function draw() {
  clear();
  pesNodeCanvas.drawNodes();
}

function mousePressed() {
  pesNodeCanvas.mousePressed();
}

function mouseDragged() {
  pesNodeCanvas.mouseDragged();
}

function mouseMoved() {
  pesNodeCanvas.mouseMoved();
}

function mouseReleased() {
  pesNodeCanvas.mouseReleased();
}

function keyPressed(){
  pesNodeCanvas.keyPressed();
}

function keyReleased(){
  pesNodeCanvas.keyReleased();
}

function windowResized(){
  // pom: override for disable canvas auto resize
  // console.log(`w=${window.windowWidth} h=${window.windowHeight}`);
  // const lw = 70;
  // const rw = 250;
  // let newWidth = window.windowWidth - lw - rw;
  // const minCanvasWidth = 400; 
  // if(newWidth>minCanvasWidth){
  //   resizeCanvas(newWidth, 720);
  // }
}


