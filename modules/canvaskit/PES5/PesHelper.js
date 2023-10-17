// const punycode = require('punycode');

// SQLJS
SQLJS = (function(){
    // var db;
    // var isOpened;

    function SQLJS(){
        this.db = null;
        this.isOpened = false;
    }

    SQLJS.prototype.openPPEF = function(data){
        if(this.isOpened){
            this.db.close();
            this.isOpened = false;
        }

        this.db = new window.SQL.Database(data);
        // console.log("db opened");
        this.isOpened = true;
    }

    SQLJS.prototype.closePPEF = function(){
        // console.log("close db");
        this.db.close();
        this.isOpened = false;
    }

    var config = function(){
        var LetterSpace = 0;
        var LineHeight = 764;
        var SpaceGap = 150;
        var LowerGap = 25;
        var BaseGap = 120;
        var UpperGap = 16;
        var UpperVovelGap = 145;
        var FishTail = 65;
        var ElephantTail = 25;
        var OwlTail = 25;
        var KiteY = 50;
    }

    SQLJS.prototype.readPPEFConfig = function(){
        var stmt = this.db.prepare("SELECT name, value FROM Config");
        while(stmt.step()){
            var results = stmt.get();
            // console.log(results);

            var name = results[0];
            var value = results[1];

            if(name=="BaseGap")
                config.BaseGap = value;
            else if(name=="ElephantTail")
                config.ElephantTail = value;
            else if(name=="FishTail")
                config.FishTail = value;
            else if(name=="KiteY")
                config.KiteY = value;
            else if(name=="LetterSpace")
                config.LetterSpace = value;
            else if(name=="LineHeight")
                config.LineHeight = value;
            else if(name=="LowerGap")
                config.LowerGap = value;
            else if(name=="OwlTail")
                config.OwlTail = value;
            else if(name=="SpaceGap")
                config.SpaceGap = value;
            else if(name=="UpperGap")
                config.UpperGap = value;
            else if(name=="UpperVovelGap")
                config.UpperVovelGap = value;

            // console.log( "readPPEFConfig: " + "row (" + name +", " + value );
        }
        stmt.reset();
    }

    SQLJS.prototype.getStringAsShapes = function(utf8, extraLetterSpace, extraSpace){
        var shapes = CanvasKit.pesFactory.MakePesData().paths;

        // console.log("getStringAsShapes" + utf8);
        var string = new String(utf8); // string = utf8;
        let n = string.length;

        var bThaiAlphabet = false;
		for(let i=0; i<n; i++){
            let unicode = string.codePointAt(i);
			if(th_isThaiLetter(unicode)){
				bThaiAlphabet = true;
				break;
			}
        }

        var uniString = [];
        var prev = -1;
        for(let i=0; i<n; i++){
            let unicode = string.codePointAt(i);
            if(bThaiAlphabet){
                // If input utf8-string contain Thai aphabet.
                if(unicode==THAI_SARA_AM) // Decompose THAI_SARA_AM to THAI_NIKHAHIT + THAI_SARA_AA
                {
                    if(prev!=-1 && th_isToneAndDiacritic(prev)){
                        uniString[uniString.length-1] = THAI_NIKHAHIT;
                        uniString.push(prev);
                        uniString.push(THAI_SARA_AA);
                    }
                    else{
                        uniString.push(THAI_NIKHAHIT);
                        uniString.push(THAI_SARA_AA);
                    }
                }
                else
                {
                    if(prev!=-1){
                        if(th_isBelowVowelAndDiacritic(unicode)){ // Substitue THAI_THO_THAN & THAI_YO_YING when followed by Lower vowel
                            if(prev==THAI_THO_THAN)
                                uniString[uniString.length-1]  = THAI_THO_THAN_NONE_TAIL;
                            else if(prev==THAI_YO_YING)
                                uniString[uniString.length-1]  = THAI_YO_YING_NONE_TAIL;
                        }
                    }
                    uniString.push(unicode);
                }
                prev = unicode;
            }
            else{
                uniString.push(unicode);
            }
        }

        const baseline = 150;
        let xstart = 0;
        let ystart = -baseline;
        let num = uniString.length;

        var ch = uniString[0];
        var lastch = ch;
        var lastLastCh = 0;

        var stmt = this.db.prepare("SELECT data FROM Data WHERE unicode = ?");

        for(let i=0;i<num;i++){
            var xmin = 0;
            var advance = 0;
            var xoffset = 0;
            var yoffset = 0;
            ch = uniString[i];
            var bExist = false;

            if(stmt.bind([ch])){
                var path = CanvasKit.pesFactory.MakePath();
                if( stmt.step()){
                    // pesPath::Command data structure
                    var vec3f = [
                        'x', 'float32',
                        'y', 'float32',
                        'z', 'float32'
                    ];
                    var def = [
                        'type', 'uint32',
                        'to', vec3f,
                        'cp1', vec3f,
                        'cp2', vec3f,
                        'radiusX', 'float32',
                        'radiusY', 'float32',
                        'angleBegin', 'float32',
                        'angleEnd', 'float32'
                    ];
                    var results = stmt.get();
                    var ds = new DataStream(results[0]);
                    // console.log("datastream-byteLength: "+ds.byteLength);
                    ds.endianness = DataStream.LITTLE_ENDIAN;
                    ds.seek(0);
                    var xmin = ds.readFloat32();    // glyph xmin
                    var advance = ds.readFloat32(); // glyph advance
                    var offset = ds.readInt32();    // offset to data
                    // console.log("data-offset: " + offset);

                    const moveTo = 0;
                    const lineTo = 1;
                    const curveTo = 2;
                    const bezierTo = 3;
                    const quadBezierTo = 4;
                    const arc = 5;
                    const arcNegative = 6;
                    const close = 7;

                    while(!ds.isEof()){
                        var cmd = ds.readStruct(def);
                        if(cmd.type==moveTo){
                            path.moveTo(cmd.to.x, cmd.to.y);
                        }
                        else if(cmd.type==lineTo){
                            path.lineTo(cmd.to.x, cmd.to.y);
                        }
                        else if(cmd.type==bezierTo){
                            var to = CanvasKit.pesFactory.MakeVec2f();
                            var cp1 = CanvasKit.pesFactory.MakeVec2f();
                            var cp2 = CanvasKit.pesFactory.MakeVec2f();
                            to.x = cmd.to.x; to.y = cmd.to.y;
                            cp1.x = cmd.cp1.x; cp1.y = cmd.cp1.y;
                            cp2.x = cmd.cp2.x; cp2.y = cmd.cp2.y;
                            path.bezierTo(cp1, cp2, to);
                        }
                        else if(cmd.type==close){
                            path.close();
                        }
                    }
                    bExist = true;
                }

                if(bExist){
                    var extSpace = (ch == 0x20) ? extraSpace : extraLetterSpace;
                    if(bThaiAlphabet){
                        if( th_isToneAndDiacritic(ch) ) { // ญี่ปุ่น
                            if( th_isBelowVowel(lastch)) {
                                lastch = lastLastCh;
                                lastLastCh = 0;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                            if(th_isAscenderConsonant(lastch)){ // (ป, ฝ, ฟ)
                                xoffset -= config.FishTail;
                            }
                            else if(lastch==THAI_CHO_CHANG || lastch==THAI_SO_SO || // (ช ซ ศ ส)
                                    lastch==THAI_SO_SALA || lastch==THAI_SO_SUA)
                            {
                                xoffset -= config.ElephantTail;
                            }
                            else if(lastch==THAI_LO_CHULA){ // ฬ
                                yoffset -= config.KiteY;
                            }
                            else if(lastch==THAI_HO_NOKHUK){ // ฮ
                                xoffset -= config.OwlTail;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(lastch)){
                            if(th_isToneAndDiacritic(ch)){
                                if(th_isAscenderConsonant(lastLastCh)){ // (ป, ฝ, ฟ)
                                    xoffset -= config.FishTail;
                                }
                                else if(lastLastCh==THAI_CHO_CHANG || lastLastCh==THAI_SO_SO || // (ช ซ ศ ส)
                                        lastLastCh==THAI_SO_SALA || lastLastCh==THAI_SO_SUA)
                                {
                                    xoffset -= config.ElephantTail;
                                }
                                else if(lastLastCh==THAI_LO_CHULA){ // ฬ
                                    yoffset -= config.KiteY;
                                }
                                else if(lastLastCh==THAI_HO_NOKHUK){ // ฮ
                                    xoffset -= config.OwlTail;
                                }
                                yoffset -= config.UpperVovelGap;
                            }
                        }

                        if(th_isBelowVowelAndDiacritic(ch)){
                            if(lastch==THAI_RU || lastch==THAI_LU){
                                xoffset -= config.FishTail;
                            }
                            else if(lastch==THAI_DO_CHADA || lastch==THAI_TO_PATAK){
                                yoffset += config.BaseGap;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(ch) || th_isBelowVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                            xoffset -= (config.LetterSpace + extSpace);
                        }
                    } // end if(bThaiAlphabet

                    var tx = (xstart - xmin + xoffset);
                    var ty = (ystart + yoffset);
                    path.translate(tx, ty);

                    xstart += advance;
                    var bZeroAdvance = (advance==0);
                    if(bZeroAdvance==false){
                        xstart += (config.LetterSpace + extSpace);
                    }
                    
                    // if(shape.getCommands().size()){
                        path.setFilled(false);
                        path.setStrokeWidth(2);
                        path.setStrokeHexColor(0x0000ff);
                        shapes.push_back(path);
                    // }
                }// end if(bExist)

                if(i>0){
                    lastLastCh = lastch;
                    lastch = ch;
                }
            }
        } // end for

        return shapes;
    }

    return SQLJS;
})();


// PesSatinColumn(Tool)
PesSatinColumnTool = (function(){
    var csp = []; //[CanvasKit.pesFactory.MakeCSP(), CanvasKit.pesFactory.MakeCSP()];

    function PesSatinColumnTool(){
        csp[0] = CanvasKit.pesFactory.MakeCSP();
        csp[1] = CanvasKit.pesFactory.MakeCSP();
    }

    PesSatinColumnTool.prototype.reset = function(){
        csp[0].reset();
        csp[1].reset();
    }

    PesSatinColumnTool.prototype.addCornerPoint = function(x, y){
        let idx = (csp[0].knots.size() + csp[1].knots.size())%2;
        csp[idx].addCornerPoint(x, y);
        csp[idx].calculateCSP();
        // console.log('add corner csp#' + idx + ' at:' + x + ',' +y);
    }

    PesSatinColumnTool.prototype.addCurvePoint = function(x, y){
        let idx = (csp[0].knots.size() + csp[1].knots.size())%2;
        csp[idx].addCurvePoint(x, y);
        csp[idx].calculateCSP();
    }

    PesSatinColumnTool.prototype.draw = function(){
        // backup render state
        var sc = js5app.strokeColor;
        var fc = js5app.fillColor;
        var nf = js5app.isNoFill;
        var ns = js5app.isNoStroke;

        //-- draw path
        var sk0 = CanvasKit.pesUtility.PesPathToSkPath(csp[0].path);
        var sk1 = CanvasKit.pesUtility.PesPathToSkPath(csp[1].path);

        js5app.stroke(100, 148, 237);
        drawPath(sk0);
        js5app.stroke(255, 165, 0);
        drawPath(sk1);
        sk0.delete();
        sk1.delete();

        var num = Math.min(csp[0].knots.size(), csp[1].knots.size());
        for(let i=0; i<num; i++){
            var p0 = csp[0].knots.get(i).p;
            var p1 = csp[1].knots.get(i).p;
            js5app.stroke(0, 255, 0);
            line(p0.x, p0.y, p1.x, p1.y);
        }
        //--- end draw path

        //--- draw knots
        var num = Math.max(csp[0].knots.size(), csp[1].knots.size());
        for(let i=0; i<num; i++){
            function drawKnot(knot){
                var p = knot.p;
                const s = 10;
                const r = s/2;
                if(knot.type==CanvasKit.PesKnotType.KNOT_TYPE_CORNER){
                    js5app.stroke(0, 0, 0);
                    js5app.fill(255, 255, 0);
                    rect(p.x, p.y, s, s);
                }
                else{
                    js5app.stroke(0, 0, 0);
                    js5app.fill(100, 148, 237);
                    circle(p.x, p.y, r);
                }
            };
            if(i<csp[0].knots.size()) drawKnot(csp[0].knots.get(i));
            if(i<csp[1].knots.size()) drawKnot(csp[1].knots.get(i));
        }
        //--- end draw knots

        // restore render state
        js5app.strokeColor = sc;
        js5app.fillColor = fc;
        js5app.isNoFill = nf;
        js5app.isNoStroke = ns;
    }

    PesSatinColumnTool.prototype.getCSP = function(){
        return csp;
    }

    return PesSatinColumnTool;
})();


// PesDrawingTool
PesDrawingTool = (function(){
    var ppath;

    function PesDrawingTool(){
        ppath = CanvasKit.pesFactory.MakePath();
    }

    PesDrawingTool.prototype.reset = function(){
        ppath.clear();
    }

    PesDrawingTool.prototype.draw = function(){
        // backup render state
        var sc = js5app.strokeColor;
        var fc = js5app.fillColor;
        var nf = js5app.isNoFill;
        var ns = js5app.isNoStroke;

        
        //-- draw path
        var spath = CanvasKit.pesUtility.PesPathToSkPath(ppath);
        js5app.stroke(0, 0, 0);
        drawPath(spath);
        if(spath.countPoints()>3){
            // stroke();
            strokeWeight(2.0);
            var p = spath.getPoint(0);
            var p1 = spath.getPoint(spath.countPoints()-1);
            var r = 10;
            var vFirst = CanvasKit.pesFactory.MakeVec2f();
            var vLast = CanvasKit.pesFactory.MakeVec2f();
            vFirst.set(p[0], p[1]);
            vLast.set(p1[0], p1[1]);
            var l = vFirst.distance(vLast);
            if(l<=r){
                fill(0, 0, 255);
                circle(p[0], p[1], r);
                // console.log('close');
            }
            else{
                // console.log('no-close');
            }
            noFill();
            stroke(0,0,0);
            circle(p[0], p[1], r);
        }
        spath.delete();
        //--- end draw path

        // restore render state
        js5app.strokeColor = sc;
        js5app.fillColor = fc;
        js5app.isNoFill = nf;
        js5app.isNoStroke = ns;
    }

    PesDrawingTool.prototype.addPoint = function(x, y){
        ppath.lineTo(x, y);
    }

    PesDrawingTool.prototype.getPesPath = function(){
        ppath.simplify(2.0);
        return ppath;
    }

    return PesDrawingTool;
})();

// PesResourceProvider
PesResourceProvider = (function(){

    function PesResourceProvider(){
        this.resourceCache = [];
    }

    PesResourceProvider.prototype.loadResource = function(name, url, onLoaded){
        if(this.resourceCache[name]){
            onLoaded(this.resourceCache[name]);
        }
        else{
            fetch(url)
            .then((response) => response.arrayBuffer())
            .then((buffer) => {
                this.resourceCache[name] = buffer;
                onLoaded(this.resourceCache[name]);
            });
        }
    }

    return PesResourceProvider;
})();


// StitchSimulator
StitchSimulator = (function(){
    var stitchPerMin = 6000;
    var stepPerTick = stitchPerMin / 60.0 / 60.0;
    var currentStitchNum = 0;
    var bStop = true;
    var bPause = true;

    function StitchSimulator(){
    }

    StitchSimulator.prototype.update = function(){
        if(bStop==false && bPause==false){
            currentStitchNum+=stepPerTick;
        }
    }

    StitchSimulator.prototype.isRunning = function(){
        return (currentStitchNum>0);
    }

    StitchSimulator.prototype.start = function(){
        currentStitchNum = 0;
        bStop = false;
        bPause = false;
    }

    StitchSimulator.prototype.speedup = function(){
        stitchPerMin+=1000;
        calcStep();
    }

    StitchSimulator.prototype.slowdown = function(){
        stitchPerMin-=1000;
        calcStep();
    }

    StitchSimulator.prototype.stop = function(){
        currentStitchNum = 0;
        bStop = true;
        bPause = true;
    }

    StitchSimulator.prototype.pause = function(){
        bPause = true;
    }

    StitchSimulator.prototype.resume = function(){
        bPause = false;
    }

    StitchSimulator.prototype.calcStep = function(){
        stitchPerMin = clamp(stitchPerMin, 400.0, 11400.0);
        stepPerTick = stitchPerMin / 60.0 / 60.0;
    }

    return StitchSimulator;
})();


// PesNodeCanvas
PesNodeCanvas = (function(){
    var pesDoc;
    var pesImages = [];
    var pesImageDatas=[];
    var selectedIndex = -1;
    var selectedLocalBound;
    var selectedCorner;
    var pressedX, pressedY;
    var previousX, previousY;
    var actionDesc = {
        action: undefined,
        // origin: {
        //       x: 'left',
        //       y: 'top'
        //   },
        tx: 0,
        ty: 0,
        dw: 0,
        dh: 0,
        degree: 0,
      };

      // coordinate policy: same of embroidery and sewing machine
      // 0,0 is origin point locate at canvas's center position
      // where x+ direction to right side and y+ is down to bottom screen
      var canvasOrigin;

      // zoom policy: 25% - 1000%
      var zoomLevel;
      const maxZoomIn = 10;     // 1000%
      const minZoomOut = 0.25;  // 25%

      // pan policy: 4800 x 4800 (48cm x 48cm)
      var panValue;
      const panAllow = {
        minX:   -2400,
        minY:   -2400,
        maxX:   2400,
        maxX:   2400,
    };

      var canvasID;

    const registerdTools = ['selectTool', 'handTool', 'satinColumnTool', 'drawingTool']; 
    var currentActiveTool = undefined;
    var satinColumnTool;
    var drawingTool;

    var isShiftPressed = false;
    var simulator;

    function PesNodeCanvas(_pesDoc, _canvasID=js5app.js5.html5_canvas.id){
        pesDoc = _pesDoc;
        canvasID = _canvasID;
        canvasOrigin = CanvasKit.pesFactory.MakeVec2f();
        canvasOrigin.x = width/2;
        canvasOrigin.y = height/2;
        zoomLevel = 1.0;
        currentActiveTool = 'selectTool';

        panValue = CanvasKit.pesFactory.MakeVec2f();
        panValue.set(0,0);

        satinColumnTool = new PesSatinColumnTool();
        this.setSelectedIndex(-1);

        drawingTool = new PesDrawingTool();
        this.setSelectedIndex(-1);

        simulator = new StitchSimulator();
    }

    PesNodeCanvas.prototype.getSimulator = function(){
        return simulator;
    }

    PesNodeCanvas.prototype.setOrigin = function(x, y){
        canvasOrigin.x = x;
        canvasOrigin.y = y;
    }

    PesNodeCanvas.prototype.setZoomLevel = function(z){
        zoomLevel = clamp(z, minZoomOut, maxZoomIn);
    }

    PesNodeCanvas.prototype.requestTool = function(reqTool){
        if(reqTool && reqTool!=currentActiveTool){
            // Todo: reset for currentActiveTool
            function resetActionDesc(){
                actionDesc.action = undefined;
                actionDesc.tx = actionDesc.ty = actionDesc.dw = actionDesc.dh = actionDesc.degree = 0;
            }

            currentActiveTool = reqTool;
            if(currentActiveTool=='handTool'){
                selectedIndex = -1;
                resetActionDesc();
                previousX = previousY = undefined;
                actionDesc.action = 'handTool-start';
            }
            else if(currentActiveTool=='satinColumnTool'){
                selectedIndex = -1;
                resetActionDesc();
                satinColumnTool.reset();
            }
            else if(currentActiveTool=='drawingTool'){
                selectedIndex = -1;
                resetActionDesc();
                drawingTool.reset();
            }
            return [currentActiveTool, true];
        }
        return [currentActiveTool, false];
    }

    PesNodeCanvas.prototype.getPesImage = function(idx){
        return pesImages[idx];
    }

    PesNodeCanvas.prototype.getPesImageData = function(idx){
        return pesImageDatas[idx];
    }

    PesNodeCanvas.prototype.swabPesImageAndData = function(idx0, idx1){
        img0 = pesImages[idx0];
        data0= pesImageDatas[idx0];

        pesImages[idx0] = pesImages[idx1];
        pesImageDatas[idx0] = pesImageDatas[idx1];

        pesImages[idx1] = img0;
        pesImageDatas[idx1] = data0;
    }

    PesNodeCanvas.prototype.insertPesImageAndData = function(idx, newImg, newData){
        pesImages.splice(idx, 0, newImg);
        pesImageDatas.splice(idx, 0, newData);
    }

    PesNodeCanvas.prototype.removePesImageAndData = function(idx){
        var imgToDelete = pesImages.splice(idx, 1);
        pesImageDatas.splice(idx, 1);
        if(imgToDelete.length==1 && imgToDelete[0]){
            imgToDelete[0].delete();
        }
    }

    PesNodeCanvas.prototype.clearAllImages = function(){
        for (let i=0; i<pesImages.length; i++) {
            if(pesImages[i]){
                pesImages[i].delete();
            }
        }
        pesImages = [];
        pesImageDatas = [];
    }

    PesNodeCanvas.prototype.loadPPES = function(buff){
        return pesDoc.loadPPESFromBuffer(buff);
    }

    PesNodeCanvas.prototype.reIndexLayerListImageItems = function(){
        var imgs = document.querySelectorAll('#layer-list-group img');
        for(let i=0; i<imgs.length; i++){
            imgs[i].src = pesImageDatas[i];
        }
    }

    PesNodeCanvas.prototype.setSelectedIndex = function(selIndex){
        // if(selectedIndex!=selIndex || selIndex==-1){
          selectedIndex = selIndex;
          var items = document.querySelectorAll('#layer-list-group li');
          for(let i=0; i<items.length; i++){
              if(i==selectedIndex){
                  items[i].classList.add('active');
              }
              else{
                  items[i].classList.remove('active');
              }
          }

          var canMoveUp = selectedIndex > 0;
          var canMoveDown = selectedIndex < (pesDoc.getObjectCount()-1);
          var canMerge = (selectedIndex>=0 && selectedIndex<pesDoc.getObjectCount()-1) && 
                        canMergeObject(selectedIndex, selectedIndex+1);
          var canDuplicate = selectedIndex>=0 && selectedIndex<pesDoc.getObjectCount();
          var canDelete = canDuplicate;

          var upElm = document.getElementById('level-up-layer-button');
          if(upElm){
            if(canMoveUp) upElm.classList.remove('disabled');
            else upElm.classList.add('disabled');
          }
          var downElm = document.getElementById('level-down-layer-button');
          if(downElm){
            if(canMoveDown) downElm.classList.remove('disabled');
            else downElm.classList.add('disabled');
          }
          var mergeElm = document.getElementById('merge-layer-button');
          if(mergeElm){
            if(canMerge) mergeElm.classList.remove('disabled');
            else mergeElm.classList.add('disabled');
          }
          var dupElm = document.getElementById('duplicate-layer-button');
          if(dupElm){
            if(canDuplicate) dupElm.classList.remove('disabled');
            else dupElm.classList.add('disabled');
          }
          var delElm = document.getElementById('delete-layer-button');
          if(delElm){
            if(canDelete) delElm.classList.remove('disabled');
            else delElm.classList.add('disabled');
          }
        // }

        if(selectedIndex==-1){
            updateDataToPanel(null);
        }else{
            updateDataToPanel(pesDoc.getDataObject(selectedIndex));
        }
    }

    PesNodeCanvas.prototype._updateImageCache = function(idx){
        var pes = CanvasKit.pesDocument.getInstance().getDataObjects().get(idx);
        if(pes){
            if(pesImages[idx]){
                pesImages[idx].delete();
            }

            if(pes.isScalable()){
                if(pes.parameter.isPPEFPath){
                    pes.applyPPEFFill();
                    pesImages[idx] = pesDoc.makePesImageSnapshot(idx);
                }
                else if(pes.parameter.isSatinColumnPath){
                    pes.applyFill();
                    pesImages[idx] = pesDoc.makePesImageSnapshot(idx);
                }
                else{
                    pes.applyFill();
                    pes.applyStroke();
                    if(pes.getTotalStitches()){
                        pesImages[idx] = pesDoc.makePesImageSnapshot(idx);
                    }
                    else{
                        pesImages[idx] = pesDoc.makePesScalableImageSnapshot(idx);
                    }
                }
            }
            else{
                pesImages[idx] = pesDoc.makePesImageSnapshot(idx);
            }

            if(pesImages[idx]){
                var imgBytes = pesImages[idx].encodeToBytes(CanvasKit.ImageFormat.PNG, 0.92);
                //var imgBytes = CanvasKit.getDataBytes(skimg);
                pesImageDatas[idx] =  'data:' + 'image/png' + ';base64,' + js5app.js5.toBase64String(imgBytes);
            }
            pes.delete();

            // updateLayerList();
        }
    }

    PesNodeCanvas.prototype.updateImageCache = function(){
        const cnt = pesDoc.getObjectCount();
        for(let i=0; i<cnt; i++){
            this._updateImageCache(i);
        }
    }

    PesNodeCanvas.prototype.drawNodes = function(){
        push();
        {
            const rs = 10;
            // translate(canvasOrigin.x, canvasOrigin.y);
            translate(canvasOrigin.x+panValue.x, canvasOrigin.y+panValue.y);
            scale(zoomLevel, zoomLevel);

            var hoopSize = CanvasKit.pesDocument.getInstance().getHoopSizeInMM();
            var hw = hoopSize.x * 10;
            var hh = hoopSize.y * 10;
            

            noStroke();
            fill(222);
            rect(0, 0, hw, hh);
            stroke(200);
            strokeWeight(1);
            // for(let x=-500; x<=500; x+=50){
            //     line(x, -500, x, 500);
            // }
            // for(let y=-500; y<=500; y+=50){
            //     line(-500, y, 500, y);
            // }
            for(let x=-hw/2; x<=hw/2; x+=50){
                line(x, -hh/2, x, hh/2);
            }
            for(let y=-hh/2; y<=hh/2; y+=50){
                line(-hw/2, y, hw/2, y);
            }

            stroke(160);
            strokeWeight(2);
            line(0, -3600, 0, 3600);
            line(-3600, 0, 3600, 0);
            stroke(0, 0, 0, 255);
            fill(255, 255, 255, 255);
            noFill();

            const cnt = pesDoc.getObjectCount();
            simulator.update();
            if(simulator.isRunning()){
                // console.log('isRunning');
                var num = simulator.currentStitchNum;
                for(let i=0; i<cnt; i++){
                    var bound = pesDoc.getDataObjects().get(i).getBoundingBox();
                    pesDoc.getDataObjects().get(i);
                    var paint = new CanvasKit.Paint();
                    // paint.setStyle();
                    push();
                    translate(bound.getCenter().x, bound.getCenter().y);
                    var rotateDegree = pesDoc.getDataObjects().get(i).parameter.rotateDegree;
                    if(rotateDegree!=0.0){
                        rotate(radians(rotateDegree));
                    }
                    // draw line here
                    var pesdata = pesDoc.getDataObjects().get(i);
                    var blocks = pesdata.getStitchBlockList();
                    // // console.log(blocks);
                    // for(let j=0; j<blocks.size(); j++){
                    //     var block = blocks[j];
                    //     console.log(block);
                    //     var c = block.getColor();
                    //     // paint.setColor(block.color.r, block.color.g, block.color.b);
                    //     // var vertices = block.polyline.getVertices();
                    //     // console.log(vertices.size());
                    // }
                    for(const block in blocks){
                        var c = block.getColor();
                        console.log(c);
                        // paint.setColor(block.color.r, block.color.g, block.color.b);
                        // var vertices = block.polyline.getVertices();
                    }

                    pop();
                }
            }
            else{
                for(let i=0; i<cnt; i++){
                    var bound = pesDoc.getDataObjects().get(i).getBoundingBox();
                    if(pesImages[i]){
                        let srcBound = CanvasKit.LTRBRect(0, 0, bound.width, bound.height);
                        let destBound = CanvasKit.LTRBRect( -bound.width/2, -bound.height/2, bound.width/2, bound.height/2);
                        var paint = new CanvasKit.Paint();
                        if(i==selectedIndex && actionDesc.action=='drag'){
                            push();
                            translate(bound.getCenter().x, bound.getCenter().y);
                            translate(actionDesc.tx, actionDesc.ty);
                            var rotateDegree = pesDoc.getDataObjects().get(i).parameter.rotateDegree;
                            if(rotateDegree!=0.0){
                            rotate(radians(rotateDegree));
                            }
                            js5app.js5.canvas.drawImageRect(pesImages[i], srcBound, destBound, paint, true );
                            pop();
                        }
                        else if(i==selectedIndex && actionDesc.action=='rotate'){
                            push();
                            translate(bound.getCenter().x, bound.getCenter().y);
                            if(actionDesc.action=='rotate'){
                            rotate(radians(actionDesc.degree));
                            }
                            js5app.js5.canvas.drawImageRect(pesImages[i], srcBound, destBound, paint, true );
                            pop();
                        }
                        else{
                            push();
                            translate(bound.getCenter().x, bound.getCenter().y);
                            var rotateDegree = pesDoc.getDataObjects().get(i).parameter.rotateDegree;
                            if(rotateDegree!=0.0){
                                rotate(radians(rotateDegree));
                            }
                            js5app.js5.canvas.drawImageRect(pesImages[i], srcBound, destBound, paint, true );
                            pop();
                        }
                        paint.delete();
                    }
                    if(i==selectedIndex){
                        function drawBorder(){
                            push();
                            {
                                if(actionDesc.action=='scale' || actionDesc.action=='scaleX' || actionDesc.action=='scaleY'){
                                    bound.width+=actionDesc.dw;
                                    bound.height+=actionDesc.dh;
                                    translate(-actionDesc.dw/2, -actionDesc.dh/2);
                                }
                                translate(bound.getCenter().x, bound.getCenter().y);
                                if(actionDesc.action=='drag'){
                                    translate(actionDesc.tx, actionDesc.ty);
                                }
                                stroke(100, 100, 255, 255);
                                noFill();
                                if(actionDesc.action=='rotate'){
                                    rotate(radians(actionDesc.degree));
                                }
                                else{
                                    var rotateDegree = pesDoc.getDataObjects().get(i).parameter.rotateDegree;
                                    if(rotateDegree!=0.0){
                                    rotate(radians(rotateDegree));
                                    }
                                }
                                noFill();

                                // if(actionDesc.action=='scale' || actionDesc.action=='scaleX' || actionDesc.action=='scaleY'){
                                //     translate(actionDesc.dw/2, actionDesc.dh/2);
                                // }

                                rect(0, 0, bound.width, bound.height);
                                fill(100, 100, 255, 255);

                                if(pesDoc.getDataObjects().get(i).isScalable()){
                                    // rect(-bound.width/2, -bound.height/2, rs, rs);
                                    // rect(0, -bound.height/2, rs, rs);
                                    // rect(bound.width/2, -bound.height/2, rs, rs);

                                    // rect(-bound.width/2, 0, rs, rs);
                                    rect(bound.width/2, 0, rs, rs);

                                    // rect(-bound.width/2, bound.height/2, rs, rs);
                                    rect(0, bound.height/2, rs, rs);
                                    rect(bound.width/2, bound.height/2, rs, rs);
                                }

                                line(0, -bound.height/2, 0, -bound.height/2-30);
                                rect(0, -bound.height/2-30, rs, rs);
                            }
                            pop();
                        }
                        drawBorder();
                    }
                }
            }

            if(currentActiveTool=='satinColumnTool'){
                satinColumnTool.draw();
            }
            else if(currentActiveTool=='drawingTool'){
                drawingTool.draw();
            }
            
        }

        // // test
        function drawStar(path) {
            let R = 115.2, C = 128.0;
            path.moveTo(C + R + 22, C);
            for (let i = 1; i < 8; i++) {
                let a = 2.6927937 * i;
                path.lineTo(C + R * Math.cos(a) + 22, C + R * Math.sin(a));
            }
            path.close();
            return path;
        }

        function drawTest(path){
            let C = 0;
            path.moveTo(0, 0); path.lineTo(100, 0); path.lineTo(100, 10); path.lineTo(0, 10); path.close();
            path.moveTo(0, 10); path.lineTo(100, 10); path.lineTo(100, 20); path.lineTo(0, 20); path.close();
            path.moveTo(0, 20); path.lineTo(100, 20); path.lineTo(100, 30); path.lineTo(0, 30); path.close();


            path.moveTo(100, 100); path.lineTo(0, 100); path.lineTo(0, 90); path.lineTo(100, 90); path.close();
            path.moveTo(100, 90); path.lineTo(0, 90); path.lineTo(0, 80); path.lineTo(100, 80); path.close();

            path.moveTo(0, 80); path.lineTo(100, 80); path.lineTo(100, 70); path.lineTo(0, 70); path.close();

            path.moveTo(0, 110); path.lineTo(100, 110); path.lineTo(100, 120); path.lineTo(0, 120); path.close();
            path.moveTo(0, 120); path.lineTo(100, 120); path.lineTo(100, 130); path.lineTo(0, 130); path.close();
            
            // path.lineTo(100, 10); 

            // path.lineTo(100, 20); path.lineTo(0, 10); 

            // path.lineTo(0, 20); path.lineTo(100, 20);

            //path.lineTo(100, 30); path.lineTo(0, 30);

            return path;
        }

        // let path = new CanvasKit.SkPath();
        // drawTest(path);
        // path.setIsVolatile(true);
        // path.simplify();
        // // drawPath(path);
        // var pesPath = CanvasKit.pesUtility.SKPathToPesPath(path);
        // path = CanvasKit.pesUtility.PesPathToSkPath(pesPath);
        // drawPath(path);

        // path.delete();
        //
        pop();

        // var items = document.querySelectorAll('#layer-list-group img');
        // for(let i=0; i<items.length; i++){
        //     items[i].src = pesImageDatas[i];
        // }
    }

    function rotatePoint(point, angle, pivot){
        var a = radians(angle);
        var xrot = ((point.x-pivot.x)*cos(a) - (point.y-pivot.y)*sin(a)) + pivot.x;
        point.y = ((point.x-pivot.x)*sin(a) + (point.y-pivot.y)*cos(a)) + pivot.y;
        point.x = xrot;
        return point;
    }

    // Returns coordinates of a mouse-pointer relative to an pes object
    function getLocalPointer(pes, canvasOrigin){
        var pointer = CanvasKit.pesFactory.MakeVec2f();
        pointer.set(mouseX, mouseY);
        // pointer.x = pointer.x - canvasOrigin.x;
        // pointer.y = pointer.y - canvasOrigin.y;
        pointer.x = pointer.x - (canvasOrigin.x + panValue.x);
        pointer.y = pointer.y - (canvasOrigin.y + panValue.y);
        pointer.x/=zoomLevel;
        pointer.y/=zoomLevel;
    
        const angle = pes.parameter.rotateDegree;
        if(Math.abs(angle)>Number.EPSILON){
            var pivot = pes.getBoundingBox().getCenter();
            pointer = rotatePoint(pointer, -angle, pivot);
        }
        return pointer;
    }

    function toLocalPoint(wx, wy, pes=null){
        var point = CanvasKit.pesFactory.MakeVec2f();
        point.set(wx, wy);
        point.x = point.x - (canvasOrigin.x + panValue.x);
        point.y = point.y - (canvasOrigin.y + panValue.y);
        point.x/=zoomLevel;
        point.y/=zoomLevel;

        if(pes){
            const angle = pes.parameter.rotateDegree;
            if(Math.abs(angle)>Number.EPSILON){
                var pivot = pes.getBoundingBox().getCenter();
                point = rotatePoint(point, -angle, pivot);
            }
        }
        return point;
    }

    canMergeObject = function(idx0, idx1){
        var type0 = pesDoc.getDataObject(idx0).parameter.type;
        var type1 = pesDoc.getDataObject(idx1).parameter.type;
        if(type0==type1){
            if( type0==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SATINCOLUMN ||
                type0==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE ||
                type0==CanvasKit.PesObjectType.OBJECT_TYPE_PES){
                    return true;
                }
        }
        return false;
    }

    PesNodeCanvas.prototype.mousePressed = function() {
        const cnt = pesDoc.getObjectCount();
        const cx = width/2, cy = height/2;
        var canvasOrigin = CanvasKit.pesFactory.MakeVec2f();
        canvasOrigin.set(cx, cy);

        if(currentActiveTool=='handTool'){
            pressedX = mouseX;
            pressedY = mouseY;
            if(previousX===undefined || previousY===undefined){
                previousX = mouseX;
                previousY = mouseY;
            }
            actionDesc.action = 'handTool-down';
            setCursorByID(canvasID, 'hand');
            return;
        }

        if(currentActiveTool=='satinColumnTool'){
            var lp = toLocalPoint(mouseX, mouseY);
            if(isShiftPressed){
                satinColumnTool.addCurvePoint(lp.x, lp.y);
            }
            else{
                satinColumnTool.addCornerPoint(lp.x, lp.y);
            }
            return;
        }

        if(currentActiveTool!='selectTool'){
            return;
        }
      
        if(selectedIndex==-1){
          for(let i=cnt-1; i>=0; i--){
            var p = getLocalPointer(pesDoc.getDataObjects().get(i), canvasOrigin);
            var localBound = pesDoc.getDataObjects().get(i).getBoundingBox();
            if(localBound.inside(p.x, p.y)){
            //   selectedIndex = i;
              this.setSelectedIndex(i);
              selectedLocalBound = localBound.copy();
              selectedCorner = 'global';
              pressedX = mouseX;
              pressedY = mouseY;
              setCursorByID(canvasID, 'move');
            //   updateDataToDialog(pesDoc.getDataObjects().get(selectedIndex));
              break;
            }
          }
          if(selectedIndex==-1){ // deselect
            this.setSelectedIndex(-1);
            // updateDataToDialog(null);
          }
        }
        else{
          var corner = undefined;
          var p = getLocalPointer(pesDoc.getDataObjects().get(selectedIndex), canvasOrigin);
          var localBound = pesDoc.getDataObjects().get(selectedIndex).getBoundingBox();
          const rs = 10;
          function checkCorners(){
            var r = CanvasKit.pesFactory.MakeRect();
            r.setFromCenter(localBound.x+localBound.width, localBound.getCenter().y, rs, rs);
            if(r.inside(p.x, p.y)){
              selectedLocalBound = localBound.copy();
            //   localPressedX = p.x;
            //   localPressedY = p.y;
              return corner = 'mr';
            }
      
            r.setFromCenter(localBound.getCenter().x, localBound.y+localBound.height, rs, rs);
            if(r.inside(p.x, p.y)){
              selectedLocalBound = localBound.copy();
            //   localPressedX = p.x;
            //   localPressedY = p.y;
              return corner = 'mb';
            }
      
            r.setFromCenter(localBound.x+localBound.width, localBound.y+localBound.height, rs, rs);
            if(r.inside(p.x, p.y)){
              selectedLocalBound = localBound.copy();
            //   localPressedX = p.x;
            //   localPressedY = p.y;
              return corner = 'br';
            }
      
            r.setFromCenter(localBound.getCenter().x, localBound.y-30, rs, rs);
            if(r.inside(p.x, p.y)){
              selectedLocalBound = localBound.copy();
              setCursorByID(canvasID, 'crosshair');
              return corner = 'mtr';
            }
      
            if(localBound.inside(p.x, p.y)){
              setCursorByID(canvasID, 'move');
              return corner = 'global';
            }
      
            return corner;
          }
          corner = checkCorners();
      
          if(corner===undefined){
            selectedIndex = -1;
            for(let i=cnt-1; i>=0; i--){
              const localBound = pesDoc.getDataObjects().get(i).getBoundingBox();
              var p = getLocalPointer(pesDoc.getDataObjects().get(i), canvasOrigin);
              if(localBound.inside(p.x, p.y)){
                //   selectedIndex = i;
                  this.setSelectedIndex(i);
                  selectedLocalBound = localBound.copy();
                  selectedCorner = 'global';
                  pressedX = mouseX;
                  pressedY = mouseY;
                //   updateDataToDialog(pesDoc.getDataObjects().get(selectedIndex));
                  break;
                }
            }
            if(selectedIndex==-1){ // deselect
                this.setSelectedIndex(-1);
                // updateDataToDialog(null);
              }
          }
          else{
            pressedX = mouseX;
            pressedY = mouseY;
      
            var action = getActionFromCorner(corner);
            if(action=='scale' || action=='scaleX' || action=='scaleY'){
              var cursor = getRotatedCornerCursor(corner, pesDoc.getDataObjects().get(selectedIndex).parameter.rotateDegree);
              setCursorByID(canvasID, cursor);
            }
          }
      
          selectedCorner = corner;
        }
      }

      PesNodeCanvas.prototype.mouseMoved = function() {
        if(currentActiveTool==='selectTool'){
            const cnt = pesDoc.getObjectCount();
            if(selectedIndex==-1){
                // for(let i=cnt-1; i>=0; i--){
                //   var p = getLocalPointer(pesDoc.getDataObjects().get(i), canvasOrigin);
                //   var localBound = pesDoc.getDataObjects().get(i).getBoundingBox();
                //   if(localBound.inside(p.x, p.y)){
                //     selectedIndex = i;
                //     selectedLocalBound = localBound.copy();
                //     selectedCorner = 'global';
                //     setCursorByID(canvasID, 'move');
                //     break;
                //   }
                // }
              }
              else{
                var corner = undefined;
                var p = getLocalPointer(pesDoc.getDataObjects().get(selectedIndex), canvasOrigin);
                var localBound = pesDoc.getDataObjects().get(selectedIndex).getBoundingBox();
                const rs = 10;
                function checkCorners(){
                  var r = CanvasKit.pesFactory.MakeRect();
                  r.setFromCenter(localBound.x+localBound.width, localBound.getCenter().y, rs, rs);
                  if(r.inside(p.x, p.y)){
                    selectedLocalBound = localBound.copy();
                    return corner = 'mr';
                  }
            
                  r.setFromCenter(localBound.getCenter().x, localBound.y+localBound.height, rs, rs);
                  if(r.inside(p.x, p.y)){
                    selectedLocalBound = localBound.copy();
                    return corner = 'mb';
                  }
            
                  r.setFromCenter(localBound.x+localBound.width, localBound.y+localBound.height, rs, rs);
                  if(r.inside(p.x, p.y)){
                    selectedLocalBound = localBound.copy();
                    return corner = 'br';
                  }
            
                  r.setFromCenter(localBound.getCenter().x, localBound.y-30, rs, rs);
                  if(r.inside(p.x, p.y)){
                    selectedLocalBound = localBound.copy();
                    setCursorByID(canvasID, 'crosshair');
                    return corner = 'mtr';
                  }
            
                  if(localBound.inside(p.x, p.y)){
                    setCursorByID(canvasID, 'move');
                    return corner = 'global';
                  }
            
                  return corner;
                }
                corner = checkCorners();
            
                if(corner===undefined){
                    setCursorByID(canvasID, 'default');
                }
                else{
                  var action = getActionFromCorner(corner);
                  if(action=='scale' || action=='scaleX' || action=='scaleY'){
                    var cursor = getRotatedCornerCursor(corner, pesDoc.getDataObjects().get(selectedIndex).parameter.rotateDegree);
                    setCursorByID(canvasID, cursor);
                  }
                }
            
                selectedCorner = corner;
              }
        }
      }


      PesNodeCanvas.prototype.mouseDragged = function() {
        if(currentActiveTool=='handTool' && (actionDesc.action == 'handTool-down' || actionDesc.action == 'handTool-drag')){
            actionDesc.action = 'handTool-drag';
            // actionDesc.tx = (mouseX - pressedX) / zoomLevel;
            // actionDesc.ty = (mouseY - pressedY) / zoomLevel;
            if(previousX!==undefined && previousY!==undefined){
                var dx = (mouseX - previousX);
                var dy = (mouseY - previousY);
                // console.log("dx, dy= " + dx + " " + dy);
                if((Math.abs(dx) + Math.abs(dy)) > Number.EPSILON){
                    panValue.x += dx;
                    panValue.y += dy;
                    panValue.x = clamp(panValue.x, panAllow.minX, panAllow.maxX);
                    panValue.y = clamp(panValue.y, panAllow.minY, panAllow.maxY);

                    // console.log("panValue: " + panValue.x + " " + panValue.y);
                }
                else{
                    // console.log("lt EPSILON " + (Math.abs(dx) + Math.abs(dy)) + " > " + Number.EPSILON);
                }
            }
            
            previousX = mouseX;
            previousY = mouseY;
            return;
        }
        else if(currentActiveTool=='drawingTool'){
            var lp = toLocalPoint(mouseX, mouseY);
            drawingTool.addPoint(lp.x, lp.y);
        }

        if(selectedCorner==undefined && selectedIndex!=-1){
          selectedCorner = 'global';
        }
      
        if(selectedCorner!=undefined){
          var action = getActionFromCorner(selectedCorner);
          // console.log(action);
          actionDesc.action = action;
          if(action=='drag'){
            actionDesc.tx = (mouseX - pressedX) / zoomLevel;
            actionDesc.ty = (mouseY - pressedY) / zoomLevel;
            setCursorByID(canvasID, 'move');
          }
          else if(action=='rotate'){      
            var localBound = selectedLocalBound.copy();
            var center = localBound.getCenter();
            const cx = width/2, cy = height/2;
            var canvasOrigin = CanvasKit.pesFactory.MakeVec2f();
            canvasOrigin.set(cx, cy);
            var p = getLocalPointer(pesDoc.getDataObjects().get(selectedIndex), canvasOrigin);
            var dir = CanvasKit.pesFactory.MakeVec2f();
            dir.set(p.x-center.x, p.y-center.y);
            dir.normalize();
            var up = CanvasKit.pesFactory.MakeVec2f();
            up.set(0, -1);
            var rotateDegree = pesDoc.getDataObjects().get(selectedIndex).parameter.rotateDegree;
            var angle = up.angle(dir);
            actionDesc.degree = wrap(rotateDegree + angle, -180, 180);
          }
          else{
            // scale
            action.origin = getOriginFromCorner(selectedCorner);
            // var dw = (mouseX - pressedX) / zoomLevel;
            // var dh = (mouseY - pressedY) / zoomLevel;
            var pes = pesDoc.getDataObjects().get(selectedIndex);
            var mouse = toLocalPoint(mouseX, mouseY, pes);
            var pressed = toLocalPoint(pressedX, pressedY, pes);
            var dw = mouse.x - pressed.x;
            var dh = mouse.y - pressed.y;

            if(actionDesc.action=='scaleX'){
              actionDesc.dw = dw;
              actionDesc.dh = 0;
            }
            else if(actionDesc.action=='scaleY'){
              actionDesc.dw = 0;
              actionDesc.dh = dh;
            }
            else if(actionDesc.action=='scale'){
            //   var dmin = Math.min(dw, dh);
            //   actionDesc.dw = dmin;
            //   actionDesc.dh = (selectedLocalBound.height/selectedLocalBound.width)*dmin;
                var d = Math.sqrt(dw*dw + dh*dh);
                actionDesc.dw = d * Math.sign(dw);
                actionDesc.dh = (selectedLocalBound.height/selectedLocalBound.width) * actionDesc.dw;
            }
          }
        }
        else{
          actionDesc.action = undefined;
        }
      }
      
      PesNodeCanvas.prototype.mouseReleased = function() {
        if(currentActiveTool=='handTool'){
            actionDesc.action = 'handTool-release';
            actionDesc.tx = 0;
            actionDesc.ty = 0;
            previousX=undefined;
            previousY=undefined;
            return;
        }
        else if(currentActiveTool=='drawingTool'){
            var path = drawingTool.getPesPath();
            path.setStrokeWidth(2.0);
            path.setStrokeColor(CanvasKit.pesUtility.GetBrotherColor(1));

            var spath = CanvasKit.pesUtility.PesPathToSkPath(path);
            var p = spath.getPoint(0);
            var p1 = spath.getPoint(spath.countPoints()-1);
            var r = 10;
            var vFirst = CanvasKit.pesFactory.MakeVec2f();
            var vLast = CanvasKit.pesFactory.MakeVec2f();
            vFirst.set(p[0], p[1]);
            vLast.set(p1[0], p1[1]);
            var l = vFirst.distance(vLast);
            if(l<=r){
                path.close();
                path.setFillColor(CanvasKit.pesUtility.GetBrotherColor(11));
                var param = CanvasKit.pesFactory.MakeParam();
                param.setType(CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE);
                param.fillType = CanvasKit.PesFillType.FILL_TYPE_PATTERN;
                param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
            }
            else{
                var param = CanvasKit.pesFactory.MakeParam();
                param.setType(CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE);
                param.fillType = CanvasKit.PesFillType.FILL_TYPE_NONE;
                param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_SATIN;
            }

            var paths = CanvasKit.pesFactory.MakePesData().paths;
            paths.push_back(path);

            var pes = CanvasKit.pesFactory.MakePesData();
            pes.strokeWidth = 2.0;
            pes.parameter = param;
            pes.paths = paths;
            var cnt = CanvasKit.pesDocument.getInstance().addObject(pes);
            const lastIndex = cnt-1;
            this._updateImageCache(lastIndex);
            this.setSelectedIndex(lastIndex);
            var selElm = document.getElementById('selectTool');
            if(selElm){
                selElm.click();
            }
            return;
        }


        if(selectedCorner!=undefined){
          var action = getActionFromCorner(selectedCorner);
          if(action=='drag'){
            var tx = (mouseX - pressedX)/zoomLevel;
            var ty = (mouseY - pressedY)/zoomLevel;
            pesDoc.getDataObjects().get(selectedIndex).translate(tx, ty);
            pesDoc.getDataObjects().get(selectedIndex).recalculate();
          }
          else if(action=='rotate'){
            var localBound = selectedLocalBound.copy();
            var center = localBound.getCenter();
            const cx = width/2, cy = height/2;
            var canvasOrigin = CanvasKit.pesFactory.MakeVec2f();
            canvasOrigin.set(cx, cy);
            var p = getLocalPointer(pesDoc.getDataObjects().get(selectedIndex), canvasOrigin);
            var dir = CanvasKit.pesFactory.MakeVec2f();
            dir.set(p.x-center.x, p.y-center.y);
            dir.normalize();
            var up = CanvasKit.pesFactory.MakeVec2f();
            up.set(0, -1);
            var rotateDegree = pesDoc.getDataObjects().get(selectedIndex).parameter.rotateDegree;
            var angle = up.angle(dir);
            const degreeTolerance = 2.0;
            if(Math.abs(angle)>degreeTolerance){
              actionDesc.degree = wrap(rotateDegree + angle, -180, 180);
              var param = pesDoc.getDataObjects().get(selectedIndex).parameter.copy();
              param.rotateDegree = actionDesc.degree;
              pesDoc.getDataObjects().get(selectedIndex).parameter = param;
            }
            actionDesc.degree = 0;
          }
          else{
            // scale
            // var dw = (mouseX - pressedX)/zoomLevel;
            // var dh = (mouseY - pressedY)/zoomLevel;
            var pes = pesDoc.getDataObjects().get(selectedIndex);
            var mouse = toLocalPoint(mouseX, mouseY, pes);
            var pressed = toLocalPoint(pressedX, pressedY, pes);
            var dw = mouse.x - pressed.x;
            var dh = mouse.y - pressed.y;
      
            if(action=='scaleX'){
              var sx = (selectedLocalBound.width+dw) / selectedLocalBound.width;
              pesDoc.getDataObjects().get(selectedIndex).scale(sx, 1);
              pesDoc.getDataObjects().get(selectedIndex).translate(-dw/2, 0);   
            }
            else if(action=='scaleY'){
              var sy = (selectedLocalBound.height+dh) / selectedLocalBound.height;
              pesDoc.getDataObjects().get(selectedIndex).scale(1, sy);
              pesDoc.getDataObjects().get(selectedIndex).translate(0, -dh/2);
            }
            else if(action=='scale'){
              var dmin = Math.min(dw, dh);
              actionDesc.dw = dmin;
              actionDesc.dh = (selectedLocalBound.height/selectedLocalBound.width)*dmin;
              var sx = (selectedLocalBound.width+actionDesc.dw) / selectedLocalBound.width;
              var sy = (selectedLocalBound.height+actionDesc.dh) / selectedLocalBound.height;
              pesDoc.getDataObjects().get(selectedIndex).scale(sx, sy);
              pesDoc.getDataObjects().get(selectedIndex).translate(-actionDesc.dw/2, -actionDesc.dh/2);
            }

            this._updateImageCache(selectedIndex);
          }
        }
        actionDesc.action = undefined;
        setCursorByID(canvasID, 'default');
      }

      PesNodeCanvas.prototype.keyPressed = function(){
        // console.log('key='+key + ' keycode:'+keyCode);
        if(key==='Shift'){
            isShiftPressed = true;
        }

        if(currentActiveTool=='satinColumnTool'){
            if(key==='Enter'){
                var csp = satinColumnTool.getCSP();
                if(csp[0].knots.size() && csp[0].knots.size()==csp[1].knots.size()){
                    // create new pesDataObject type scalable
                    var param = CanvasKit.pesFactory.MakeParam();
                    // param.type = CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SATINCOLUMN;
                    // param.isSatinColumnPath = true;
                    param.setType(CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SATINCOLUMN);
                    param.isSatinColumnPath = true;
                    param.fillType = CanvasKit.PesFillType.FILL_TYPE_SATIN_COLUMN;
                    param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
                    param.fillColorIndex = 11; // Depp gold

                    var paths = CanvasKit.pesFactory.MakePesData().paths;
                    paths.push_back(csp[0].path);
                    paths.push_back(csp[1].path);
                    // console.log("enter0:"+paths.size());

                    var pes = CanvasKit.pesFactory.MakePesData();
                    pes.parameter = param;
                    pes.paths = paths;
                    var cnt = CanvasKit.pesDocument.getInstance().addObject(pes);
                    const lastIndex = cnt-1;
                    this._updateImageCache(lastIndex);
                    this.setSelectedIndex(lastIndex);
                }
                var selElm = document.getElementById('selectTool');
                if(selElm){
                    selElm.click();
                }
            }
        }
        // else if(currentActiveTool=='satinColumnTool'){
        //     if(key==='Enter'){
        //         var ppath = drawingTool.getPesPath();
        //     }
        // }
      }

      PesNodeCanvas.prototype.keyReleased = function(){
        if(key==='Shift'){
            isShiftPressed = false;
        }
      }

      PesNodeCanvas.prototype.zoomIn = function(){
        var levels = [0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4, 5, 7.5, 10];
        for(let i=0; i<levels.length; i++){
            if(zoomLevel<levels[i]){
                this.setZoomLevel(levels[i]);
                return [zoomLevel, i];
            }
        }
        return undefined;
      }

      PesNodeCanvas.prototype.zoomOut = function(){
        var levels = [0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4, 5, 7.5, 10];
        for(let i=levels.length-1; i>=0; i--){
            if(zoomLevel>levels[i]){
                this.setZoomLevel(levels[i]);
                return [zoomLevel, i];
            }
        }
        return undefined;
      }

      PesNodeCanvas.prototype.getSelectedIndex = function(){
        return selectedIndex;
      }

      PesNodeCanvas.prototype.deSelectObject = function(){
        this.setSelectedIndex(-1);
      }

      PesNodeCanvas.prototype.objectTypeToString = function(type){
        var CP = CanvasKit.PesObjectType;
        if(type==CP.OBJECT_TYPE_PES2_TEXT || type==CP.OBJECT_TYPE_PES || type==CP.OBJECT_TYPE_SHAPE){
            return "Stitch";
        }
        else if(type==CP.OBJECT_TYPE_BACKGROUND){
            return "Background";
        }
        else if(type==CP.OBJECT_TYPE_SCALABLE_SVG_FILE){
            return "SVG";
        }
        else if(type==CP.OBJECT_TYPE_SCALABLE_PPEF_TEXT){
            return "PPEF Text";
        }
        else if(type==CP.OBJECT_TYPE_SCALABLE_TTF_TEXT){
            return "TTF Text";
        }
        else if(type==CP.OBJECT_TYPE_SCALABLE_SATINCOLUMN){
            return "Satin Column";
        }
        return "Unknow";
      }

      function getOriginFromCorner(corner) {
            var origin = {
                x: 'left',
                y: 'top'
            }
    
          if (corner === 'ml' || corner === 'tl' || corner === 'bl') {
            origin.x = 'right';
          }
          else if (corner === 'mr' || corner === 'tr' || corner === 'br') {
            origin.x = 'left';
          }
    
          if (corner === 'tl' || corner === 'mt' || corner === 'tr') {
            origin.y = 'bottom';
          }
          else if (corner === 'bl' || corner === 'mb' || corner === 'br') {
            origin.y = 'top';
          }
    
          return origin;
        }
  
    function getActionFromCorner(corner) {
        if(corner=='global'){
        return 'drag';
        }
        switch(corner){
        case 'mtr':
            return 'rotate';
        case 'ml':
        case 'mr':
          return 'scaleX';
        case 'mt':
        case 'mb':
          return 'scaleY';
        case 'tl':
        case 'tr':
        case 'bl':
        case 'br':
          return 'scale';
        default:
          return undefined;  
        }
    }
  
    var cursorOffset = {
            mt: 0, // n
            tr: 1, // ne
            mr: 2, // e
            br: 3, // se
            mb: 4, // s
            bl: 5, // sw
            ml: 6, // w
            tl: 7 // nw
          };
    
    
    function getRotatedCornerCursor(corner, rotateDegree){
        var cursorMap = ['n-resize',
              'ne-resize',
              'e-resize',
              'se-resize',
              's-resize',
              'sw-resize',
              'w-resize',
              'nw-resize'];
    
        var n = Math.round((rotateDegree % 360) / 45);
        if (n < 0) {
            n += 8; // full circle ahead
        }
      n += cursorOffset[corner];
        n %= 8;
        return cursorMap[n];
    }
    
    function setCursorByID(id,cursorStyle) {
        var elem;
        if (document.getElementById &&
            (elem=document.getElementById(id)) ) {

            if (elem.style) {
                elem.style.cursor=cursorStyle;
            }
        }
    }



    return PesNodeCanvas;
})();

// legacy sql class work w/ WebWorker
// Worker sql.js
SQLWebWorker = (function(){
    var worker = undefined
    var db = undefined;
    var ppef_opened = false;

    function SQLWebWorker(pathToWasm){
        if(worker==undefined){
            this.worker = new Worker(pathToWasm);
        }
    }

    SQLWebWorker.prototype.openPPEF = function(data){
        if(ppef_opened){
            this.db.close();
            ppef_opened = false;
        }

        this.db = new window.SQL.Database(data);
        // console.log("db opened");
        ppef_opened = true;
    }

    SQLWebWorker.prototype.closePPEF = function(){
        // console.log("close db");
        this.db.close();
        ppef_opened = false;
    }

    SQLWebWorker.prototype.execute = function(commands) {
        tic();
        this.worker.onmessage = function (event) {
            var results = event.data.results;
            toc("Executing SQL");
            tic();
            toc("Displaying results");
        }
        this.worker.postMessage({ action: 'exec', sql: commands });
    }

    var config = function(){
        var LetterSpace = 0;
        var LineHeight = 764;
        var SpaceGap = 150;
        var LowerGap = 25;
        var BaseGap = 120;
        var UpperGap = 16;
        var UpperVovelGap = 145;
        var FishTail = 65;
        var ElephantTail = 25;
        var OwlTail = 25;
        var KiteY = 50;
    }

    SQLWebWorker.prototype.readPPEFConfig = function(){
        var stmt = this.db.prepare("SELECT name, value FROM Config");
        while(stmt.step()){
            var results = stmt.get();
            // console.log(results);

            var name = results[0];
            var value = results[1];

            if(name=="BaseGap")
                config.BaseGap = value;
            else if(name=="ElephantTail")
                config.ElephantTail = value;
            else if(name=="FishTail")
                config.FishTail = value;
            else if(name=="KiteY")
                config.KiteY = value;
            else if(name=="LetterSpace")
                config.LetterSpace = value;
            else if(name=="LineHeight")
                config.LineHeight = value;
            else if(name=="LowerGap")
                config.LowerGap = value;
            else if(name=="OwlTail")
                config.OwlTail = value;
            else if(name=="SpaceGap")
                config.SpaceGap = value;
            else if(name=="UpperGap")
                config.UpperGap = value;
            else if(name=="UpperVovelGap")
                config.UpperVovelGap = value;

            // console.log( "readPPEFConfig: " + "row (" + name +", " + value );
        }
        stmt.reset();
    }

    SQLWebWorker.prototype.getStringAsShapes = function(utf8, extraLetterSpace, extraSpace){
        var shapes = CanvasKit.pesFactory.MakePesData().paths;

        // console.log("getStringAsShapes" + utf8);
        var string = new String(utf8); // string = utf8;
        let n = string.length;

        var bThaiAlphabet = false;
		for(let i=0; i<n; i++){
            let unicode = string.codePointAt(i);
			if(th_isThaiLetter(unicode)){
				bThaiAlphabet = true;
				break;
			}
        }

        var uniString = [];
        var prev = -1;
        for(let i=0; i<n; i++){
            let unicode = string.codePointAt(i);
            if(bThaiAlphabet){
                // If input utf8-string contain Thai aphabet.
                if(unicode==THAI_SARA_AM) // Decompose THAI_SARA_AM to THAI_NIKHAHIT + THAI_SARA_AA
                {
                    if(prev!=-1 && th_isToneAndDiacritic(prev)){
                        uniString[uniString.length-1] = THAI_NIKHAHIT;
                        uniString.push(prev);
                        uniString.push(THAI_SARA_AA);
                    }
                    else{
                        uniString.push(THAI_NIKHAHIT);
                        uniString.push(THAI_SARA_AA);
                    }
                }
                else
                {
                    if(prev!=-1){
                        if(th_isBelowVowelAndDiacritic(unicode)){ // Substitue THAI_THO_THAN & THAI_YO_YING when followed by Lower vowel
                            if(prev==THAI_THO_THAN)
                                uniString[uniString.length-1]  = THAI_THO_THAN_NONE_TAIL;
                            else if(prev==THAI_YO_YING)
                                uniString[uniString.length-1]  = THAI_YO_YING_NONE_TAIL;
                        }
                    }
                    uniString.push(unicode);
                }
                prev = unicode;
            }
            else{
                uniString.push(unicode);
            }
        }

        const baseline = 150;
        let xstart = 0;
        let ystart = -baseline;
        let num = uniString.length;

        var ch = uniString[0];
        var lastch = ch;
        var lastLastCh = 0;

        var stmt = this.db.prepare("SELECT data FROM Data WHERE unicode = ?");

        for(let i=0;i<num;i++){
            var xmin = 0;
            var advance = 0;
            var xoffset = 0;
            var yoffset = 0;
            ch = uniString[i];
            var bExist = false;

            if(stmt.bind([ch])){
                var path = CanvasKit.pesFactory.MakePath();
                if( stmt.step()){
                    // pesPath::Command data structure
                    var vec3f = [
                        'x', 'float32',
                        'y', 'float32',
                        'z', 'float32'
                    ];
                    var def = [
                        'type', 'uint32',
                        'to', vec3f,
                        'cp1', vec3f,
                        'cp2', vec3f,
                        'radiusX', 'float32',
                        'radiusY', 'float32',
                        'angleBegin', 'float32',
                        'angleEnd', 'float32'
                    ];
                    var results = stmt.get();
                    var ds = new DataStream(results[0]);
                    // console.log("datastream-byteLength: "+ds.byteLength);
                    ds.endianness = DataStream.LITTLE_ENDIAN;
                    ds.seek(0);
                    var xmin = ds.readFloat32();    // glyph xmin
                    var advance = ds.readFloat32(); // glyph advance
                    var offset = ds.readInt32();    // offset to data
                    // console.log("data-offset: " + offset);

                    const moveTo = 0;
                    const lineTo = 1;
                    const curveTo = 2;
                    const bezierTo = 3;
                    const quadBezierTo = 4;
                    const arc = 5;
                    const arcNegative = 6;
                    const close = 7;

                    while(!ds.isEof()){
                        var cmd = ds.readStruct(def);
                        if(cmd.type==moveTo){
                            path.moveTo(cmd.to.x, cmd.to.y);
                        }
                        else if(cmd.type==lineTo){
                            path.lineTo(cmd.to.x, cmd.to.y);
                        }
                        else if(cmd.type==bezierTo){
                            var to = CanvasKit.pesFactory.MakeVec2f();
                            var cp1 = CanvasKit.pesFactory.MakeVec2f();
                            var cp2 = CanvasKit.pesFactory.MakeVec2f();
                            to.x = cmd.to.x; to.y = cmd.to.y;
                            cp1.x = cmd.cp1.x; cp1.y = cmd.cp1.y;
                            cp2.x = cmd.cp2.x; cp2.y = cmd.cp2.y;
                            path.bezierTo(cp1, cp2, to);
                        }
                        else if(cmd.type==close){
                            path.close();
                        }
                    }
                    bExist = true;
                }

                if(bExist){
                    var extSpace = (ch == 0x20) ? extraSpace : extraLetterSpace;
                    if(bThaiAlphabet){
                        if( th_isToneAndDiacritic(ch) ) { // ญี่ปุ่น
                            if( th_isBelowVowel(lastch)) {
                                lastch = lastLastCh;
                                lastLastCh = 0;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                            if(th_isAscenderConsonant(lastch)){ // (ป, ฝ, ฟ)
                                xoffset -= config.FishTail;
                            }
                            else if(lastch==THAI_CHO_CHANG || lastch==THAI_SO_SO || // (ช ซ ศ ส)
                                    lastch==THAI_SO_SALA || lastch==THAI_SO_SUA)
                            {
                                xoffset -= config.ElephantTail;
                            }
                            else if(lastch==THAI_LO_CHULA){ // ฬ
                                yoffset -= config.KiteY;
                            }
                            else if(lastch==THAI_HO_NOKHUK){ // ฮ
                                xoffset -= config.OwlTail;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(lastch)){
                            if(th_isToneAndDiacritic(ch)){
                                if(th_isAscenderConsonant(lastLastCh)){ // (ป, ฝ, ฟ)
                                    xoffset -= config.FishTail;
                                }
                                else if(lastLastCh==THAI_CHO_CHANG || lastLastCh==THAI_SO_SO || // (ช ซ ศ ส)
                                        lastLastCh==THAI_SO_SALA || lastLastCh==THAI_SO_SUA)
                                {
                                    xoffset -= config.ElephantTail;
                                }
                                else if(lastLastCh==THAI_LO_CHULA){ // ฬ
                                    yoffset -= config.KiteY;
                                }
                                else if(lastLastCh==THAI_HO_NOKHUK){ // ฮ
                                    xoffset -= config.OwlTail;
                                }
                                yoffset -= config.UpperVovelGap;
                            }
                        }

                        if(th_isBelowVowelAndDiacritic(ch)){
                            if(lastch==THAI_RU || lastch==THAI_LU){
                                xoffset -= config.FishTail;
                            }
                            else if(lastch==THAI_DO_CHADA || lastch==THAI_TO_PATAK){
                                yoffset += config.BaseGap;
                            }
                        }

                        if(th_isAboveVowelAndDiacritic(ch) || th_isBelowVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                            xoffset -= (config.LetterSpace + extSpace);
                        }
                    } // end if(bThaiAlphabet

                    var tx = (xstart - xmin + xoffset);
                    var ty = (ystart + yoffset);
                    path.translate(tx, ty);

                    xstart += advance;
                    var bZeroAdvance = (advance==0);
                    if(bZeroAdvance==false){
                        xstart += (config.LetterSpace + extSpace);
                    }
                    
                    // if(shape.getCommands().size()){
                        path.setFilled(false);
                        path.setStrokeWidth(2);
                        path.setStrokeHexColor(0x0000ff);
                        shapes.push_back(path);
                    // }
                }// end if(bExist)

                if(i>0){
                    lastLastCh = lastch;
                    lastch = ch;
                }
            }
        } // end for

        return shapes;
    }

    return SQLWebWorker;
})();
