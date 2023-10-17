export class JS5App {
  
    preload() {
        if( typeof preload !== 'undefined' )
            preload();
    }

    setup() {
        let content = document.getElementById( "js5-content" );
        if( content ) {
            window.windowWidth = content.clientWidth;
            window.windowHeight = content.clientHeight;
        }
        else {
            window.windowWidth = document.body.offsetWidth;
            window.windowHeight = document.body.offsetHeight;
            }
        if( typeof setup !== 'undefined' )
            setup();
    }
    
    draw() {
        window.mouseX = js5app.mouseX;
        window.mouseY = js5app.mouseY;
        window.mouseIsPressed = js5app.mouseIsPressed;
        window.mouseButton = js5app.mouseButton;

        if( typeof draw !== 'undefined' )
            draw();
    }

    keyPressed() {
        if( typeof keyPressed !== 'undefined' ) {
            window.key = js5app.key;
            window.keyCode = js5app.keyCode;
    
            keyPressed();
        }
    }

    keyReleased() {
        if( typeof keyReleased !== 'undefined' ) {
            window.key = js5app.key;
            window.keyCode = js5app.keyCode;
    
            keyReleased();
        }
    }

    mousePressed() {        
        if( typeof mousePressed !== 'undefined' ) {
            window.mouseX = js5app.mouseX;
            window.mouseY = js5app.mouseY;
            window.pmouseX = js5app.pmouseX;
            window.pmouseY = js5app.pmouseY;
            window.mouseIsPressed = js5app.mouseIsPressed;
            window.mouseButton = js5app.mouseButton;
    
            mousePressed();
        }
    }  

    mouseReleased() {
        if( typeof mouseReleased !== 'undefined' ) {
            window.mouseX = js5app.mouseX;
            window.mouseY = js5app.mouseY;
            window.mouseIsPressed = js5app.mouseIsPressed;
            window.mouseButton = js5app.mouseButton;

            mouseReleased();
        }

    }   

    mouseMoved() {        
        if( typeof mouseMoved !== 'undefined' ) {
            window.mouseX = js5app.mouseX;
            window.mouseY = js5app.mouseY;
            window.pmouseX = js5app.pmouseX;
            window.pmouseY = js5app.pmouseY;
            window.mouseIsPressed = js5app.mouseIsPressed;
            window.mouseButton = js5app.mouseButton;
    
            mouseMoved();
        }
    } 

    mouseDragged() {        
        if( typeof mouseDragged !== 'undefined' ) {
            window.mouseX = js5app.mouseX;
            window.mouseY = js5app.mouseY;
            window.pmouseX = js5app.pmouseX;
            window.pmouseY = js5app.pmouseY;
            window.mouseIsPressed = js5app.mouseIsPressed;
            window.mouseButton = js5app.mouseButton;
    
            mouseDragged();
        }
    } 
}

window.onresize = function(event) {
    var that = this;
    this.setTimeout( function() {
        let content = document.getElementById( "js5-content" );
        let lowpanel = document.getElementById( "js5-lower-panel" );

        if( content ) {
            window.windowWidth = content.clientWidth;
            window.windowHeight = content.clientHeight;
            var style = window.getComputedStyle(lowpanel);
            if( style.display != 'none') {
                window.windowHeight = window.windowHeight - lowpanel.offsetHeight;
                content.height = window.windowHeight;
                content.style.height = window.windowHeight;
                console.log( window.windowHeight );
            }
        }
        else {
            window.windowWidth = document.body.offsetWidth;
            window.windowHeight = document.body.offsetHeight;
        }
        if( typeof windowResized !== 'undefined' ) {
            windowResized();
        }
        else {
            that.resizeCanvas(window.windowWidth, window.windowHeight);
        }    
    }, 20 );
}

window.MITER = 0;
window.ROUND = 1;
window.BEVEL = 2;

window.SQUARE = 2;
window.PROJECT = 0;

window.CPU = 0;
window.GPU = 1;

window.CORNER = 0;
window.CENTER = 1;
window.RADIUS = 2;

window.LEFT = 0;
window.RIGHT = 2;

window.RGB = 0;
window.HSV = 1;
window.HSB = 1;
window.HSL = 2;

window.POLYLINES = 0;
window.POINTS = 1;
window.LINES = 2;
window.TRIANGLES = 3;
window.TRIANGLE_STRIP = 4;
window.TRIANGLE_FAN = 5;
window.QUADS = 6;
window.QUAD_STRIP = 7;
window.CLOSE = 99;

window.TAU = Math.PI * 2;
window.PI = Math.PI;
window.HALF_PI = Math.PI / 2;
window.QUARTER_PI = Math.PI / 4;

