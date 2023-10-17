const PI = Math.PI;
const TWO_PI = Math.PI * 2;
const HALF_PI = Math.PI / 2;
const QUARTER_PI = Math.PI / 4; 

export class JS5 { 
   
    constructor( canvasID, surfaceMode=0 ) {
        this.tagID = canvasID
        this.surfaceMode = surfaceMode;
        this.app = null

        this.surface_mode = 0;
        this.isFullScreen = false;
    
        this.canvas_infos = [];
    
        this.font_resources = {};
        this.image_resources = {};
    
        this.joinType = [ null, null, null ];
        this.capType = [ null, null, null ];
    
        this.surface = null;
        this.canvas = null;
        this.html5_canvas = null;
        this.paint = null;
    
        this.MITER = 0;
        this.ROUND = 1;
        this.BEVEL = 2;
    
        this.SQUARE = 0;
        this.BUTT = 2;
    
        this.CPU = 0;
        this.GPU = 1;
    
        this.CORNER = 0;
        this.CENTER = 1;
        this.RADIUS = 2;
    
        this.LEFT = 0;
        this.RIGHT = 2;
    
        this.RGB = 0;
        this.HSB = 1;
        this.HSV = 1;
        this.HSL = 2;
    
        this.POLYLINES = 0;
        this.POINTS = 1;
        this.LINES = 2;
        this.TRIANGLES = 3;
        this.TRIANGLE_STRIP = 4;
        this.TRIANGLE_FAN = 5;
        this.QUADS = 6;
        this.QUAD_STRIP = 7;
        this.CURVE = 8;
        this.CLOSE = 99;
    
        this.TAU = Math.PI * 2;
        this.PI = Math.PI;
    
        this.backgroundColor = null;
        this.fillColor = null;
        this.strokeColor = null;
        this.strokeWidth = null;
        this.strokeCapType = null;
        this.strokeJoinType = null;
        this.isNoStroke = false;
        this.isNoFill = false;
    
        this.typeface = null;
        this.fontSize = null;
        this.font = null;
    
        this.translateX = 0;
        this.translateY = 0;
        this.rotateDegree = 0;
        this.scaleX = 1.0;
        this.scaleY = 1.0;
    
        this.currentImageMode = 0;
        this.currentRectMode = 0;
        this.currentColorMode = 0;
        this.currentColorMax = [255,255,255,255];
    
        this.snapshotNeed = false;
        this.snapshotCodec = 'image/png';
        this.snapshotFormat = CanvasKit.ImageFormat.PNG;
        this.snapshotName = 'image.png';
    
        this.shapeVertices = null;
        this.shapeVertex = null;
        this.shapeMode = 0;

        this.m_w = 123456789;
        this.m_z = 987654321;
        this.mask = 0xffffffff;

        // pom
        this.offScreenSurface = null;

        this.loadResources = async () => {
            for (let res in this.font_resources) {
                if (this.font_resources[res].endsWith('.ttf')) {
                    await this._loadFont(res, this.font_resources[res]);
                }
            }
            for (let res in this.image_resources) {
                if (this.image_resources[res]['name'].endsWith('.svg') ||
                    this.image_resources[res]['name'].endsWith('.png') ||
                    this.image_resources[res]['name'].endsWith('.jpg') ) {
                    await this._loadImage(res, this.image_resources[res]['name']);
                }
            }        
           
            this.preRun();
            this.app.setup();
            if( this.canvas_infos.length ) {
                this.run( true );
            }
            else {
                if( this.surface ) {
                    this.surface.flush();
                }    
                this.run();
            }
        };    
    
        this._loadFont = async (resName, fontURL) => {
//            const fontMgr = CanvasKit.FontMgr.RefDefault();
            const response = await fetch(fontURL);
            const buffer = await response.arrayBuffer();
//            this.font_resources[resName] = fontMgr.MakeTypefaceFromData(buffer);
//            this.font_resources[resName] = CanvasKit.FontMgr.FromData(buffer);
            this.font_resources[resName] = CanvasKit.Typeface.MakeFreeTypeFaceFromData(buffer);
        }
    
        this._loadImage = async (resName, imgURL) => {
            const response = await fetch(imgURL);
            const buffer = await response.arrayBuffer();
            
            if( imgURL.toLowerCase().endsWith( ".svg" ) ) {
    
               if(buffer.byteLength ){
                    var view = new Uint8Array(buffer);
                    var pbuff = new CanvasKit.pesBuffer();
                    pbuff.set(view);
                    var pesdata = CanvasKit.pesFactory.MakePesData();
                    const ok = pesdata.loadSVGFromBuffer(pbuff, true);
    
                    pbuff.delete();
    
                    this.image_resources[resName]['paths'] = [];
                    if(pesdata.paths.size()){
                        var bounds = null;
                        for(let i=0; i<pesdata.paths.size(); i++){
                            var path = pesdata.paths.get(i);
                            var skpath = CanvasKit.pesUtility.PesPathToSkPath(path);
                            var b = skpath.getBounds();
                            if( !bounds ) bounds = b;
                            if( b.fLeft < bounds.fLeft ) bounds.fLeft = b.fLeft;
                            if( b.fRight > bounds.fRight ) bounds.fRight = b.fRight;
                            if( b.fTop < bounds.fTop ) bounds.fTop = b.fTop;
                            if( b.fBottom > bounds.fBottom ) bounds.fBottom = b.fBottom;
    
                            var fillColor = null;
                            var strokeColor = null;
                            var strokeWidth = 0;
                            if( path.isFill() ) {
                                fillColor = path.getFillColor();
                            }
                            if( path.isStroke() ) {
                                strokeColor = path.getStrokeColor();
                                strokeWidth = path.getStrokeWidth();
                            }
                            this.image_resources[resName]['paths'].push( [skpath, strokeColor, fillColor, strokeWidth] );
                        }
                        this.image_resources[resName]['app'] = this;
                        this.image_resources[resName]['bounds'] = bounds;
                        this.image_resources[resName]['width'] = bounds.fRight-bounds.fLeft;
                        this.image_resources[resName]['height'] = bounds.fBottom-bounds.fTop;
                    }
                }
            }
            else if( imgURL.toLowerCase().endsWith( ".png" ) || imgURL.toLowerCase().endsWith( ".jpg" ) ) {
                if(buffer.byteLength ){
                    let img = CanvasKit.MakeImageFromEncoded(buffer);
                    this.image_resources[resName]['image'] = img;
                    this.image_resources[resName]['app'] = this;
                    //console.log( img );
                    let bounds = CanvasKit.LTRBRect(0, 0, img.width(), img.height() );
                    this.image_resources[resName]['bounds'] = bounds;
                    this.image_resources[resName]['width'] = bounds.fRight-bounds.fLeft;
                    this.image_resources[resName]['height'] = bounds.fBottom-bounds.fTop;
                    this.image_resources[resName]['resize'] = function(w, h) {
                        this.app.js5.resizeImage( this.app.image_resources[resName], w, h );
                    }
                    this.image_resources[resName]['loadPixels'] = function() {
                        this.app.js5.loadPixels( this.app.image_resources[resName] );
                    }
                    this.image_resources[resName]['get'] = function( x, y ) {
                        if( this.app.image_resources[resName]['pixels'] === undefined ) {
                            this.app.image_resources[resName].loadPixels();
                        }
                        let i = this.app.image_resources[resName]['width'] * 4 * y + (x * 4);
                        let r = this.app.image_resources[resName]['pixels'][ i ];
                        let g = this.app.image_resources[resName]['pixels'][ i+1 ];
                        let b = this.app.image_resources[resName]['pixels'][ i+2 ];
                        let a = this.app.image_resources[resName]['pixels'][ i+3 ];
                        return this.app.color( r, g, b, a );
                    }
                }
            }
    
    
            //var parser = new DOMParser();
            //var doc = parser.parseFromString(buffer, "image/svg+xml");
            //console.log( doc );
    
        }    
    }

    
   