window.sin = Math.sin;
window.cos = Math.cos;
window.tan = Math.tan;
window.atan = Math.atan;
window.atan2 = Math.atan2;
window.asin = Math.asin;
window.acos = Math.acos;

window.noCanvas = function() {
    
}
window.createCanvas = function( w, h, mode ) { 
    js5app.createCanvas( w, h, mode ); 
    window.width = js5app.width;
    window.height = js5app.height;

    if( window.width == window.windowWidth && window.height == window.windowHeight ) {
        js5app.js5.isFillScreen = true;
    }
}
window.resizeCanvas = function( w, h ) {
    js5app.resizeCanvas( w, h );
    window.width = js5app.width;
    window.height = js5app.height;
}
window.useCanvas = function( canvasID, mode ) { return js5app.useCanvas( canvasID, mode ); }
window.saveCanvas = function( name, ext ) { js5app.saveCanvas( name, ext ); }
window.noLoop = function() { js5app.noLoop(); }
window.pixelDensity = function( density ) {}

window.push = function() { js5app.save(); }
window.pop = function() { js5app.restore(); }

window.frameRate = function( fps ) { js5app.frameRate( fps ); }
window.noCursor = function() { js5app.noCursor(); }
window.grid = function() { js5app.grid(); }

window.colorMode = function( mode, max1, max2, max3, max4 ) { js5app.colorMode( mode, max1, max2, max3, max4 ); }
window.color = function( r, g, b, a ) { return js5app.color( r, g, b, a ); }
window.red = function(color) { return js5app.red(color); }
window.green = function(color) { return js5app.green(color); }
window.blue = function(color) { return js5app.blue(color); }
window.hue = function(color) { return js5app.hue(color); }
window.saturation = function(color) { return js5app.saturation(color); }
window.brightness = function(color) { return js5app.brightness(color); }
window.lerpColor = function( color1, color2, amount ) { return js5app.lerpColor( color1, color2, amount ); }
window.rectMode = function( mode ) { js5app.rectMode( mode ); }
window.imageMode = function( mode ) { js5app.imageMode( mode ); }

window.noStroke = function() { js5app.noStroke(); }
window.noFill = function() { js5app.noFill(); }
window.strokeCap = function( cap ) { js5app.strokeCap( cap ); }
window.strokeJoin = function( join ) { js5app.strokeJoin( join ); }
window.strokeWeight = function( weight ) { js5app.strokeWeight( weight ); }

window.clear = function() { js5app.clear(); }
window.background = function( r, g, b, a ) { js5app.background( r, g, b, a ); }
window.stroke = function( r, g, b, a ) { js5app.stroke( r, g, b, a ); }
window.fill = function( r, g, b, a ) { js5app.fill( r, g, b, a ); }

window.translate = function( x, y ) { js5app.translate( x, y ); }
window.scale = function( x, y ) { js5app.scale( x, y ); }
window.rotate = function( degree ) { js5app.rotate( degree ); }

window.scalePath = function( path, scaleX, scaleY ) { js5app.scalePath( path, scaleX, scaleY ); }
window.rotatePath = function( path, degree ) { js5app.rotatePath( path, degree ); }
window.translatePath = function( path, x, y ) { js5app.translatePath( path, x, y ); }

window.loadFont = function( url ) { return js5app.js5.loadFont( url ); }
window.loadImage = function( url ) { return js5app.js5.loadImage( url ); }

window.textFont = function( font ) { js5app.textFont( font ); }
window.textSize = function( size ) { js5app.textSize( size ); }

window.createPath = function() { return js5app.createPath(); }
window.textPath = function( text, x, y ) { return js5app.textPath( text, x, y ); }
window.polygonPath = function( lines ) { return js5app.polygonPath( lines ); }
window.createPathFromSVG = function( str ) { return js5app.createPathFromSVG( str ); }

window.createVector = function( x, y ) { return js5app.createVector( x, y ); }