    init( app ) {
        this.app = app;
        this.js5 = this;
        app.js5 = this;

        this.js5.backgroundColor = this.js5.createSkColor( 211 );

        this.joinType[this.MITER] = CanvasKit.StrokeJoin.Miter;
        this.joinType[this.BEVEL] = CanvasKit.StrokeJoin.Bevel;
        this.joinType[this.ROUND] = CanvasKit.StrokeJoin.Round;
    
        this.capType[this.BUTT] = CanvasKit.StrokeCap.Butt;
        this.capType[this.ROUND] = CanvasKit.StrokeCap.Round;
        this.capType[this.SQUARE] = CanvasKit.StrokeCap.Square;

        this.paint = new CanvasKit.Paint();
        this.paint.setStyle(CanvasKit.PaintStyle.Stroke);
        this.paint.setStrokeWidth(1);
        this.paint.setAntiAlias(true);

        this.strokeColor = CanvasKit.Color(0, 0, 0, 1.0)
        this.fillColor = CanvasKit.Color(0, 0, 0, 1.0)
        this.strokeWidth = 1;
        this.strokeJoinType = this.joinType[this.MITER];
        this.strokeCapType = this.capType[this.ROUND];

        this.currentFrameRate = 30;
        this.currentRectMode = this.CORNER;
        this.currentImageMode = this.CORNER;
        
        this.app.createCanvas = this.createCanvas;
        this.app.useCanvas = this.useCanvas;
        this.app.resizeCanvas = this.resizeCanvas;
        this.app.loadFont = this.loadFont;
        this.app.loadImage = this.loadImage;

        this.app.createVector = this.createVector;

        this.app.loopEnabled = true;

        this.app.frameRate = this.frameRate;
        this.app.clear = this.clear;
        this.app.background = this.background;
        this.app.cursor = this.cursor;
        this.app.noCursor = this.noCursor;
        this.app.grid = this.grid;
        this.app.fill = this.fill;
        this.app.stroke = this.stroke;
        this.app.strokeWeight = this.strokeWeight;
        this.app.strokeJoin = this.strokeJoin;
        this.app.strokeCap = this.strokeCap;
        this.app.noFill = this.noFill;
        this.app.noStroke = this.noStroke;

        this.app.rectMode = this.rectMode;
        this.app.imageMode = this.imageMode;

        this.app.mouseX = 0;
        this.app.mouseY = 0;
        this.app.pmouseX = 0;
        this.app.pmouseY = 0;
        this.app.mouseIsPressed = false;
        this.app.mouseButton = -1;
        this.app.key = '';
        this.app.keyCode = 0;

        this.app.CPU = this.CPU;
        this.app.GPU = this.GPU;

        this.app.MITER = this.MITER;
        this.app.BEVEL = this.BEVEL;
        this.app.ROUND = this.ROUND;
        this.app.SQUARE = this.SQUARE;
        this.app.BUTT = this.BUTT;

        this.app.CORNER = this.CORNER;
        this.app.CENTER = this.CENTER;
        this.app.RADIUS = this.RADIUS;

        this.app.RGB = this.RGB;
        this.app.HSV = this.HSV;
        this.app.HSB = this.HSB;
        this.app.HSL = this.HSL;

        this.app.PNG = 'png';
        this.app.JPG = 'jpg';
        this.app.JPEG = 'jpg';

        this.app.TAU = this.TAU;
        this.app.PI = this.PI;

        this.app.MULTILINES = this.MULTILINES;
        this.app.POINTS = this.POINTS;
        this.app.LINES = this.LINES;
        this.app.TRIANGLES = this.TRIANGLES;
        this.app.TRIANGLE_STRIP = this.TRIANGLE_STRIP;
        this.app.TRIANGLE_FAN = this.TRIANGLE_FAN;
        this.app.QUADS = this.QUADS;
        this.app.QUAD_STRIP = this.QUAD_STRIP;
        this.app.CLOSE = this.CLOSE;

        this.app.colorMode = this.colorMode;
        this.app.color = this.color;
        this.app.lerpColor = this.lerpColor;
        this.app.red = this.red;
        this.app.green = this.green;
        this.app.blue = this.blue;
        this.app.hue = this.hue;
        this.app.saturation = this.saturation;
        this.app.brightness = this.brightness;

        this.app.scale = this.scale;
        this.app.rotate = this.rotate;
        this.app.translate = this.translate;
        this.app.save = this.save;
        this.app.restore = this.restore;

        this.app.translatePath = this.translatePath;
        this.app.scalePath = this.scalePath;
        this.app.rotatePath = this.rotatePath;

        this.app.textFont = this.textFont;
        this.app.textSize = this.textSize;
        this.app.text = this.text;
        this.app.textPath = this.textPath;

        this.app.point = this.point;
        this.app.line = this.line;
        this.app.rect = this.rect;
        this.app.roundrect = this.roundrect;
        this.app.circle = this.circle;
        this.app.ellipse = this.ellipse;
        this.app.arc = this.arc;

        this.app.createPath = this.createPath;
        this.app.createPathFromSVG = this.createPathFromSVG;
        this.app.drawPath = this.drawPath;
        this.app.fillPath = this.fillPath;
        this.app.strokePath = this.strokePath;
        this.app.polyline = this.polyline;
        this.app.polygon = this.polygon;
        this.app.polygonOutline = this.polygonOutline;
        this.app.triangle = this.triangle;
        this.app.quad = this.quad;
        this.app.cubic = this.cubic;
        this.app.quadratic = this.quadratic;
        this.app.conic = this.conic;
        this.app.makePesImageSnapshot = this.makePesImageSnapshot;
        this.app.makePesScalableImageSnapshot = this.makePesScalableImageSnapshot;
        this.app.drawPesStitches = this.drawPesStitches;
        this.app.drawPesScalable = this.drawPesScalable;
        this.app.image = this.image;

        this.app.saveCanvas = this.saveCanvas;

        this.app.beginShape = this.beginShape;
        this.app.endShape = this.endShape;
        this.app.vertex = this.vertex;
        this.app.curveVertex = this.curveVertex;

        this.app.radians = this.radians;
        this.app.random = this.random;
        this.app.randomSeed = this.randomSeed;
        this.app.int = this.int;
        this.app.map = this.map;
        this.app.sqrt = this.sqrt;
        this.app.pow = this.pow;
        this.app.clamp = this.clamp;
        this.app.dist = this.dist;
        this.app.min = this.min;
        this.app.max = this.max;
        this.app.wrap = this.wrap;

        this.app.createNavBar = this.createNavBar;
        this.app.createHTMLElement = this.createHTMLElement;
        this.app.createVideo = this.createVideo;

        this.app.timestamp = () => {
            return Date.now();
        }

        this.app.noLoop = () => {
            console.log( "No loop" );
            this.app.loopEnabled = false;
        }

        // Start app
        this.app.preload();
        this.loadResources();


    }

    createCanvas( width, height, mode ) {
        if( mode === undefined )
            mode = 1;
        var canvas = document.createElement("canvas");

        canvas.id = "myCanvas";

        let content = document.getElementById( "js5-content" );
        if( content ) {
            let lowpanel = document.getElementById( "js5-lower-panel" );
            var style = window.getComputedStyle(lowpanel);
            if( style.display != 'none') {
                height = height - lowpanel.offsetHeight;
            }

            canvas.width = width;
            canvas.height = height;
            canvas.style.width = width + "px";
            canvas.style.height = height + "px";
            canvas.style.position = "absolute";
            content.appendChild( canvas );
        }
        else {
            canvas.style.border = "1px solid";
            canvas.style.width = "100%";
            canvas.style.height = "100%";
            canvas.width = width;
            canvas.height = height;
            var body = document.getElementsByTagName("body")[0];
            body.appendChild(canvas);
        }

        this.js5.initCanvas( canvas, mode );
    }

    useCanvas( canvasID, mode ) {
        let html5_canvas = document.getElementById( canvasID );
        if( html5_canvas ) {
            if( mode === this.undefined )
                mode = CPU;
    
            var surface = null;
            if( mode == 1 ) {
                surface = CanvasKit.MakeCanvasSurface(canvasID);
                console.log( "GPU mode" );
            }
            else {
                surface = CanvasKit.MakeSWCanvasSurface(canvasID);
                console.log( "CPU mode" );
            }
            let canvas = surface.getCanvas();
    
            let width = surface.width();
            let height = surface.height();
    
            var canvas_info = {
                'html5_canvas': html5_canvas,
                'surface': surface,
                'canvas': canvas,
                'width': width,
                'height': height
            }
            this.js5.canvas_infos.push( canvas_info );
    
            return canvas_info;
        }
    }

    resizeCanvas( w, h ) {
        if( this.js5.html5_canvas === undefined )
            return;
        this.js5.html5_canvas.width = w;
        this.js5.html5_canvas.height = h;
        this.js5.html5_canvas.style.width = w + 'px';
        this.js5.html5_canvas.style.height = h + 'px';

        this.js5.surface.delete();
        if( this.js5.surface_mode == 1 ) {
            this.js5.surface = CanvasKit.MakeCanvasSurface(this.js5.html5_canvas.id);
            console.log( "GPU mode" );
        }
        else {
            this.js5.surface = CanvasKit.MakeSWCanvasSurface(this.js5.html5_canvas.id);
            console.log( "CPU mode" );
        }

        this.js5.canvas = this.js5.surface.getCanvas();

        this.js5.width = this.js5.surface.width();
        this.js5.height = this.js5.surface.height();

        // Public to app
        this.width = this.js5.width;
        this.height = this.js5.height;
    }

    initCanvas( canvas, mode ) {
        this.js5.html5_canvas = canvas;
        this.js5.surface_mode = mode;

        if( mode == 1 ) {
            this.js5.surface = CanvasKit.MakeCanvasSurface(this.js5.html5_canvas.id);
            console.log( "GPU mode" );
        }
        else {
            this.js5.surface = CanvasKit.MakeSWCanvasSurface(this.js5.html5_canvas.id);
            console.log( "CPU mode" );
        }
        // /*
        let interact = (e) => {
            let type = e.type;
            
            this.app.mouseX = e.offsetX;
            this.app.mouseY = e.offsetY;

            if (type === 'mouseleave' || 
            	type === 'mouseup' ) {
                this.app.mouseIsPressed = false;
                this.app.mouseButton = -1;
                if( type === 'mouseup' ||
                	type === 'touchend' ) {
                    if( this.app.mouseReleased ) {
                        this.app.mouseReleased();
                    }
                }
                return;
            }
            else if( type === 'mousedown' ) {
                this.app.mouseIsPressed = true;
                this.app.mouseButton = e.button;
                this.app.pmouseX = this.app.mouseX;
                this.app.pmouseY = this.app.mouseY;
                if( this.app.mousePressed ) {
                    this.app.mousePressed();
                }
            }
            else if( type === 'mousemove' ) {
                if(this.app.mouseIsPressed){
                    if( this.app.mouseDragged ) {
                        this.app.mouseDragged();
                    }
                }
                else{
                    if( this.app.mouseMoved ) {
                        this.app.mouseMoved();
                    }
                }
            }
            
        }
        
        
        let touchinteract = (e) => {
            let type = e.type;
            
            if (type === 'touchstart' ) {
                var touch = e.touches[0];
                this.app.mouseX = touch.clientX - touch.target.offsetLeft;
                this.app.mouseY = touch.clientY - touch.target.offsetTop;

                this.app.mouseIsPressed = true;
                this.app.mouseButton = e.button;
                this.app.pmouseX = this.app.mouseX;
                this.app.pmouseY = this.app.mouseY;
                if( this.app.mousePressed ) {
                    this.app.mousePressed();
                }
            }
            else if( type === 'touchend' ) {
                this.app.mouseIsPressed = false;
                this.app.mouseButton = -1;
                
                if( this.app.mouseReleased ) {
                    this.app.mouseReleased();
                }
            }
            else if( type === 'touchmove' ) {
                var touch = e.touches[0];
                this.app.mouseX = touch.clientX - touch.target.offsetLeft;
                this.app.mouseY = touch.clientY - touch.target.offsetTop;
                
                this.app.mouseIsPressed = false;
                this.app.mouseButton = -1;
                if( type === 'mouseup' ||
                	type === 'touchend' ) {
                    if( this.app.mouseReleased ) {
                        this.app.mouseReleased();
                    }
                }
            }
        }
        // */
        let keyinteract = (e) => {
            let type = e.type;

            if( type === 'keydown' ) {
                this.app.key = e.key;
                this.app.keyCode = e.keyCode;
                if( this.app.keyPressed ) {
                    this.app.keyPressed();
                }
            }
            else if( type === 'keyup' ) {
                this.app.key = e.key;
                this.app.keyCode = e.keyCode;
                if( this.app.keyReleased ) {
                    this.app.keyReleased();
                }
            }
        }
        
				
        /*
        function getModifiers(event) {
            let mod = CanvasKit.ModifierKey.None;
            if (event.shiftKey) {
                mod |= CanvasKit.ModifierKey.Shift;
            } else if (event.ctrlKey) {
                mod |= CanvasKit.ModifierKey.Control;
            } else if (event.altKey) {
                mod |= CanvasKit.ModifierKey.Option;
            } else if (event.metaKey) {
                mod |= CanvasKit.ModifierKey.Command;
            }
            return mod;
        }

        function getKey(event) {
            let key = event.key;
            //console.log(key);
            switch (key) {
                case " ":
                    return CanvasKit.Key.Space;
                case "a":
                    return CanvasKit.Key.keyA;
                case "c":
                    return CanvasKit.Key.keyC;
                case "p":
                    return CanvasKit.Key.keyP;
                case "v":
                    return CanvasKit.Key.keyV;
                case "x":
                    return CanvasKit.Key.keyX;
                case "y":
                    return CanvasKit.Key.keyY;
                case "z":
                    return CanvasKit.Key.keyZ;
                case "0":
                    return CanvasKit.Key.key0;
                case "1":
                    return CanvasKit.Key.key1;
                case "2":
                    return CanvasKit.Key.key2;
                case "3":
                    return CanvasKit.Key.key3;
                case "4":
                    return CanvasKit.Key.key4;
                case "5":
                    return CanvasKit.Key.key5;
                case "6":
                    return CanvasKit.Key.key6;
                case "7":
                    return CanvasKit.Key.key7;
                case "8":
                    return CanvasKit.Key.key8;
                case "9":
                    return CanvasKit.Key.key9;
                case "ArrowLeft":
                    return CanvasKit.Key.Left;
                case "ArrowRight":
                    return CanvasKit.Key.Right;
                case "ArrowUp":
                    return CanvasKit.Key.Up;
                case "ArrowDown":
                    return CanvasKit.Key.Down;
                case "Tab":
                    return CanvasKit.Key.Tab;
                case "Shift":
                    return CanvasKit.Key.Shift;
                case "Control":
                    return CanvasKit.Key.Ctrl;
                case "Alt":
                    return CanvasKit.Key.Option;
                case "Meta":
                    return CanvasKit.Key.Super;
                default:
                    console.log("Invalid Key");
                    return 0;
            }
            return key;
        }

        function interact(e) {
            if (!app) {
                return;
            }
            const canvas = this;
            const rect = canvas.getBoundingClientRect();
            x = (e.offsetX / (rect.right - rect.left)) * canvas.width;
            y = (e.offsetY / (rect.bottom - rect.top)) * canvas.height;

            let type = e.type;
            if (type === "lostpointercapture" || type === "pointerup") {
                touched = false;
                app.onAppMouse(
                    x,
                    y,
                    CanvasKit.InputState.Up,
                    getModifiers(e)
                );
            }
            if (touched) {
                app.onAppMouse(
                    x,
                    y,
                    CanvasKit.InputState.Move,
                    getModifiers(e)
                );
            } else {
                if (e.pressure) {
                    touched = true;
                    app.onAppMouse(
                        x,
                        y,
                        CanvasKit.InputState.Down,
                        getModifiers(e)
                    );
                } else {
                    app.onAppMouse(
                        x,
                        y,
                        CanvasKit.InputState.Move,
                        getModifiers(e)
                    );
                }
            }
        }

        function wheel(event) {
            event.preventDefault();
            if (this.app) {
                let w = 0 - event.deltaY;
                let wx = 0 - event.deltaX;              
                this.app.onAppMouseWheel(wx, w, getModifiers(event));
            }
        }
        */
		// /*		
        this.js5.html5_canvas.addEventListener('mousemove', interact);
        this.js5.html5_canvas.addEventListener('mousedown', interact);
        //this.js5.html5_canvas.addEventListener('mouseleave', interact);
        this.js5.html5_canvas.addEventListener('mouseup', interact);
        this.js5.html5_canvas.addEventListener('touchmove', touchinteract);
        this.js5.html5_canvas.addEventListener('touchstart', touchinteract);
        this.js5.html5_canvas.addEventListener('touchend', touchinteract);
        window.addEventListener('keydown', keyinteract, true);
        window.addEventListener('keyup', keyinteract, true);
        // */
        this.js5.canvas = this.js5.surface.getCanvas();
		/*
        canvas.addEventListener("touchmove", (e) => {
            // Prevents touch events in the canvas from scrolling the canvas.
            e.preventDefault();
            e.stopPropagation();
        });
        canvas.addEventListener("pointermove", interact);
        canvas.addEventListener("pointerdown", interact);
        canvas.addEventListener("lostpointercapture", interact);
        canvas.addEventListener("pointerup", interact);
        canvas.onwheel = wheel;

        document.addEventListener("keypress", (event) => {
            let mod = getModifiers(event);
            if (this.app) {
                //this.app.onAppChar(event.key.charCodeAt(0), mod);
            }
        });

        document.addEventListener("keydown", (event) => {
            let mod = getModifiers(event);
            let key = getKey(event);
            if (this.app) {
                //this.app.onAppKey(key, CanvasKit.InputState.Down, mod);
            }
        });
        document.addEventListener("keyup", (event) => {
            let mod = getModifiers(event);
            let key = getKey(event);
            if (this.app) {
                //this.app.onAppKey(key, CanvasKit.InputState.Up, mod);
            }
        });
        */
        this.js5.width = this.js5.surface.width();
        this.js5.height = this.js5.surface.height();

        // Public to app
        this.app.width = this.js5.width;
        this.app.height = this.js5.height;
    }

    preRun() {
        if( this.tagID ) {
            console.log( this.tagID );
            let canvas = document.getElementById(this.tagID);
            if( canvas ) {
                this.initCanvas( canvas, this.surfaceMode );
            }
        } 
    }

    run( isMultiCanvas = 0 ) {
        var that = this;

        var fpsInterval = 1000 / this.js5.currentFrameRate;
        var then = Date.now();
        var startTime = then;

        function drawFrame(canvas) {
            if( that.app.loopEnabled ) {
                window.requestAnimationFrame(drawFrame);
            }

            // calc elapsed time since last loop
            let now = Date.now();
            let elapsed = now - then;

            // if enough time has elapsed, draw the next frame
            if (elapsed > fpsInterval || !that.app.loopEnabled) {
                then = now - (elapsed % fpsInterval);

                if( isMultiCanvas ) {
                    for( var i=0; i<that.js5.canvas_infos.length; i++ ) {
                        var canvas_info = that.canvas_infos[i];
                        var saveCount = canvas_info.canvas.save();
                        if( typeof canvas_info.draw == 'function' ) {
                            that.js5.canvas = canvas_info.canvas;
                            that.js5.surface = canvas_info.surface;
                            window.width = canvas_info.width;
                            window.height = canvas_info.height;
                            canvas_info.draw( canvas_info );
                        }
                        canvas_info.canvas.restoreToCount(saveCount);
                        canvas_info.surface.flush();
                    }
                }
                else {
                    if( that.js5.canvas )
                        var saveCount = that.js5.canvas.save();
                    that.app.draw();
                    if( that.js5.canvas )
                        that.js5.canvas.restoreToCount(saveCount);

                    if( that.js5.surface )
                        that.js5.surface.flush();
                    if( that.js5.snapshotNeed ) {
                        that.js5.snapshotNeed = false;
                        that.js5.doSaveCanvas();
                    }
                }
            }
        }
        drawFrame(this.js5.canvas);
    }