window.drawPath = function( path ) { js5app.drawPath( path ); }
window.point = function( x, y ) { js5app.point( x, y ); }
window.line = function( x1, y1, x2, y2 ) { js5app.line( x1, y1, x2, y2 ); }
window.rect = function( x, y, w, h ) { js5app.rect( x, y, w, h ); }
window.roundrect = function( x, y, w, h, rx, ry ) { js5app.roundrect( x, y, w, h, rx, ry ); }
window.circle = function( x, y, r ) { js5app.circle( x, y, r ); }
window.ellipse = function( x, y, w, h ) { js5app.ellipse( x, y, w, h ); }
window.arc = function( x, y, w, h, start, stop, center ) { js5app.arc( x, y, w, h, start, stop, center ); }
window.triangle = function( x1, y1, x2, y2, x3, y3 ) { js5app.triangle( x1, y1, x2, y2, x3, y3 ); }
window.quad = function( x1, y1, x2, y2, x3, y3, x4, y4 ) { js5app.quad( x1, y1, x2, y2, x3, y3, x4, y4 ); }
window.cubic = function( x1, y1, x2, y2, x3, y3, x4, y4 ) { js5app.cubic( x1, y1, x2, y2, x3, y3, x4, y4 ); }
window.quadratic = function( x1, y1, x2, y2, x3, y3, x4, y4 ) { js5app.quadratic( x1, y1, x2, y2, x3, y3, x4, y4 ); }
window.conic = function( x1, y1, x2, y2, x3, y3, x4, y4 ) { js5app.conic( x1, y1, x2, y2, x3, y3, x4, y4 ); }
window.points = function( pts ) { js5app.points( pts ); }
window.polyline = function( pts ) { js5app.polyline( pts ); }
window.polygon = function( pts ) { js5app.polygon( pts ); }
window.polygonOutline = function( pts ) { js5app.polygonOutline( pts ); }
window.image = function( img, x, y, w, h ) { js5app.image( img, x, y, w, h ); }

window.beginShape = function( mode ) { js5app.beginShape( mode ); }
window.endShape = function( option ) { js5app.endShape( option ); }
window.vertex = function( x, y ) { js5app.vertex( x, y ); }
window.curveVertex = function( x, y ) { js5app.curveVertex( x, y ); }

window.radians = function( degree ) { return js5app.radians( degree ); }
window.random = function( min, max ) { return js5app.random( min, max ); }
window.randomSeed = function( seed ) { return js5app.randomSeed( seed ); }
window.int = function( num ) { return js5app.int( num ); }
window.floor = function( num ) { return Math.floor(num); }
window.ceil = function( num ) { return Math.ceil(num); }
window.sqrt = function( num ) { return js5app.sqrt(num); }
window.pow = function( num1, num2 ) { return js5app.pow( num1, num2 ); }
window.map = function( num, in_min, in_max, out_min, out_max ) { return js5app.map( num, in_min, in_max, out_min, out_max); }
window.clamp = function(num, min, max) { return js5app.clamp(num, min, max); }
window.dist = function( x1, y1, x2, y2 ) { return js5app.dist( x1, y1, x2, y2 ); }
window.min = function( num1, num2 ) { return js5app.min( num1, num2 ); }
window.max = function( num1, num2 ) { return js5app.max( num1, num2 ); }
window.gd = {
    timestamp: function() { js5app.timestamp() }
}
window.wrap = function(value, from, to){ return js5app.wrap(value, from, to); }

window.BACKSPACE = 8;
window.DELETE = 0;
window.ENTER = 13;
window.TAB = 9;
window.ESCAPE = 27;
window.SHIFT = 16;
window.CONTROL = 17;
window.OPTION = 91;
window.ALT = 18;
window.UP_ARROW = 38;
window.DOWN_ARROW = 40;
window.LEFT_ARROW = 37;
window.RIGHT_ARROW = 39;

window.createNavBar = function( parent, className, innerHTML ) {
    return js5app.createNavBar( parent, className, innerHTML );
}

window.createDiv = function( parent, className, innerHTML ) { 
    return js5app.createHTMLElement( parent, 'div', null, className, innerHTML );
}

window.createSection = function( parent, className, innerHTML ) { 
    return js5app.createHTMLElement( parent, 'section', null, className, innerHTML );
}

window.createSpan = function( parent, className, innerHTML ) { 
    return js5app.createHTMLElement( parent, 'SPAN', null, className, innerHTML );
}


window.createCheckbox = function( parent ) { 
    let checkbox = js5app.createHTMLElement( parent, 'INPUT', 'checkbox' ); 
    checkbox['checked'] = function( bChecked ) {
        checkbox.el.checked = bChecked;
    }
    return checkbox;
}
window.createSlider = function( parent, min, max, value ) { 
    let slider = js5app.createHTMLElement( parent, 'INPUT', 'range' ); 
    slider.el.min = min;
    slider.el.max = max;
    slider.el.value = value;
    return slider;
}

window.createVideo = function( parent, vdoURL ) { 
    return js5app.createVideo( parent, vdoURL); 
}

window.p5 = {
    Vector : {
        dist : function(newPos, lastPos) {
            let distX = newPos['x'] - lastPos['x'];
            let distY = newPos['y'] - lastPos['y'];
            let dist = Math.sqrt( distX * distX + distY * distY );
            return dist;
        },
        fromAngle : function( angle ) {
            let x = cos(angle);
            let y = sin(angle);
            let v = createVector( x, y );
            let len = this.dist( v, createVector(0,0) );
            v.x = v.x / len;
            v.y = v.y / len;
            return v;
        }
    }
    
}