    colorMode( mode, max0 = undefined, max1 = undefined, max2 = undefined, max3 = undefined ) {
        this.js5.currentColorMode = mode;
        if( mode === this.js5.RGB ) {
            if( max0 === undefined ) 
                max0 = 255;
            if( max2 === undefined )
                max2 = max0;
            if( max1 === undefined ) 
                max1 = max0;
            if( max3 === undefined )
                max3 = 255;
        }
        else {
            if( max0 === undefined ) 
                max0 = 360;
            if( max1 === undefined )
                max1 = 100;
            if( max2 === undefined ) 
                max2 = 100;
            if( max3 === undefined )
                max3 = 1;                
        }
        this.js5.currentColorMax[0] = max0;
        this.js5.currentColorMax[1] = max1;
        this.js5.currentColorMax[2] = max2;
        this.js5.currentColorMax[3] = max3;
    }

    toBase64String(bytes) {

          // From https://stackoverflow.com/a/25644409
          // because the naive solution of
          //     btoa(String.fromCharCode.apply(null, bytes));
          // would occasionally throw "Maximum call stack size exceeded"
          var CHUNK_SIZE = 0x8000; //arbitrary number
          var index = 0;
          var length = bytes.length;
          var result = '';
          var slice;
          while (index < length) {
            slice = bytes.slice(index, Math.min(index + CHUNK_SIZE, length));
            result += String.fromCharCode.apply(null, slice);
            index += CHUNK_SIZE;
          }
          return btoa(result);

    }

    saveCanvas( name, codec ) {
        if( codec === this.PNG ) {
            this.js5.snapshotCodec = 'image/png';
            this.js5.snapshotFormat = CanvasKit.ImageFormat.PNG;
            this.js5.snapshotName = name + '.png';
        }
        else if( codec === this.JPEG || codec === this.JPG ) {
            this.js5.snapshotCodec = 'image/jpeg';
            this.js5.snapshotFormat = CanvasKit.ImageFormat.JPEG;
            this.js5.snapshotName = name + '.jpg';
        }
        else {
            this.js5.snapshotNeed = false;
            return;
        }
        this.js5.snapshotNeed = true;
    }

    doSaveCanvas() {
        if( this.js5.surface ) {
            let img = this.js5.surface.makeImageSnapshot();

            var skimg = img.encodeToData(this.js5.snapshotFormat, 0.92);
            var imgBytes = CanvasKit.getSkDataBytes(skimg);
            var data =  'data:' + this.js5.snapshotCodec + ';base64,' + this.js5.toBase64String(imgBytes);

            var dataURL = data.replace(this.js5.snapshotCodec, "image/octet-stream");

            var link = document.createElement('a');
            link.href = dataURL;
            link.download = this.snapshotName;
            link.click();
        }
    }

    createSkColor( r, g, b, a ) {
        if( b === undefined && a === undefined && r.mode !== undefined ) {
            if( r.mode === this.js5.HSB ) {
                let rgb = this.js5.HSBtoRGB( r.r, r.g, r.b, r.a );
                if( g !== undefined )
                    rgb.a = g;
                return CanvasKit.Color( rgb.r, rgb.g, rgb.b, rgb.a/255 );
            }
            else if( r.mode === this.js5.HSL ) {
                let hsb = this.js5.HSLtoHSB( r.r, r.g, r.b, r.a );
                let rgb = this.js5.HSBtoRGB( hsb.h, hsb.s, hsb.v, hsb.a );
                if( g !== undefined )
                    rgb.a = g;                
                return CanvasKit.Color( rgb.r, rgb.g, rgb.b, a );
            }
            if( g !== undefined )
                return CanvasKit.Color( r.r, r.g, r.b, g/255 );
            return CanvasKit.Color( r.r, r.g, r.b, r.a/255 );
        }
        if( this.js5.currentColorMode === this.js5.RGB ) {
            if( b === undefined && a === undefined && g != undefined && r != undefined ) {
                a = g;
                g = r;
                b = r;
            }
            else {
                if( a === undefined )
                    a = 255;
                if( g === undefined && b === undefined ) {
                    g = r;
                    b = r;
                }
            }
            return CanvasKit.Color(
                r * 255 / this.js5.currentColorMax[0], 
                g * 255 / this.js5.currentColorMax[1], 
                b * 255 / this.js5.currentColorMax[2], 
                (a * 255 / this.js5.currentColorMax[3])/255);
        }
        else if( this.js5.currentColorMode === this.js5.HSB ) {
            if( b === undefined && a === undefined && g != undefined && r != undefined ) {
                a = g;
                g = r;
                b = r;
            }
            else {            
                if( a === undefined )
                    a = 1.0;
            }
            let rgb = this.js5.HSBtoRGB( 
                r * 1.0 / this.js5.currentColorMax[0],
                g * 1.0 / this.js5.currentColorMax[1],
                b * 1.0 / this.js5.currentColorMax[2],
                a * 1.0 / this.js5.currentColorMax[3]
            );
            return CanvasKit.Color( rgb.r, rgb.g, rgb.b, rgb.a/255 );
        }
    }

    red( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.RGB ) {
                return color.r * this.js5.currentColorMax[0] / 255;
            }
            else if( color.mode === this.js5.HSB ) {
                let newColor = this.js5.HSBtoRGB( color.r, color.g, color.b, color.a );
                return newColor.r * this.js5.currentColorMax[0] / 255;
            }
        }
        return 0;
    }

    green( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.RGB ) {
                return color.g * this.js5.currentColorMax[1] / 255;
            }
            else if( color.mode === this.js5.HSB ) {
                let newColor = this.js5.HSBtoRGB( color.r, color.g, color.b, color.a);
                return newColor.g * this.js5.currentColorMax[1] / 255;
            }
        }
        return 0;
    }    

    blue( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.RGB ) {
                return color.b * this.js5.currentColorMax[2] / 255;
            }
            else if( color.mode === this.js5.HSB ) {
                let newColor = this.js5.HSBtoRGB( color.r, color.g, color.b, color.a );
                return newColor.b * this.js5.currentColorMax[2] / 255;
            }
        }
        return 0;
    }  
    
    hue( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.HSB ) {
                return color.r * this.js5.currentColorMax[0] / 1.0;
            }
            else if( color.mode === this.js5.RGB ) {
                let newColor = this.js5.RGBtoHSB( color.r, color.g, color.b, color.a );
                return newColor.r * this.js5.currentColorMax[0] / 1.0;
            }
        }
        return 0;
    }    

    saturation( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.HSB ) {
                return color.g * this.js5.currentColorMax[1] / 1.0;
            }
            else if( color.mode === this.js5.RGB ) {
                let newColor = this.js5.RGBtoHSB( color.r, color.g, color.b, color.a );
                return newColor.g * this.js5.currentColorMax[1] / 1.0;
            }
        }
        return 0;
    }     
    
    brightness( color ) {
        if( color.mode !== undefined ) {
            if( color.mode === this.js5.HSB ) {
                return color.b * this.js5.currentColorMax[2] / 1.0;
            }
            else if( color.mode === this.js5.RGB ) {
                let newColor = this.js5.RGBtoHSB( color.r, color.g, color.b, color.a );
                return newColor.b * this.js5.currentColorMax[2] / 1.0;
            }
        }
        return 0;
    }      

    color( r, g, b, a ) {
        if( r.mode !== undefined ) {
            return this.js5.colorToColorCurrentMode(r);
        }
        if( this.js5.currentColorMode === this.js5.RGB ) {
            if( b === undefined && a === undefined && g != undefined && r != undefined ) {
                a = g;
                g = r;
                b = r;
            }
            else {            
                if( a === undefined )
                    a = 255;
                if( g === undefined && b === undefined ) {
                    g = r;
                    b = r;
                }
            }
            return {
                r: r * 255 / this.js5.currentColorMax[0], 
                g: g * 255 / this.js5.currentColorMax[1], 
                b: b * 255 / this.js5.currentColorMax[2], 
                a: a * 255 / this.js5.currentColorMax[3],
                mode:  this.js5.currentColorMode,
                toString: function() {
                    return "" + this.r + ","+this.g+","+this.b;
                }
            }
        }
        else  {
            if( b === undefined && a === undefined && g != undefined && r != undefined ) {
                a = g;
                g = r;
                b = r;
            }
            else {              
                if( a === undefined )
                    a = 1.0;
            }
            return {
                r: r * 1.0 / this.js5.currentColorMax[0],
                g: g * 1.0 / this.js5.currentColorMax[1],
                b: b * 1.0 / this.js5.currentColorMax[2],
                a: a * 1.0 / this.js5.currentColorMax[3],
                mode:  this.js5.currentColorMode,
                toString: function() {
                    return "" + this.r + ","+this.g+","+this.b;
                }
            }
        }
    }

    createVector( x, y ) {
        return { 
            x: x, 
            y: y,
            add: function( dx, dy ) {
                if( dy === undefined  ) {
                    if( dx.x !== undefined) {
                        this.x = this.x + dx.x;
                        this.y = this.y + dx.y;
                    }
                }
                else {
                    this.x = this.x + dx;
                    this.y = this.y + dy;
                }
            },
            rotate: function( radians ) {
                var ca = Math.cos(radians);
                var sa = Math.sin(radians);
                this.x = ca*this.x - sa*this.y;
                this.y = sa*this.x + ca*this.y;
            }
        };
    }

    clear() {
        this.js5.canvas.clear( this.js5.backgroundColor );
    }

    background( r, g=undefined, b=undefined, a=undefined ) {
        this.js5.backgroundColor = this.js5.createSkColor(r, g, b, a);
        this.js5.canvas.clear( this.js5.backgroundColor );
    }

    cursor( cursorType ) {
        this.js5.html5_canvas.style.cursor = cursorType;
    }

    noCursor() {
        this.js5.html5_canvas.style.cursor = 'none';
    }

    rectMode( mode ) {
        this.js5.currentRectMode = mode;
    }

    imageMode( mode ) {
        this.js5.currentImageMode = mode;
    }

    frameRate( framerate ) {
        this.js5.currentFrameRate = framerate;
    }

    fill( r, g=undefined, b=undefined, a=undefined ) {
        this.js5.fillColor = this.js5.createSkColor(r, g, b, a);
        this.js5.isNoFill = false;
    }

    noFill() {
        this.js5.isNoFill = true;
    }
    
    stroke( r, g=undefined, b=undefined, a=undefined ) {
        this.js5.strokeColor = this.js5.createSkColor(r, g, b, a);
        this.js5.isNoStroke = false;
    }

    noStroke() {
        this.js5.isNoStroke = true;
    }

    strokeWeight( weight ) {
        this.js5.strokeWidth = weight;
    }
    
    strokeJoin( join ) {
        this.js5.strokeJoinType = this.js5.joinType[join];
    }
 
    strokeCap( cap ) {
        this.js5.strokeCapType = this.js5.capType[cap];
    }    

    loadFont( fontURL ) {
        this.js5.font_resources[fontURL] = fontURL;
        return fontURL;
    }

    loadImage( imgURL ) {
        this.js5.image_resources[imgURL] = { 'name': imgURL };
        return this.js5.image_resources[imgURL];
    }

  
    resizeImage( image, w, h ) {
        let img = image['image'];

        const surf = window.CanvasKit.MakeSurface(w, h);
        const tmpCanvas = surf.getCanvas();
        var paint = new CanvasKit.Paint();
        const destBounds = CanvasKit.LTRBRect( 0, 0, w, h );

        tmpCanvas.drawImageRect(img, image.bounds, destBounds, paint, true );

        paint.delete();
        image['image'] = surf.makeImageSnapshot();
        surf.delete();
    }

    loadPixels( image ) {
        let img = image['image'];

        var imageInfo = {
            'width':  image.width,
            'height': image.height,
            // RGBA 8888 is the only pixel format we can show on an HTML canvas
            'colorType': CanvasKit.ColorType.RGBA_8888,
            // We are sending these pixels directly into the HTML canvas,
            // (and those pixels are un-premultiplied, i.e. straight r,g,b,a)
            'alphaType': CanvasKit.AlphaType.Unpremul,
        }
        image['pixels'] = img.readPixels( imageInfo, 0, 0 );
    }

    textFont( fontName ) {
        this.js5.typeface = fontName;
        this.js5.deleteCurrentFont();
    }
    
    textSize( fontSize ) {
        this.js5.fontSize = fontSize;
        this.js5.deleteCurrentFont();
    }

    deleteCurrentFont() {
        if( this.js5.font ) {
            this.js5.font.delete();
            this.js5.font = null;
        }
    }

    createFont( resName, size ) {
        if( this.js5.font_resources[resName] ) {
            let font = new CanvasKit.Font(this.js5.font_resources[resName], size);
            return font;
        }
        else {
            let font = new CanvasKit.Font(null, size);
            return font;
        }
    }
    
    textPath( str, x, y ) {
        if( !this.js5.font )
            this.js5.font = this.js5.createFont(this.js5.typeface, this.js5.fontSize);
        //return this.js5.font.getPath(str, x, y);
				return CanvasKit.Path.MakeFromText(str, this.js5.font, x, y)
    }
     
    text( str, x, y ) {
        let path = this.js5.textPath( str, x, y  );
    
        this.js5.drawPath( path );
        
        path.delete();
    }

    fillPath( path ) {
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawPath(path, this.js5.paint);    
        }
    }
    
    strokePath( path ) {
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawPath(path, this.js5.paint);
        }
    }
    
    drawPath( pathin ) {
        if( !this.js5.isNoFill )
            this.js5.fillPath( pathin );
        if( !this.js5.isNoStroke )
            this.js5.strokePath( pathin );
    }

    grid() {
        var stColor = this.js5.strokeColor;
        var nStroke = this.js5.isNoStroke;
        this.stroke( 0, 0, 0, 75 );
        for( var i=0; i<this.js5.width; i+=100 ) {
            this.line( i, 0, i, this.js5.height );
        }
        for( var i=0; i<this.js5.height; i+= 100 ) {
            this.line( 0, i, this.js5.width, i );
        }
        this.js5.strokeColor = stColor;
        this.js5.isNoStroke = nStroke;
    }

    setFillParams() {
        if( this.js5.isNoFill )
            return false;
        if( this.js5.fillColor & 0xFF000000 ) {
            this.js5.paint.setColor(this.js5.fillColor);
            this.js5.paint.setStyle(CanvasKit.PaintStyle.Fill);
            return true;
        }
        return false;
    }

    setStrokeParams() {
        if( this.js5.isNoStroke )
            return false;
        if( this.js5.strokeWidth > 0 ) {
            this.js5.paint.setColor(this.js5.strokeColor);
            this.js5.paint.setStrokeWidth(this.js5.strokeWidth);
            this.js5.paint.setStrokeJoin(this.js5.strokeJoinType);
            this.js5.paint.setStrokeCap(this.js5.strokeCapType);
            this.js5.paint.setStyle(CanvasKit.PaintStyle.Stroke);
            return true;
        }
        return false;
    }

    point( x, y ) {
        this.js5.rect( x, y, 1, 1 );
    }

    line( x1, y1, x2, y2 ) {
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawLine( x1, y1, x2, y2, this.js5.paint );
        }
    }

    rect( x, y, w, h ) {
        if( this.js5.currentRectMode == this.js5.CENTER ) {
            x = x - (w/2);
            y = y - (h/2);
        }
            
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawRect(CanvasKit.LTRBRect(x, y, x+w, y+h), this.js5.paint);
        }
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawRect(CanvasKit.LTRBRect(x, y, x+w, y+h), this.js5.paint);
        }
    }

    roundrect( x, y, w, h, rx, ry ) {
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawRRect(CanvasKit.RRectXY(CanvasKit.LTRBRect(x, y, x+w, y+h), rx, ry), this.js5.paint);
        }
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawRRect(CanvasKit.RRectXY(CanvasKit.LTRBRect(x, y, x+w, y+h), rx, ry), this.js5.paint);
        }
    }
    
    circle( x, y, r ) {
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawCircle( x, y, r, this.js5.paint );
        }
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawCircle( x, y, r, this.js5.paint );
        }
    }

    ellipse( x, y, w, h ) {
        if( h == undefined ) {
            h = w;
        }
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawOval(CanvasKit.XYWHRect(x-w/2, y-h/2, w, h), this.js5.paint );
        }
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawOval(CanvasKit.XYWHRect(x-w/2, y-h/2, w, h), this.js5.paint );
        }
    }

    arc( x, y, w, h, start, stop, center ) {
        if( this.js5.setFillParams() ) {
            this.js5.canvas.drawArc(CanvasKit.XYWHRect(x-w/2, y-h/2, w, h), start, stop-start, true, this.js5.paint );
        }
        if( this.js5.setStrokeParams() ) {
            this.js5.canvas.drawArc(CanvasKit.XYWHRect(x-w/2, y-h/2, w, h), start, stop-start, center, this.js5.paint );
        }
    }

    createPath() {
        const path = new CanvasKit.SkPath();
        return path;
    }

    createPathFromSVG( str ) {
        let path = CanvasKit.MakePathFromSVGString( str );
        var bounds = path.getBounds();
        let ox = (bounds.fRight + bounds.fLeft) / 2;
        let oy = (bounds.fBottom + bounds.fTop) / 2;
        let mt = CanvasKit.SkMatrix.translated( -ox, -oy );
        path.transform(mt)
        return path;
    }

    polygonPath( lines ) {
        const path = new CanvasKit.SkPath();
        path.moveTo( lines[0], lines[1] );
        for( let i=2; i<lines.length; i+=2 ) {
            path.lineTo( lines[i], lines[i+1] );
        }
        return path;
    }

    points( lines ) {
        if( lines.length < 2 )
            return;

        for( var i=0; i<lines.length; i+=2 ) {
            this.js5.point( lines[i], lines[i+1] );
        }
    }

    polyline( lines ) {
        if( lines.length < 2 )
            return;

        const path = this.js5.polygonPath( lines );
        this.js5.strokePath( path );
    }

    polygon( lines ) {
        if( lines.length < 2 )
            return;
    
        let path = this.js5.polygonPath( lines );
        path.close();
        this.js5.drawPath( path );
    }

    polygonOutline( lines ) {
        if( lines.length <= 4 )
            return;
    
        let path = this.js5.polygonPath( lines );
        path.close();
        path.simplify();
        this.js5.drawPath( path );
    }

    triangle( x1, y1, x2, y2, x3, y3 ) {
        let path = this.js5.polygonPath([x1, y1, x2, y2, x3, y3]);
        path.close();
        this.js5.drawPath( path );
    }

    quad( x1, y1, x2, y2, x3, y3, x4, y4 ) {
        let path = this.js5.polygonPath([x1, y1, x2, y2, x3, y3, x4, y4]);
        path.close();
        this.js5.drawPath( path );
    }

    cubic( x1, y1, x2, y2, x3, y3, x4, y4 ) {
        const path = new CanvasKit.SkPath();
        path.moveTo( x1, y1 );
        path.cubicTo( x2, y2, x3, y3, x4, y4 );
        this.js5.strokePath( path );
    }

    quadratic( x1, y1, x2, y2, x3, y3 ) {
        const path = new CanvasKit.SkPath();
        path.moveTo( x1, y1 );
        path.quadTo( x2, y2, x3, y3 );
        this.js5.strokePath( path );
    }
   
    conic( x1, y1, x2, y2, x3, y3, w ) {
        const path = new CanvasKit.SkPath();
        path.moveTo( x1, y1 );
        path.conicTo( x2, y2, x3, y3, w );
        this.js5.strokePath( path );
    }

    image( img, x, y, w, h ) {
        var tx = x;
        var ty = y;
        var bounds = img.bounds;


        this.js5.canvas.save();
        if( img.paths !== undefined) {
            if( this.js5.currentImageMode === this.js5.CENTER ) {
                tx -= (bounds.fRight + bounds.fLeft) / 2;
                ty -= (bounds.fBottom + bounds.fTop) / 2;
            }
            else {
                tx -= bounds.fLeft;
                ty -= bounds.fTop;
            }
            this.js5.scale( w / (bounds.fRight - bounds.fLeft), h / (bounds.fBottom - bounds.fTop) );
            this.js5.translate( tx, ty );
            for( let i=0; i<img.paths.length; i++){
                let path = img.paths[i][0];
                var paint = new CanvasKit.Paint();
                paint.setAntiAlias(true);
                if(img.paths[i][2]){
                    let c = img.paths[i][2];
                    paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a/255));
                    paint.setStyle(CanvasKit.PaintStyle.Fill);
                    this.js5.canvas.drawPath(path, paint);
                }
                else{
                    // this.noFill();
                }
                if(img.paths[i][1]){
                    let c = img.paths[i][1];
                    paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                    paint.setStrokeWidth(img.paths[i][3]);
                    paint.setStyle(CanvasKit.PaintStyle.Stroke);
                    this.js5.canvas.drawPath(path, paint);
                }
                else{
                    // this.noStroke();
                }            
            }
        }
        else if( img.image != undefined) {
            let destBounds = CanvasKit.LTRBRect( x, y, x+w, y+h);
            var paint = new CanvasKit.Paint();
            this.js5.canvas.drawImageRect(img.image, img.bounds, destBounds, paint, true );
        }
        this.js5.canvas.restore();
    }

    rotate( rad ) {
        this.js5.canvas.rotate(rad * 180 / Math.PI, 0, 0);
    }

    scale( x, y ) {
        if( y === undefined )
        y = x;
        this.js5.canvas.scale(x, y);
    }

    translate( x, y ) {
        this.js5.canvas.translate(x, y);
    }

    save() {
        this.js5.canvas.save();
    }

    restore() {
        this.js5.canvas.restore();
    }

    radians( degree ) {
        return( degree * Math.PI / 180.0 );
    }

    scalePath( path, scaleX, scaleY ) {
        if( scaleY === undefined )
            scaleY = scaleX;
        let ms = CanvasKit.SkMatrix.scaled( scaleX, scaleY );
        path.transform(ms);
    }
    
    rotatePath( path, degree ) {
        let mr = CanvasKit.SkMatrix.rotated( degree * Math.PI / 180.0 );
        path.transform(mr);
    }
    
    translatePath( path, x, y ) {
        let mt = CanvasKit.SkMatrix.translated( x, y );
        path.transform(mt);
    }



    drawPesStitches(pesdata){
        const bound = pesdata.getBoundingBox();
        // console.log("w=" + bound.width + " h="+bound.height);
        this.js5.canvas.translate(-bound.x, -bound.y);
        var paint = new CanvasKit.Paint();
        paint.setStyle(CanvasKit.PaintStyle.Stroke);
        paint.setStrokeWidth(1);
        paint.setAntiAlias(true);
        const blocks = pesdata.getStitchBlockList();
        var skPath = new CanvasKit.SkPath();
        for (let i=0; i<blocks.size(); i++) {
            skPath.reset();
            const vertices = blocks.get(i).polyline.getVertices();
            for(let j=0; j<vertices.size(); j++){
                const v = vertices.get(j);
                if(j==0){
                    skPath.moveTo(v.x, v.y);
                }
                else{
                    skPath.lineTo(v.x, v.y);
                }
            }

            paint.setColor( CanvasKit.Color(blocks.get(i).color.r, blocks.get(i).color.g, blocks.get(i).color.b, 255) );
            this.js5.canvas.drawPath(skPath, paint);
        }
        skPath.delete();
        paint.delete();
        this.js5.canvas.translate(bound.x, bound.y);
    }

    
    // makePesImageSnapshot(pesdata){
    //     const bound = pesdata.getBoundingBox();
    //     // console.log("w=" + bound.width + " h="+bound.height);
    //     if(bound.width>0 && bound.height>0){
    //         var intw = Math.ceil( bound.width );
    //         var inth = Math.ceil( bound.height );
    //         const surf = window.CanvasKit.MakeSurface(intw, inth);
    //         const tmpCanvas = surf.getCanvas();
    //         // tmpCanvas.scale(0.5, 0.5);
    //         tmpCanvas.translate(-bound.x, -bound.y);
    //         tmpCanvas.clear(CanvasKit.TRANSPARENT);
    //         var paint = new CanvasKit.Paint();
    //         paint.setStyle(CanvasKit.PaintStyle.Stroke);
    //         paint.setStrokeWidth(1);
    //         paint.setAntiAlias(true);
    
    //         const blocks = pesdata.getStitchBlockList();
    //         var skPath = new CanvasKit.SkPath();
    //         for (let i=0; i<blocks.size(); i++) {
    //             skPath.reset();
    //             const vertices = blocks.get(i).polyline.getVertices();
    //             for(let j=0; j<vertices.size(); j++){
    //                 const v = vertices.get(j);
    //                 if(j==0){
    //                     skPath.moveTo(v.x, v.y);
    //                 }
    //                 else{
    //                     skPath.lineTo(v.x, v.y);
    //                 }
    //             }
    
    //             paint.setColor( CanvasKit.Color(blocks.get(i).color.r, blocks.get(i).color.g, blocks.get(i).color.b, 255) );
    //             tmpCanvas.drawPath(skPath, paint);
    //         }
    //         paint.delete();
    //         skPath.delete();
    //         return surf.makeImageSnapshot();
    //     }
    // }

    makePesImageSnapshot(pesdata){
        const bound = pesdata.getBoundingBox();
        // console.log("w=" + bound.width + " h="+bound.height);
        if(bound.width>0 && bound.height>0){
            var intw = Math.ceil( bound.width );
            var inth = Math.ceil( bound.height );
            // const surf = window.CanvasKit.MakeSurface(intw, inth);
            // const tmpCanvas = surf.getCanvas();
            // // tmpCanvas.scale(0.5, 0.5);
            // tmpCanvas.translate(-bound.x, -bound.y);
            // tmpCanvas.clear(CanvasKit.TRANSPARENT);
            if(this.offScreenSurface==null){
                this.offScreenSurface = CanvasKit.MakeSurface(intw, inth);
            }

            // create a subsurface as a temporary workspace.
            var subSurface = this.offScreenSurface.makeSurface({
                width: intw,
                height: inth,
                alphaType: CanvasKit.AlphaType.Premul,
                colorType: CanvasKit.ColorType.RGBA_8888,
            });

            var subCanvas = subSurface.getCanvas();
            subCanvas.translate(-bound.x, -bound.y);
            subCanvas.clear(CanvasKit.TRANSPARENT);


            var paint = new CanvasKit.Paint();
            paint.setStyle(CanvasKit.PaintStyle.Stroke);
            paint.setStrokeWidth(1);
            paint.setAntiAlias(true);
    
            const blocks = pesdata.getStitchBlockList();
            var skPath = new CanvasKit.SkPath();
            for (let i=0; i<blocks.size(); i++) {
                skPath.reset();
                const vertices = blocks.get(i).polyline.getVertices();
                paint.setColor( CanvasKit.Color(blocks.get(i).color.r, blocks.get(i).color.g, blocks.get(i).color.b, 255) );
                for(let j=0; j<vertices.size(); j++){
                    const v = vertices.get(j);
                    if(j==0){
                        skPath.moveTo(v.x, v.y);
                        subCanvas.drawCircle(v.x, v.y, 1, paint);
                    }
                    else{
                        skPath.lineTo(v.x, v.y);
                        subCanvas.drawCircle(v.x, v.y, 1, paint);
                    }
                }
    
                subCanvas.drawPath(skPath, paint);
            }
            // paint.delete();
            // skPath.delete();
            // return surf.makeImageSnapshot();

            const img = subSurface.makeImageSnapshot();
            subSurface.delete();
            paint.delete();
            skPath.delete();
            return img;
        }
    }

    drawPesScalable(pesdata){
        const bound = pesdata.getBoundingBox();
        this.js5.canvas.translate(-bound.x, -bound.y);
        var paint = new CanvasKit.Paint();
        paint.setAntiAlias(true);
        for(let i=0; i<pesdata.paths.size(); i++){
            var path = pesdata.paths.get(i);
            var skpath = CanvasKit.pesUtility.PesPathToSkPath(path);
            if(path.isFill()){
                var c = path.getFillColor();
                paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                paint.setStyle(CanvasKit.PaintStyle.Fill);
                this.js5.canvas.drawPath(skpath, paint);
            }
            else{
                // this.noFill();
            }
            if(path.isStroke()){
                var c = path.getStrokeColor();
                paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                paint.setStrokeWidth(path.getStrokeWidth());
                paint.setStyle(CanvasKit.PaintStyle.Stroke);
                this.js5.canvas.drawPath(skpath, paint);
            }
            else{
                // this.noStroke();
            }
        }
        paint.delete();
        this.js5.canvas.translate(bound.x, bound.y);
    }

    // makePesScalableImageSnapshot(pesdata){
    //     const bound = pesdata.getBoundingBox();
    //     // console.log("w=" + bound.width + " h="+bound.height);
    //     if(bound.width>0 && bound.height>0){
    //         var intw = Math.ceil( bound.width );
    //         var inth = Math.ceil( bound.height );
    //         const surf = CanvasKit.MakeSurface(intw, inth);
    //         const tmpCanvas = surf.getCanvas();
    //         // tmpCanvas.scale(0.5, 0.5);
    //         tmpCanvas.translate(-bound.x, -bound.y);
    //         tmpCanvas.clear(CanvasKit.TRANSPARENT);
    //         var paint = new CanvasKit.Paint();
    //         paint.setStrokeWidth(1);
    //         paint.setAntiAlias(true);
    //         if(pesdata.paths.size()){
    //             for(let i=0; i<pesdata.paths.size(); i++){
    //                 var path = pesdata.paths.get(i);
    //                 var skpath = CanvasKit.pesUtility.PesPathToSkPath(path);
    //                 if(path.isFill()){
    //                     var c = path.getFillColor();
    //                     paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
    //                     paint.setStyle(CanvasKit.PaintStyle.Fill);
    //                     tmpCanvas.drawPath(skpath, paint);
    //                 }
    //                 else{
    //                     // this.noFill();
    //                 }
    //                 if(path.isStroke()){
    //                     var c = path.getStrokeColor();
    //                     paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
    //                     paint.setStrokeWidth(path.getStrokeWidth());
    //                     paint.setStyle(CanvasKit.PaintStyle.Stroke);
    //                     tmpCanvas.drawPath(skpath, paint);
    //                 }
    //                 else{
    //                     // this.noStroke();
    //                 }
    //             }
    //         }

    //         paint.delete();
    //         return surf.makeImageSnapshot();
    //     }
    // }

    makePesScalableImageSnapshot(pesdata){
        const bound = pesdata.getBoundingBox();
        // console.log("w=" + bound.width + " h="+bound.height);
        if(bound.width>0 && bound.height>0){
            var intw = Math.ceil( bound.width );
            var inth = Math.ceil( bound.height );
            if(this.offScreenSurface==null){
                this.offScreenSurface = CanvasKit.MakeSurface(intw, inth);
            }
            // const context = CanvasKit.currentContext();
            // const canvas = this.offScreenSurface.getCanvas();

            // create a subsurface as a temporary workspace.
            var subSurface = this.offScreenSurface.makeSurface({
                width: intw,
                height: inth,
                alphaType: CanvasKit.AlphaType.Premul,
                colorType: CanvasKit.ColorType.RGBA_8888,
            });

            var subCanvas = subSurface.getCanvas();
            subCanvas.translate(-bound.x, -bound.y);
            subCanvas.clear(CanvasKit.TRANSPARENT);

            var paint = new CanvasKit.Paint();
            paint.setStrokeWidth(1);
            paint.setAntiAlias(true);
            if(pesdata.paths.size()){
                for(let i=0; i<pesdata.paths.size(); i++){
                    var path = pesdata.paths.get(i);
                    var skpath = CanvasKit.pesUtility.PesPathToSkPath(path);
                    if(path.isFill()){
                        var c = path.getFillColor();
                        paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                        paint.setStyle(CanvasKit.PaintStyle.Fill);
                        subCanvas.drawPath(skpath, paint);
                    }
                    else{
                        // this.noFill();
                    }
                    if(path.isStroke()){
                        var c = path.getStrokeColor();
                        paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                        paint.setStrokeWidth(path.getStrokeWidth());
                        paint.setStyle(CanvasKit.PaintStyle.Stroke);
                        subCanvas.drawPath(skpath, paint);
                    }
                    else{
                        // this.noStroke();
                    }
                }
            }

            const img = subSurface.makeImageSnapshot();
            subSurface.delete();
            paint.delete();

            return img;
        }
    }

    // accepts parameters
    //* h  Object = {h:x, s:y, v:z}
    //* OR 
    //* h, s, v
    //
    HSBtoRGB(h, s, v, a) {
        if( a === undefined ) {
            a = 1;
        }

        h = this.clamp( h, 0, 1 );
        s = this.clamp( s, 0, 1 );
        v = this.clamp( v, 0, 1 );

        var r, g, b, i, f, p, q, t;
        if (arguments.length === 1) {
            s = h.s, v = h.v, h = h.h;
        }
        i = Math.floor(h * 6);
        f = h * 6 - i;
        p = v * (1 - s);
        q = v * (1 - f * s);
        t = v * (1 - (1 - f) * s);
        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
        }
        return {
            r: Math.round(r * 255),
            g: Math.round(g * 255),
            b: Math.round(b * 255),
            a: a * 255 / 1.0,
            mode: this.js5.RGB
        };
    }

    // accepts parameters
    //* r  Object = {r:x, g:y, b:z}
    //* OR 
    //* r, g, b
    //
    RGBtoHSB(r, g, b, a) {
        if( a === undefined ) {
            a = 1;
        }

        if (arguments.length === 1) {
            g = r.g, b = r.b, r = r.r;
        }
        var max = Math.max(r, g, b), min = Math.min(r, g, b),
            d = max - min,
            h,
            s = (max === 0 ? 0 : d / max),
            v = max / 255;

        switch (max) {
            case min: h = 0; break;
            case r: h = (g - b) + d * (g < b ? 6: 0); h /= 6 * d; break;
            case g: h = (b - r) + d * 2; h /= 6 * d; break;
            case b: h = (r - g) + d * 4; h /= 6 * d; break;
        }

        return {
            h: h,
            s: s,
            v: v,
            a: a * 1.0 / 255,
            mode: this.js5.HSB
        };
    }

    HSBtoHSL(h, s, v, a) {
        if( a === undefined ) {
            a = 1;
        }
        if (arguments.length === 1) {
            s = h.s, v = h.v, h = h.h;
        }
        var _h = h,
            _s = s * v,
            _l = (2 - s) * v;
        _s /= (_l <= 1) ? _l : 2 - _l;
        _l /= 2;
    
        return {
            h: _h,
            s: _s,
            l: _l,
            a: a
        };
    }
    
    HSLtoHSB(h, s, l, a) {
        if( a === undefined ) {
            a = 1;
        }
        if (arguments.length === 1) {
            s = h.s, l = h.l, h = h.h;
        }
        var _h = h,
            _s,
            _v;
    
        l *= 2;
        s *= (l <= 1) ? l : 2 - l;
        _v = (l + s) / 2;
        _s = (2 * s) / (l + s);
    
        return {
            h: _h,
            s: _s,
            v: _v,
            a: a
        };
    }

    beginShape( mode ) {
        if( !mode ) {
            mode = this.js5.POLYLINES;
        }
        this.js5.shapeMode = mode;
        this.js5.shapeVertices = [];
        this.js5.shapeVertex = null;
    }

    endShape( mode ) {
        if( this.js5.shapeMode === this.js5.TRIANGLE_FAN ||
            this.js5.shapeMode === this.js5.TRIANGLES ||
            this.js5.shapeMode === this.js5.TRIANGLE_STRIP ||
            this.js5.shapeMode === this.js5.QUADS ||
            this.js5.shapeMode === this.js5.QUAD_STRIP) {
            for( var s=0; s<this.js5.shapeVertices.length; s++ ) {
                this.js5.fillColor = this.js5.shapeVertices[s][1]
                this.js5.strokeColor = this.js5.shapeVertices[s][2]
                this.js5.polygon( this.js5.shapeVertices[s][0] );
            }
        }
        else if( this.js5.shapeMode === this.js5.LINES ) {
            for( var s=0; s<this.js5.shapeVertices.length; s++ ) {
                this.js5.strokeColor = this.js5.shapeVertices[s][1]
                this.js5.polyline( this.js5.shapeVertices[s][0] );
            }
        }
        else if( this.js5.shapeMode === this.js5.CURVE ) {
            for( var s=0; s<this.js5.shapeVertices.length; s++ ) {
                this.js5.strokeColor = this.js5.shapeVertices[s][1]
                let p = this.js5.shapeVertices[s][0];
                this.js5.cubic( p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
            }
        }
        else if( this.js5.shapeMode == this.js5.POLYLINES ) {
            if( mode === this.js5.CLOSE ) {
                this.js5.polygon( this.js5.shapeVertices )
            }
            else {
                this.js5.polyline( this.js5.shapeVertices )
            }
        }
        else if( this.js5.shapeMode == this.js5.POINTS ) {
            this.js5.points( this.js5.shapeVertices )
        }
        this.js5.shapeMode = 0;
        this.js5.shapeVertices = null;
    }

    curveVertex( x, y ) {
        this.js5.shapeMode = this.js5.CURVE;
        this.js5.vertex( x, y );
    }

    vertex( x, y ) {
        if( !this.js5.shapeVertex ) {
            this.js5.shapeVertex = [];
        }
        if( this.js5.shapeMode === this.js5.CURVE ) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 8 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.strokeColor] );
                this.js5.shapeVertex = [this.js5.shapeVertex[6], this.js5.shapeVertex[7]];
            }            
        }
        else if( this.js5.shapeMode === this.js5.TRIANGLE_FAN ) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 6 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.fillColor, this.js5.strokeColor] );
                this.js5.shapeVertex = [this.js5.shapeVertex[0], this.js5.shapeVertex[1], this.js5.shapeVertex[4], this.js5.shapeVertex[5]];
            }
        }
        else if( this.js5.shapeMode === this.js5.LINES ) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 4 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.strokeColor] );
                this.js5.shapeVertex = [];
            }
        }
        else if( this.js5.shapeMode === this.js5.TRIANGLES ) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 6 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.fillColor, this.js5.strokeColor] );
                this.js5.shapeVertex = [];
            }
        }       
        else if( this.js5.shapeMode === this.js5.TRIANGLE_STRIP) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 6 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.fillColor, this.js5.strokeColor] );
                this.js5.shapeVertex = [this.js5.shapeVertex[2], this.js5.shapeVertex[3], this.js5.shapeVertex[4], this.js5.shapeVertex[5]];
            }
        }         
        else if( this.js5.shapeMode == this.js5.POLYLINES ||
                 this.js5.shapeMode == this.js5.POINTS ||
                 this.js5.shapsMode == this.js5.LINES ) {
            this.js5.shapeVertices.push( x, y );
        }
        else if( this.js5.shapeMode === this.js5.QUADS) {
            this.js5.shapeVertex.push( x, y );
            if( this.js5.shapeVertex.length == 8 ) {
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.fillColor, this.js5.strokeColor] );
                this.js5.shapeVertex = [];
            }
        }       
        else if( this.js5.shapeMode === this.js5.QUAD_STRIP) {
            if( this.js5.shapeVertex.length == 4 ) {
                this.js5.shapeVertex.push( x, y );
                this.js5.shapeVertex.push( x, y );
            }
            else if( this.js5.shapeVertex.length == 8 ) {
                this.js5.shapeVertex[4] = x;
                this.js5.shapeVertex[5] = y;
                this.js5.shapeVertices.push( [this.js5.shapeVertex, this.js5.fillColor, this.js5.strokeColor] );
                this.js5.shapeVertex = [this.js5.shapeVertex[6], this.js5.shapeVertex[7], this.js5.shapeVertex[4], this.js5.shapeVertex[5]];
            }
            else {
                this.js5.shapeVertex.push( x, y );
            }
        }       
        
    }

    colorToColorCurrentMode( col ) {
        if( this.js5.currentColorMode === this.js5.RGB ) {
            if( col.mode == this.js5.HSB ) {
                return this.js5.HSBtoRGB( col.r, col.g, col.b, col.a );
            }
        }
        return col;
    }

    lerpColor( color1, color2, amount ) {
        var col1 = this.js5.colorToColorCurrentMode(color1);
        var col2 = this.js5.colorToColorCurrentMode(color2);

        var r = (col1.r * (1.0-amount)) + (col2.r * (amount));
        var g = (col1.g * (1.0-amount)) + (col2.g * (amount));
        var b = (col1.b * (1.0-amount)) + (col2.b * (amount));
        var a = (col1.a * (1.0-amount)) + (col2.a * (amount));
        return { r:r, g:g, b:b, a:a, mode: this.js5.currentColorMode };
    }
     


    randomSeed( seed ) {
        this.js5.m_w = (123456789 + seed) & this.js5.mask;
        this.js5.m_z = (987654321 - seed) & this.js5.mask;
    }

    random( min, max ) {
        if( max === undefined ) {
            max = min;
            min = 0;
        }
        this.js5.m_z = (36969 * (this.js5.m_z & 65535) + (this.js5.m_z >> 16)) & this.js5.mask;
        this.js5.m_w = (18000 * (this.js5.m_w & 65535) + (this.js5.m_w >> 16)) & this.js5.mask;
        var result = ((this.js5.m_z << 16) + (this.js5.m_w & 65535)) >>> 0;
        result /= 4294967296;
        

        
        return Math.floor( result * (max - min + 0) ) + min;
    }

    int( num ) {
        return Math.floor( num );
    }

    map( num, in_min, in_max, out_min, out_max) {
        return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    clamp(num, min, max) {
        return num <= min ? min : num >= max ? max : num;
    }

    sqrt( num ) {
        return Math.sqrt(num);
    }

    pow( num1, num2 ) {
        return Math.pow( num1, num2 );
    }

    dist( x1, y1, x2, y2 ) {
        let a = x1-x2;
        let b = y1-y2;
        return Math.sqrt( a*a + b*b );
    }

    min( num1, num2 ) {
        if( num1 < num2 ) 
            return num1;
        return num2;
    }

    max( num1, num2 ) {
        if( num1 > num2 ) 
            return num1;
        return num2;
    }   

    wrap(value, from, to){
        if(from>to){
            function swap(a, b) {
                return [b, a]
              }
            [from, to] = swap(from, to);
        }
        var cycle = to - from;
        if(Math.abs(cycle - 0.0)< Number.EPSILON){ // IsFloatEqual?
            return to;
        }
        return value - cycle * floor((value - from) / cycle);
    }

    createNavBar( parent, className, innerHTML ) {
        var nav = this.js5.createHTMLElement( parent, "nav", null, "navbar has-shadow" );
        nav['brand'] = nav.createElement( 'div', null, 'navbar-brand' );
        nav['burger'] = nav['brand'].createDiv( 'navbar-burger', '<span>Hello</span>' );
        nav['menu'] = nav.createElement( 'div', null, 'navbar-menu' );
        return nav;
    }
    
    createHTMLElement( parent, element, attr, className, innerHTML ) { 
        var x = document.createElement(element);

        if( className )
            x.className = className;

        if( attr )   
            x.setAttribute("type", attr);

        //x.style.display = 'block';
    
        if( innerHTML)
            x.innerHTML = innerHTML;

        if( parent ) {
            if( parent.el ) {
                this._parent = parent.el;
                this._parent.appendChild(x);
            }
            else {
                this._parent = document.getElementById(parent);
                this._parent.appendChild(x);
            }
        }
        else {
            var body = document.getElementsByTagName("body")[0];
            body.appendChild(x);
        }
    
        return { 
            el: x,
            app: this,
            _parent: body,
            id: function( _id ) { this.id = _id; return this; },
            style: function( name, value ) { this.el.style[name] = value; return this; },
            parent: function( parent ) { 
                if( this._parent ) {
                    this._parent.removeChild( this.el );
                }
                if( parent.el ) {
                    this._parent = parent.el;
                    this._parent.appendChild(x);
                }
                else {
                    this._parent = document.getElementById(parent);
                    this._parent.appendChild(this.el);
                }
                return this;
            },
            position: function( x, y ) {
                this.el.style['position'] = 'absolute';
                this.el.style['left'] = '' + x + 'px';
                this.el.style['top'] = '' + y + 'px';

                return this;
            },
            value: function( val ) {
                if( val !== undefined )
                    this.el.value = val;
                return this.el.value;
            },
            createDiv: function( className, innerHTML ) { 
                return this.app.createHTMLElement( this, 'div', null, className, innerHTML );
            },
            createForm: function( className, innerHTML ) { 
                var form = this.app.createHTMLElement( this, 'form', null, className, innerHTML );
                form['addField'] = function( label, inputClass, inputType, placeHolder ) {
                    var field = js5app.createHTMLElement( form, 'div', null, 'field has-text-left' );
                    field.createElement( 'label', null, 'label', label )
                    var input = field.createElement( 'input', inputType, inputClass );
                    input.el.placeholder = placeHolder;
                    return form;
                }
                return form;
            },
            createElement: function( element, attr, className, innerHTML ) {
                return this.app.createHTMLElement( this, element, attr, className, innerHTML );
            }
        };
    }

    createVideo( parent, vdoURL ) {
        var x = document.createElement("VIDEO");
        x.muted= true;
        x.autoplay = true;
        x.setAttribute("src",vdoURL);

        //x.setAttribute("width", "80");
        //x.setAttribute("height", "80");
        //x.setAttribute("controls", "controls");

        if( parent ) {
            parent.el.appendChild(x);
        }
        else {
            var body = document.getElementsByTagName("body")[0];
            body.appendChild(x);
        }
        x.play();

        return { 
            el: x,
            canvas: null,
            ctx: null,
            _parent: body,
            width: 0,
            height: 0,
            app: this,
            id: function( _id ) { this.id = _id; return this; },
            style: function( name, value ) { this.el.style[name] = value; return this; },
            attribute: function( name, value ) { this.el.setAttribute( name, value ); return this; },
            parent: function( id ) { 
                if( this._parent ) {
                    this._parent.removeChild( this.el );
                }
                this._parent = document.getElementById(id);
                this._parent.appendChild(this.el);
                return this;
            },
            position: function( x, y ) {
                this.el.style['position'] = 'absolute';
                this.el.style['left'] = '' + x + 'px';
                this.el.style['top'] = '' + y + 'px';
            },
            loadPixels() {
                this.ctx.clearRect(0,0,this.width,this.height);
                this.ctx.drawImage(x,0,0,this.width,this.height);
                return this.ctx.getImageData(0, 0, this.width, this.height).data;
            },
            get( x, y ) {
                if( this.el.videoWidth == 0 || this.el.videoHeight == 0 )
                    return null;
                if( !this.canvas ) {
                    this.width = this.el.videoWidth;
                    this.height = this.el.videoHeight;

                    this.canvas = document.createElement("canvas");
                    this.canvas.width = this.el.videoWidth;
                    this.canvas.height = this.el.videoHeight;
                    //this.canvas.style.display = 'none';
                    var body = document.getElementsByTagName("body")[0];
                    body.appendChild(this.canvas);
                    this.ctx = this.canvas.getContext('2d');
                }
                var data = this.loadPixels();
                let i = this.width * 4 * y + (x * 4);
                let r = data[ i ];
                let g = data[ i+1 ];
                let b = data[ i+2 ];
                let a = data[ i+3 ];
                return this.app.color( r, g, b, a );
            }
           
        };        
    }

}

