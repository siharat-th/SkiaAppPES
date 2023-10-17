var newDocElm = document.getElementById('menubar-new-button');
var openfileElm = document.getElementById('openfile');
var importfileElm = document.getElementById('importfile');
var importfileClassifyElm = document.getElementById('importfileClassify');
var savefileElm = document.getElementById('menubar-save-button');
var exportfileElm = document.getElementById('menubar-export-button');
var zoomSelectElm= document.getElementById('scale-select');
var zoomInElm= document.getElementById('menubar-zoomIn-button');
var zoomOutElm= document.getElementById('menubar-zoomOut-button');
var hoopSizeElm = document.getElementById('hoop-select');

// Tool
var selectToolElm = document.getElementById('selectTool');
var handToolElm = document.getElementById('handTool');
var shapeToolElm = document.getElementById('shapeTool');
var ttfFontToolElm = document.getElementById('ttfFontTool');
var ppefFontToolElm = document.getElementById('ppefFontTool');
var satinColumnToolElm = document.getElementById('satinColumnTool');
var drawingToolElm = document.getElementById('drawingTool');

// Properties Panel
var paramFillColorElm = document.getElementById('paramFillColor');
var applyButtonElm = document.getElementById('applyButton');
var strokeTypeElm = document.getElementById("paramStrokeType");
var fillTypeElm = document.getElementById("paramFillType");
var interCharDecElm = document.getElementById("inter-char-dec");
var interCharIncElm = document.getElementById("inter-char-inc");
var spaceDecElm = document.getElementById("space-dec");
var spaceIncElm = document.getElementById("space-inc");
var effectElm = document.getElementById("paramEffectType");

// Layer Toolbar
var moveUpElm = document.getElementById('level-up-layer-button');
var moveDownElm = document.getElementById('level-down-layer-button');
var mergeElm = document.getElementById('merge-layer-button');
var duplicateElm = document.getElementById('duplicate-layer-button');
var deleteElm = document.getElementById('delete-layer-button');

// Stitch Simulator
// var simStart = document.getElementById('menubar-simstart-button');
// var simPause = document.getElementById('menubar-simpause-button');
// var simStop = document.getElementById('menubar-simstop-button');

// Util function
function pad(n, width, z) {
  z = z || '0';
  n = n + '';
  return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
}

// Html element
insertHtmlOptionElements = function(){
  var ppefElm = document.getElementById('paramPPEFFont');
  ppefElm.innerHTML = `
    <option value=PPEF/Thai001.ppef selected>รัฐภูมิ</option>
    <option value=PPEF/Thai002.ppef>วราลี</option>
    <option value=PPEF/Thai003.ppef>ณิช</option>
    <option value=PPEF/Thai004.ppef>วิหค</option>
    <option value=PPEF/Thai005.ppef>ขุนเขา</option>
    <option value=PPEF/Thai006.ppef>อณิกา</option>
    <option value=PPEF/Thai007.ppef>คชินทร์</option>
    <option value=PPEF/Thai008.ppef>วงเดือน</option>
    <option value=PPEF/Thai009.ppef>ปุณณภา</option>
    <option value=PPEF/Thai010.ppef>คมกฤช</option>
    <option value=PPEF/Thai999.ppef>จุดดาว</option>
    <option value=PPEF/Thai011.ppef>ก้าวหน้า</option>
    <option value=PPEF/Thai012.ppef>ฌามส์</option>
    <option value=PPEF/Thai013.ppef>วัยวัฒน์</option>
    <option value=PPEF/Thai014.ppef>พัลลวรรณ</option>
    <option value=PPEF/Thai015.ppef>ชัญญา</option>
    <option value=PPEF/Thai016.ppef>ฟารัต</option>
    <option value=PPEF/Thai017.ppef>ลฉัตร</option>
    <option value=PPEF/Thai018.ppef>นาวา</option>
    <option value=PPEF/Thai019.ppef>การ์ตูน</option>
    <option value=PPEF/Thai020.ppef>ล้านนา</option>
    <option value=PPEF/Thai021.ppef>ลิขิต</option>
    <option value=PPEF/Thai022.ppef>ลายมือ</option>
    <option value=PPEF/Thai023.ppef>อักษรา</option>
    <option value=PPEF/Thai024.ppef>บัวบุญ</option>
    <option value=PPEF/Thai025.ppef>ชลิต</option>
    <option value=PPEF/Thai026.ppef>ชนก</option>
    <option value=PPEF/Thai027.ppef>ชอลิ้วเฮียง</option>
    <option value=PPEF/Thai028.ppef>ละอองดาว</option>
  `;

  var ttfElm = document.getElementById('paramTTFFont');
  ttfElm.innerHTML = `
    <option value='TTF/JS-Boaboon.ttf' selected>JS-Boaboon</option>
    <option value='TTF/JS-Chaimongkol.ttf'>JS-Chaimongkol</option>
    <option value='TTF/JS-Chalit.ttf'>JS-Chalit</option>
    <option value='TTF/JS-Chanok-Normal.ttf'>JS-Chanok-Normal</option>
    <option value='TTF/JS-Chawlewhieng.ttf'>JS-Chawlewhieng</option>
    <option value='TTF/JS-Chodok-Bold.ttf'>JS-Chodok-Bold</option>
    <option value='TTF/JS-Giat-Bold.ttf'>JS-Giat-Bold</option>
    <option value='TTF/JS-Kobori-Allcaps-Bold.ttf'>JS-Kobori-Allcaps-Bold</option>
    <option value='TTF/JS-Laongdao-Bold.ttf'>JS-Laongdao-Bold</option>
    <option value='TTF/JS-Neeno-Normal.ttf'>JS-Neeno-Normal</option>
    <option value='TTF/JS-Ninja-Allcaps-Bold.ttf'>JS-Ninja-Allcaps-Bold</option>
    <option value='TTF/JS-Noklae-Normal.ttf'>JS-Noklae-Normal</option>
    <option value='TTF/JS-Pranee-Italic.ttf'>JS-Pranee-Italic</option>
    <option value='TTF/JS-Sadayu-Bold.ttf'>JS-Sadayu-Bold</option>
    <option value='TTF/JS-Sirium-Normal.ttf'>JS-Sirium-Normal</option>
    <option value='TTF/JS-Thanaporn-Normal.ttf'>JS-Thanaporn-Normal</option>
    <option value='TTF/JS-Tina-Normal.ttf'>JS-Tina-Normal</option>
    <option value='TTF/JS-Toomtam-Normal.ttf'>JS-Toomtam-Normal</option>
    <option value='TTF/JS-Wanida-Italic.ttf'>JS-Wanida-Italic</option>
    <option value='TTF/JS-Wansika-Italic.ttf'>JS-Wansika-Italic</option>
  `;

  var fontsizeElm = document.getElementById('paramFontSize');
  fontsizeElm.innerHTML = `
    <option value=4>4</option>
    <option value=5>5</option>
    <option value=6>6</option>
    <option value=7>7</option>
    <option value=8 selected>8</option>
    <option value=9>9</option>
    <option value=10>10</option>
    <option value=11>11</option>
    <option value=12>12</option>
    <option value=13>13</option>
    <option value=14>14</option>
    <option value=15>15</option>
    <option value=16>16</option>
    <option value=17>17</option>
    <option value=18>18</option>
    <option value=20>20</option>
    <option value=22>22</option>
    <option value=24>24</option>
    <option value=26>26</option>
    <option value=28>28</option>
    <option value=30>30</option>
  `;
  
  var filltypeElm = document.getElementById('paramFillType');
  filltypeElm.innerHTML = `
    <option value=none selected>None</option>
    <option value=normal>Step</option>
    <option value=pattern>Pattern</option>
    <option value=motif>Motif</option>
    <!-- <option value=4>Satin Column</option> -->
    <!-- <option value=color>Color</option> -->
    <!-- <option value=6>Disable</option> -->
  `;

  var stroketypeElm = document.getElementById('paramStrokeType');
  stroketypeElm.innerHTML = `
    <option value=none selected>None</option>
    <option value=running>Running</option>
    <option value=double>Double Running</option>
    <option value=triple>Triple Running</option>
    <option value=bean>Bean</option>
    <option value=satin>Satin</option>
    <option value=satin-outset>Outset Satin</option>
    <option value=satin-inset>Inset Satin</option>
    <option value=motif>Motif</option>
  `;


  var effectTypeElm = document.getElementById('paramEffectType');
  effectTypeElm.innerHTML = `
    <option selected>Plain Text</option>
    <option>Arch Up(Curve)</option>
    <option>Arch Down(Curve)</option>
    <option>Circle(Curve)</option>
    <option>Wave</option>

    <option>Chevron Up</option>
    <option>Chevron Down</option>

    <option>Slant Up</option>
    <option>Slant Down</option>
    <option>Triangle Up</option>
    <option>Triangle Down</option>
    <option>Fade Right</option>
    <option>Fade Left</option>
    <option>Fade Up</option>
    <option>Fade Down</option>
    <option>Inflate</option>
  `;

  
  var jointypeElm = document.getElementById('paramJoinType');
  jointypeElm.innerHTML = `
  <option value=square selected>Square</option>
  <option value=round>Round</option>
  <option value=miter>Miter</option>
  `;
  
  // Inset motif list item elements.(Motif001 - Motif089)
  var motifListElm = document.getElementById('motif-list-group');
  for(let i=1; i<=89; i++){
    motifListElm.innerHTML += `<li class="list-group-item">Motif` +pad(i, 3) + `</li>`;
  }
  
  // Inset pattern list item
  const PAS = ['net1','net2','net3','net4',
    'pat01','pat02','pat03','pat04','pat05','pat06','pat07','pat08','pat09','pat10','pat11','pat12','pat13','pat14','pat15','pat16','pat17',
    'stamp1','stamp2','stamp3','stamp4','stamp5','stamp6','stamp7',
    'stampA','stampB','stampC','stampD','stampE','stampF','stampG',
    'tatami1','tatami2','tatami3','tatami4','tatami5',
    'wave1','wave2','wave3','wave4'];
  var patternListElm = document.getElementById('pattern-list-group');
  for(let i=0; i<PAS.length; i++){
    patternListElm.innerHTML += `<li class="list-group-item">` + PAS[i] + `</li>`;
  }

  // Insert shape list item
  const SHAPE = ['triangle','rectangle','diamond','hexagon','octagon','star','heart','circle','round-rectangle','ribbon','chevron'];
  var shapeListElm = document.getElementById('shape-list-group');
  for(let i=0; i<SHAPE.length; i++){
    shapeListElm.innerHTML += `<li class="list-group-item">` + SHAPE[i] + `</li>`;
  }

  handleMotifDialogEvents();
  handlePatternDialogEvents();
  handleShapeDialogEvents();
}

function handleMotifDialogEvents(){
  var selectedIndex = -1;
  var items = document.querySelectorAll("#motif-list-group li");
  onMotifSelectedIndex = function(idx){
    if(idx!=selectedIndex){
      if(selectedIndex!=-1){
        items[selectedIndex].classList.remove('active');
      }
      selectedIndex = idx;
      items[selectedIndex].classList.add('active');

      var motifListElm = document.getElementById('motif-list-group');
      if(motifListElm){
        motifListElm.selectedIndex = selectedIndex;
        motifListElm.value = items[selectedIndex].innerText;
      }

      var name = items[selectedIndex].innerText;
      var url = './assets/PMF/' + name + '.pmf';
      pesResourceProvider.loadResource(name, url, function(buffer){
          var uints = new Uint8Array(buffer);
          var pesBuff = CanvasKit.pesFactory.MakeBuffer();
          pesBuff.set(uints);
          var skPath = CanvasKit.pesUtility.LoadPMFFromBuffer(pesBuff);
          const surface = CanvasKit.MakeSWCanvasSurface('motif-modal-canvas');
          if (!surface) {
            console.error('Could not make surface');
          }
          else{
            const canvas = surface.getCanvas();
            let paint = new CanvasKit.Paint();

            const w = 1024;
            const h = 1024;
            var first = skPath.getPoint(0);
            var last = skPath.getPoint(skPath.countPoints()-1);
            
            canvas.save();
            {
              canvas.scale(0.25, 0.25);
              canvas.clear(CanvasKit.TRANSPARENT);
              paint.setColor(CanvasKit.BLUE);
              paint.setStyle(CanvasKit.PaintStyle.Stroke);
              canvas.drawPath(skPath, paint);

              paint.setColor(CanvasKit.Color(0, 255, 0, 255));
              canvas.drawLine(0, h/2, w, h/2, paint);
              paint.setStyle(CanvasKit.PaintStyle.Fill);
              paint.setColor(CanvasKit.BLUE);
              canvas.drawCircle(first[0], first[1], 4*4, paint);
              paint.setColor(CanvasKit.RED);
              canvas.drawCircle(last[0], last[1], 4*4, paint);
            }
            canvas.restore();
            surface.flush();

            paint.delete();
            surface.delete();
            skPath.delete();
          }
          pesBuff.clear();
          pesBuff.delete();
      });
    }
  }

  for(let i=0; i<items.length; i++){
    items[i].onclick = function(){
      onMotifSelectedIndex(i);
    };
  }
  if(items[0]){
    items[0].click();
  }
}

function handlePatternDialogEvents(){
  var selectedIndex = -1;
  var items = document.querySelectorAll("#pattern-list-group li");
  onPatternSelectedIndex = function(idx){
    if(idx!=selectedIndex){
      if(selectedIndex!=-1){
        items[selectedIndex].classList.remove('active');
      }
      selectedIndex = idx;
      items[selectedIndex].classList.add('active');

      var patternListElm = document.getElementById('pattern-list-group');
      if(patternListElm){
        patternListElm.selectedIndex = selectedIndex;
        patternListElm.value = items[selectedIndex].innerText;
      }

      var name = items[selectedIndex].innerText;
      var url = './assets/PAS/' + name + '.pas';
      pesResourceProvider.loadResource(name, url, function(buffer){
          var uints = new Uint8Array(buffer);
          var pesBuff = CanvasKit.pesFactory.MakeBuffer();
          pesBuff.set(uints);
          var skPath = CanvasKit.pesUtility.LoadPASFromBuffer(pesBuff);
          const surface = CanvasKit.MakeSWCanvasSurface('pattern-modal-canvas');
          if (!surface) {
            console.error('Could not make surface');
          }
          else{
            const canvas = surface.getCanvas();
            let paint = new CanvasKit.Paint();

            const w = 1024;
            const h = 1024;
            var first = skPath.getPoint(0);
            var last = skPath.getPoint(skPath.countPoints()-1);
            
            canvas.save();
            {
              canvas.scale(0.25, 0.25);
              canvas.clear(CanvasKit.TRANSPARENT);
              paint.setColor(CanvasKit.BLUE);
              paint.setStyle(CanvasKit.PaintStyle.Stroke);
              canvas.drawPath(skPath, paint);
            }
            canvas.restore();
            surface.flush();

            paint.delete();
            surface.delete();
            skPath.delete();
          }
          pesBuff.clear();
          pesBuff.delete();
      });
    }
  }

  for(let i=0; i<items.length; i++){
    items[i].onclick = function(){
      onPatternSelectedIndex(i);
    };
  }
  if(items[0]){
    items[0].click();
  }
}

function handleShapeDialogEvents(){
  var selectedIndex = -1;
  var items = document.querySelectorAll("#shape-list-group li");
  onShapeSelectedIndex = function(idx){
    if(idx!=selectedIndex){
      if(selectedIndex!=-1){
        items[selectedIndex].classList.remove('active');
      }
      selectedIndex = idx;
      items[selectedIndex].classList.add('active');

      var shapeListElm = document.getElementById('shape-list-group');
      if(shapeListElm){
        shapeListElm.selectedIndex = selectedIndex;
        shapeListElm.value = items[selectedIndex].innerText;
      }

      var name = 'SHAPE/' + items[selectedIndex].innerText;
      var url = './assets/' + name + '.svg';
      pesResourceProvider.loadResource(name, url, function(buffer){
          var uints = new Uint8Array(buffer);
          var pesBuff = CanvasKit.pesFactory.MakeBuffer();
          pesBuff.set(uints);
          // var skPath = CanvasKit.pesUtility.LoadPASFromBuffer(pesBuff);
          var pesdata = CanvasKit.pesFactory.MakePesData();
          pesdata.loadSVGFromBuffer(pesBuff, true);

          const surface = CanvasKit.MakeSWCanvasSurface('shape-modal-canvas');
          if (!surface) {
            console.error('Could not make surface');
          }
          else{
            const canvas = surface.getCanvas();
            let paint = new CanvasKit.Paint();
            
            canvas.save();
            {
              canvas.translate(128, 128);
              canvas.scale(0.25, 0.25);
              canvas.clear(CanvasKit.TRANSPARENT);
              // paint.setColor(CanvasKit.BLUE);
              // paint.setStyle(CanvasKit.PaintStyle.Stroke);
              // canvas.drawPath(skPath, paint);
              paint.setAntiAlias(true);
              for(let i=0; i<pesdata.paths.size(); i++){
                  var path = pesdata.paths.get(i);
                  var skpath = CanvasKit.pesUtility.PesPathToSkPath(path);
                  if(path.isFill()){
                      var c = path.getFillColor();
                      paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                      paint.setStyle(CanvasKit.PaintStyle.Fill);
                      canvas.drawPath(skpath, paint);
                  }
                  else{
                      // this.noFill();
                  }
                  if(path.isStroke()){
                      var c = path.getStrokeColor();
                      paint.setColor(CanvasKit.Color(c.r, c.g, c.b, c.a));
                      paint.setStrokeWidth(path.getStrokeWidth());
                      paint.setStyle(CanvasKit.PaintStyle.Stroke);
                      canvas.drawPath(skpath, paint);
                  }
                  else{
                      // this.noStroke();
                  }
                  skpath.delete();
              }
              // paint.delete();
            }
            canvas.restore();
            surface.flush();

            paint.delete();
            surface.delete();
          }
          pesBuff.clear();
          pesBuff.delete();
      });
    }
  }

  for(let i=0; i<items.length; i++){
    items[i].onclick = function(){
      onShapeSelectedIndex(i);
    };
  }
  if(items[0]){
    items[0].click();
  }
}

// update selected pesdata.parameter to Properties panel
function updateDataToPanel(pesdata){
  if(pesdata){
    document.getElementById("Properties").hidden = false;

    var bStitch = pesdata.parameter.type == CanvasKit.PesObjectType.OBJECT_TYPE_PES;
    document.getElementById("labelTotalStitches").hidden = !bStitch;
    document.getElementById("labelTotalJump").hidden = !bStitch;
    if(bStitch){
      document.getElementById("labelTotalStitches").innerText = 'Total stitches: ' + pesdata.getTotalStitches();
      document.getElementById("labelTotalJump").innerText = 'Total jump: ' + pesdata.getTotalJump();
    }

    var bText = pesdata.parameter.isTTFPath || pesdata.parameter.isPPEFPath;
    document.getElementById("paramText").hidden = !bText;
    document.getElementById("labelText").hidden = !bText;
    if(bText){
      document.getElementById("paramText").value = pesdata.parameter.text;
    }

    var bPPEF = pesdata.parameter.isPPEFPath;
    document.getElementById("paramPPEFFont").hidden = !bPPEF;
    document.getElementById("labelPPEFFont").hidden = !bPPEF;
    document.getElementById('ppef-adv-group').hidden = !bPPEF;
    if(bPPEF){
      var ppefElm = document.getElementById("paramPPEFFont");
      var selIndex = 0;
      for(let i=0; i<ppefElm.options.length; i++){
        if(pesdata.parameter.fontName==ppefElm.options[i].value){
          selIndex = i;
          break;
        }
      }
      ppefElm.selectedIndex = selIndex;

      document.getElementById("paramEffectType").selectedIndex = pesdata.parameter.shapeIndex;
      document.getElementById("paramItalic").checked = pesdata.parameter.italic;
      document.getElementById("paramBorder").checked = pesdata.parameter.border;
      // document.getElementById("paramFontWeight").value = pesdata.parameter.fontWeight;
    }

    var bSatinFill = pesdata.parameter.fillType==CanvasKit.PesFillType.FILL_TYPE_SATIN_COLUMN;
    document.getElementById('satin-fill-group').hidden = !bSatinFill;
    if(bSatinFill){
      document.getElementById('param-satin-fill-density').value = pesdata.parameter.fill.density;
      document.getElementById('param-pullCompensate').value = pesdata.parameter.pullCompensate;
    }

    var bTTF = pesdata.parameter.isTTFPath;
    document.getElementById("paramTTFFont").hidden = !bTTF;
    document.getElementById("labelTTFFont").hidden = !bTTF;
    if(bTTF){
      var ttfElm = document.getElementById("paramTTFFont");
      var selIndex = 0;
      for(let i=0; i<ttfElm.options.length; i++){
        if(pesdata.parameter.fontName==ttfElm.options[i].value){
          selIndex = i;
          break;
        }
      }
      ttfElm.selectedIndex = selIndex;
    }

    var bFontSize = bText;
    document.getElementById("paramFontSize").hidden = !bFontSize;
    document.getElementById("labelFontSize").hidden = !bFontSize;
    if(bFontSize){
      var elm = document.getElementById("labelFontSize");
      if(bPPEF){
        elm.innerText = 'Font size(mm):';
      }
      else if(bTTF){
        elm.innerText = 'Font size(pt):';
      }
      
      var fonsizeElm = document.getElementById("paramFontSize");
      var selIndex = 0;
      for(let i=0; i<fonsizeElm.options.length; i++){
        if(pesdata.parameter.fontSize==fonsizeElm.options[i].value){
          selIndex = i;
          break;
        }
      }
      fonsizeElm.selectedIndex = selIndex;
    }

    var bFillColor = pesdata.isScalable();
    document.getElementById("fillColor").hidden = !bFillColor;
    if(bFillColor){
      var c = CanvasKit.pesUtility.GetBrotherColor(pesdata.parameter.fillColorIndex);
      document.getElementById("paramFillColor").value = CanvasKit.pesUtility.PesColorToHtmlColor(c);
    }

    var bSVGFromfile = pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE;
    document.getElementById('autoColor').hidden = !bSVGFromfile;
    if(bSVGFromfile){
      document.getElementById('paramAutoColor').checked = !pesdata.parameter.useColorFromPicker;
    }

    var bScalable = (pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE || 
      pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_TTF_TEXT);
    document.getElementById("paramFillType").hidden = !bScalable;
    document.getElementById("paramStrokeType").hidden = !bScalable;
    document.getElementById("strokeColor").hidden = !bScalable;
    document.getElementById("paramJoinType").hidden = !bScalable;
    document.getElementById("labelFillType").hidden = !bScalable;
    document.getElementById("labelStrokeType").hidden = !bScalable;
    document.getElementById("labelJoinType").hidden = !bScalable;
    if(bScalable){
      // fill type
      var selIndex = -1;
      if(pesdata.parameter.fillType===CanvasKit.PesFillType.FILL_TYPE_NONE) selIndex = 0;
      else if(pesdata.parameter.fillType===CanvasKit.PesFillType.FILL_TYPE_NORMAL) selIndex = 1;
      else if(pesdata.parameter.fillType===CanvasKit.PesFillType.FILL_TYPE_PATTERN) selIndex = 2;
      else if(pesdata.parameter.fillType===CanvasKit.PesFillType.FILL_TYPE_MOTIF) selIndex = 3;
      else if(pesdata.parameter.fillType===CanvasKit.PesFillType.FILL_TYPE_COLOR) selIndex = 4;
      if(selIndex!=-1) document.getElementById("paramFillType").selectedIndex = selIndex;

      // stroke color
      var c = CanvasKit.pesUtility.GetBrotherColor(pesdata.parameter.colorIndex);
      document.getElementById("paramStrokeColor").value = CanvasKit.pesUtility.PesColorToHtmlColor(c);

      // stroke type
      selIndex = -1;
      if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_NONE) selIndex = 0;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING) selIndex = 1;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_DOUBLE) selIndex = 2;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_TRIPLE) selIndex = 3;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_BEAN) selIndex = 4;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_SATIN) selIndex = 5;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_OUTER) selIndex = 6;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_INNER) selIndex = 7;
      else if(pesdata.parameter.strokeType===CanvasKit.PesStrokeType.STROKE_TYPE_MOTIF) selIndex = 8;
      if(selIndex!=-1) document.getElementById("paramStrokeType").selectedIndex = selIndex;

      // join type
      selIndex = -1;
      if(pesdata.parameter.strokeJoinType===CanvasKit.PesJoinType.JOINT_TYPE_SQUARE) selIndex = 0;
      else if(pesdata.parameter.strokeJoinType===CanvasKit.PesJoinType.JOINT_TYPE_ROUND) selIndex = 1;
      else if(pesdata.parameter.strokeJoinType===CanvasKit.PesJoinType.JOINT_TYPE_MITER) selIndex = 2;
      if(selIndex!=-1) document.getElementById("paramJoinType").selectedIndex = selIndex;
    }

    document.getElementById('applyButton').hidden = !pesdata.isScalable();

    var stType = pesdata.parameter.strokeType;
    var bStrokeRunning = bScalable && (stType.value>=CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING.value && 
      stType.value<=CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_BEAN.value);
    document.getElementById('stroke-running-group').hidden = !bStrokeRunning;
    if(bStrokeRunning){
      document.getElementById('param-strokeRunPitch').value = pesdata.parameter.strokeRunPitch;
      document.getElementById('param-strokeRunningInset').value = pesdata.parameter.strokeRunningInset;
    }


    var bStrokeSatin = bScalable &&(stType.value>=CanvasKit.PesStrokeType.STROKE_TYPE_SATIN.value && 
      stType.value<=CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_INNER.value);
    document.getElementById('stroke-satin-group').hidden = !bStrokeSatin;
    if(bStrokeSatin){
      document.getElementById('param-strokeWidth').value = pesdata.parameter.strokeWidth;
      document.getElementById('param-strokeDensity').value = pesdata.parameter.strokeDensity;
    }

    var bStrokeMotif = bScalable && stType.value==CanvasKit.PesStrokeType.STROKE_TYPE_MOTIF.value;
    document.getElementById('stroke-motif-group').hidden = !bStrokeMotif;
    if(bStrokeMotif){
      document.getElementById('param-motif-size').value = pesdata.parameter.motif.size;
      document.getElementById('param-motif-spacing').value = pesdata.parameter.motif.spacing;
      document.getElementById('param-motif-runPitch').value = pesdata.parameter.motif.runPitch;
    }

    var fillType = pesdata.parameter.fillType;
    var bStepFill =  bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_NORMAL.value;
    document.getElementById('step-fill-group').hidden = !bStepFill;
    if(bStepFill){
      document.getElementById('param-fill-sewDirection').value = pesdata.parameter.fill.sewDirection;
      document.getElementById('param-fill-density').value = pesdata.parameter.fill.density;
    }

    var bPatternFill = bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_PATTERN.value;
    document.getElementById('pattern-fill-group').hidden = !bPatternFill;
    if(bPatternFill){
      document.getElementById('param-patternFill-size').value = pesdata.parameter.patternFill.patternSize;
      document.getElementById('param-patternFill-sewDirection').value = pesdata.parameter.patternFill.sewDirection;
      document.getElementById('param-patternFill-density').value = pesdata.parameter.patternFill.density;
    }
    
    var bMotifFill = bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_MOTIF.value;
    document.getElementById('motif-fill-group').hidden = !bMotifFill;
    if(bMotifFill){
      document.getElementById('param-motifFill-size').value = pesdata.parameter.motifFill.size;
      document.getElementById('param-motifFill-spacing').value = pesdata.parameter.motifFill.spacing;
      document.getElementById('param-motifFill-rowSpacing').value = pesdata.parameter.motifFill.rowSpacing;
    }

  }
  else{
    document.getElementById("Properties").hidden = true;
  }
}

// update selected pesdata.parameter from Properties panel then apply fill and stroke
function onApplyButtonPressed(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    // var pesdata = pesDoc.getDataObjects().get(selectedIndex);
    var pesdata = pesDoc.getDataObject(selectedIndex);
    if(pesdata && pesdata.isScalable()){
      var bText = pesdata.parameter.isTTFPath || pesdata.parameter.isPPEFPath;
      var bPPEF = pesdata.parameter.isPPEFPath;
      var bTTF = pesdata.parameter.isTTFPath;
      var bFontSize = bText;
      var bFillColor = pesdata.isScalable();
      var bSVGFromfile = pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE;
      var bScalable = (pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_SVG_FILE || 
        pesdata.parameter.type==CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_TTF_TEXT);
      var bSatinFill = pesdata.parameter.fillType==CanvasKit.PesFillType.FILL_TYPE_SATIN_COLUMN;

      var stType = pesdata.parameter.strokeType;
      var bStrokeRunning = bScalable && (stType.value>=CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING.value && 
        stType.value<=CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_BEAN.value);
      var bStrokeSatin = bScalable &&(stType.value>=CanvasKit.PesStrokeType.STROKE_TYPE_SATIN.value && 
        stType.value<=CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_INNER.value);
      var bStrokeMotif = bScalable && stType.value==CanvasKit.PesStrokeType.STROKE_TYPE_MOTIF.value;

      var fillType = pesdata.parameter.fillType;
      var bStepFill = bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_NORMAL.value;
      var bPatternFill = bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_PATTERN.value;
      var bMotifFill = bScalable && fillType.value==CanvasKit.PesFillType.FILL_TYPE_MOTIF.value;

      var param = pesdata.parameter.copy();

      if(bText){
        param.text = document.getElementById("paramText").value;
      }
      if(bPPEF){
        param.fontName = document.getElementById("paramPPEFFont").value;
        param.shapeIndex = document.getElementById("paramEffectType").selectedIndex;
        param.italic = document.getElementById("paramItalic").checked;
        param.border = document.getElementById("paramBorder").checked;
        // param.fontWeight = parseInt( document.getElementById("paramFontWeight").value );
      }
      else if(bTTF){
        param.fontName = document.getElementById("paramTTFFont").value;
      }
      if(bFontSize){
        param.fontSize = parseInt(document.getElementById("paramFontSize").value);
      }
      if(bFillColor){
        // fill color
        var c = CanvasKit.pesUtility.HtmlColorToPesColor(document.getElementById("paramFillColor").value);
        var results = findNearestBrotherColor(c);
        param.fillColorIndex = results[0];
        document.getElementById("paramFillColor").value = CanvasKit.pesUtility.PesColorToHtmlColor(results[1]);
      }
      if(bSVGFromfile){
        var checked = document.getElementById('paramAutoColor').checked;
        param.useColorFromPicker = !checked;
      }
      if(bScalable){
        // stroke color
        var c = CanvasKit.pesUtility.HtmlColorToPesColor(document.getElementById("paramStrokeColor").value);
        var results = findNearestBrotherColor(c);
        param.colorIndex = results[0];
        document.getElementById("paramStrokeColor").value = CanvasKit.pesUtility.PesColorToHtmlColor(results[1]);
      
        // fill type
        var ft = document.getElementById("paramFillType").value;
        switch(ft){
          case 'none':
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_NONE;
            break;

          case 'normal':
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_NORMAL;
            break;

          case 'pattern':
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_PATTERN;
            break;

          case 'motif':
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_MOTIF;
            break;

          case 'color':
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_COLOR;
            break;

          default:
            param.fillType = CanvasKit.PesFillType.FILL_TYPE_NONE;
            break;
        }

        // stroke type
        var st = document.getElementById("paramStrokeType").value;
        switch(st){
          case 'none':
            param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
            break;

          case 'running':
            param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING;
            break;

          case 'double':
            param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_DOUBLE;
            break;

          case 'triple':
            param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_TRIPLE;
            break;

          case 'bean':
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_RUNNING_BEAN;
          break;

          case 'satin':
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_SATIN;
          break;

          case 'satin-outset':
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_OUTER;
          break;

          case 'satin-inset':
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_SATIN_INNER;
          break;

          case 'motif':
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_MOTIF;
          break;

          default:
            param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
            break;
        }

        // join type
        var jt = document.getElementById("paramJoinType").value;
        switch(jt){
          case 'square':
            param.strokeJoinType = CanvasKit.PesJoinType.JOINT_TYPE_SQUARE;
            break;

          case 'round':
            param.strokeJoinType = CanvasKit.PesJoinType.JOINT_TYPE_ROUND;
            break;

          case 'miter':
            param.strokeJoinType = CanvasKit.PesJoinType.JOINT_TYPE_MITER;
            break;

          default:
            param.strokeJoinType = CanvasKit.PesJoinType.JOINT_TYPE_SQUARE;
          break;
        }
      }

      if(bSatinFill){
        var fill = param.fill;
        fill.density = parseFloat(document.getElementById('param-satin-fill-density').value);
        param.fill = fill;
        param.pullCompensate = parseFloat( document.getElementById('param-pullCompensate').value);
        // console.log('density(0): '+param.fill.density);
      }

      if(bStrokeRunning){
        param.strokeRunPitch = parseFloat(document.getElementById('param-strokeRunPitch').value);
        param.strokeRunningInset = parseFloat(document.getElementById('param-strokeRunningInset').value);
      }
      if(bStrokeSatin){
        param.strokeWidth = parseFloat(document.getElementById('param-strokeWidth').value );
        param.strokeDensity = parseFloat(document.getElementById('param-strokeDensity').value );
      }
      if(bStrokeMotif){
        var motif = param.motif;
        motif.size = parseFloat(document.getElementById('param-motif-size').value);
        motif.spacing = parseFloat(document.getElementById('param-motif-spacing').value);
        motif.runPitch = parseFloat(document.getElementById('param-motif-runPitch').value);
        param.motif = motif;
      }

      if(bStepFill){
        var fill = param.fill;
        fill.sewDirection = parseInt(document.getElementById('param-fill-sewDirection').value);
        fill.density = parseFloat(document.getElementById('param-fill-density').value);
        param.fill = fill;
      }
      if(bPatternFill){
        var patternFill = param.patternFill;
        patternFill.patternSize = parseFloat(document.getElementById('param-patternFill-size').value);
        patternFill.sewDirection = parseFloat(document.getElementById('param-patternFill-sewDirection').value);
        patternFill.density = parseFloat(document.getElementById('param-patternFill-density').value);
        param.patternFill = patternFill;
      }
      if(bMotifFill){
        var motifFill = param.motifFill;
        motifFill.size = parseFloat(document.getElementById('param-motifFill-size').value);
        motifFill.spacing = parseFloat(document.getElementById('param-motifFill-spacing').value);
        motifFill.rowSpacing = parseFloat(document.getElementById('param-motifFill-rowSpacing').value);
        param.motifFill = motifFill;
      }

      pesdata.parameter = param;
      if(bPPEF){
        // reproduce ppef text object
        var text = pesdata.parameter.text.trim();
        if(text){
          reproducePPEFText(pesdata);
        }
      }
      else if(bTTF){
        // reproduce ttf text object
        var text = pesdata.parameter.text.trim();
        if(text){
          reproduceTTFText(pesdata);
        }
      }
      else{
        pesNodeCanvas._updateImageCache(selectedIndex);
      }
    }
  }
}
applyButtonElm.addEventListener('click', onApplyButtonPressed, true);

hoopSizeElm.onchange = function(){
  var arr = hoopSizeElm.value.split('x');
  if(arr.length==2){
    var hw = parseFloat(arr[0]);
    var hh = parseFloat(arr[1]);
    CanvasKit.pesDocument.getInstance().setHoopSizeInMM(hw*10, hh*10); // *10 cm.=>mm.
  }
}

// Tool
deactivateTools = function(){
  var pesTools = document.getElementsByClassName("pestool");
  for(let i=0; i<pesTools.length; i++){
    pesTools[i].classList.remove('active');
  }
}

onSelectTool = function(){
  var allow = pesNodeCanvas.requestTool('selectTool');
  if(allow[0]=='selectTool' && allow[1]){
    deactivateTools();
    selectToolElm.classList.add('active');
  }
}
selectToolElm.addEventListener("click", onSelectTool, true);

onHandTool = function(){
  var allow = pesNodeCanvas.requestTool('handTool');
  if(allow[0]=='handTool' && allow[1]){
    deactivateTools();
    handToolElm.classList.add('active');
  }
}
handToolElm.addEventListener("click", onHandTool, true);

onShapeTool = function(){
  var allow = pesNodeCanvas.requestTool('shapeTool');
  if(allow[0]=='shapeTool' && allow[1]){
    deactivateTools();
    shapeToolElm.classList.add('active');

    $('#shape-modal').modal('toggle');
    var elm = document.getElementById("shape-modal-ok-button");

    elm.onclick = function(){
      var shapeListElm = document.getElementById('shape-list-group');
      // console.log('sel:'+shapeListElm.selectedIndex +' value:'+shapeListElm.value);
      var name = 'SHAPE/' + shapeListElm.value;
      var url = './assets/' + name + '.svg';
      pesResourceProvider.loadResource(name, url, function(buffer){
        var uints = new Uint8Array(buffer);
        var pbuff = CanvasKit.pesFactory.MakeBuffer();
        pbuff.set(uints);
        var pesdata = CanvasKit.pesFactory.MakePesData();
        pesdata.loadSVGFromBuffer(pbuff, true);
        var cnt = CanvasKit.pesDocument.getInstance().addObject(pesdata);
        if(cnt){
          pesNodeCanvas._updateImageCache(cnt-1);
          pesNodeCanvas.setSelectedIndex(cnt-1);
        }

        pbuff.delete();
        pesdata.delete();
      });

      $('#shape-modal').modal('hide');
    }

    document.getElementById('shape-modal').onhidden = function(){
      selectToolElm.click();
    }
  }
}
shapeToolElm.addEventListener("click", onShapeTool, true);

onTTFFontTool = function(){
  var allow = pesNodeCanvas.requestTool('ttfFontTool');
  if(allow[0]=='ttfFontTool' && allow[1]){
    deactivateTools();
    ttfFontToolElm.className += " active";

    $('#embFontModal').modal('toggle');
    var elm = document.getElementById("modal-ok-button");
    onTTFFontCreate = function(){
      // console.log('onTTFFontCreate ' + document.getElementById("modal-input-text").value);
      var text = document.getElementById("modal-input-text").value;
      if(text){
        const ptSize = 80;
        textSize(ptSize);
        var sk_path = textPath( text, 0, 0 );
        sk_path.simplify();
        var pes_path = CanvasKit.pesUtility.SKPathToPesPath(sk_path);
        var pes = CanvasKit.pesFactory.MakePesData();
        var pes_paths = pes_path.getSubPath();
        var fc = CanvasKit.pesFactory.MakeColor(); 
        fc.r = 232; fc.g=169; fc.b=0; fc.a=255;   // {{232, 169,   0}, "Deep Gold",       "214"} (11)

        var sc = CanvasKit.pesFactory.MakeColor();
        sc.r=41; sc.g=49; sc.b=51; sc.a=255;      // {{ 41,  49,  51}, "Dark Gray",       "707"} (23)

        for(let i=0; i<pes_paths.size(); i++){
          var p = pes_paths.get(i);
          p.setFilled(true);
          p.setStrokeWidth(2);
          p.setFillColor(fc); 
          p.setStrokeColor(sc); 
          pes_paths.set(i, p);
        }
        pes.paths = pes_paths;
        var param = CanvasKit.pesFactory.MakeParam();
        param.setType(CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_TTF_TEXT);
        param.text = text;
        param.isTTFPath = true;
        param.fontName = "TTF/JS-Boaboon.ttf";
        param.fillColorIndex = 11;
        param.colorIndex = 23;
        param.fillType = CanvasKit.PesFillType.FILL_TYPE_NONE;
        param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
        // pes.parameter = param;
        param.lastFontSize = param.fontSize;
        param.ppefScaleX = param.ppefScaleY = 1;
        pes.parameter = param;

        pes.calcBoundingBox();
        var b = pes.getBoundingBox();
        if(b.getCenter().x!=0 || b.getCenter().y!=0){
          pes.translate(-b.getCenter().x, -b.getCenter().y);
        }
        var cnt = CanvasKit.pesDocument.getInstance().addObject(pes);
        if(cnt>0){
          const lastIndex = cnt-1;
          pesNodeCanvas._updateImageCache(lastIndex);
          pesNodeCanvas.setSelectedIndex(lastIndex);
        }

        sk_path.delete();
        pes_path.delete();
        pes.delete();
      }
      // elm.removeEventListener('click', onTTFFontCreate, true);
      $('#embFontModal').modal('hide');
    }
    // elm.addEventListener("click", onTTFFontCreate, true);
    elm.onclick = onTTFFontCreate;

    document.getElementById('embFontModal').onhidden = function(){
      selectToolElm.click();
    }

  }
}
ttfFontToolElm.addEventListener("click", onTTFFontTool, true);

onPPEFFontTool = function(){
  var allow = pesNodeCanvas.requestTool('ppefFontTool');
  if(allow[0]=='ppefFontTool' && allow[1]){
    deactivateTools();
    ppefFontToolElm.className += " active";

    $('#embFontModal').modal('toggle');
    var elm = document.getElementById("modal-ok-button");
    onPPEFFontCreate = function(){
      var text = document.getElementById("modal-input-text").value;
      if(text){
        var utf8 = text.trim();
        var shapes = defaultPPEF.getStringAsShapes(utf8, 0, 0); 
        if(shapes.size()){
          var pes = CanvasKit.pesFactory.MakePesData();
          pes.paths = shapes;
          var param = CanvasKit.pesFactory.MakeParam();
          param.setType(CanvasKit.PesObjectType.OBJECT_TYPE_SCALABLE_PPEF_TEXT);
          param.text = text;
          param.isPPEFPath = true;
          param.fontName = "PPEF/Thai001.ppef";
          param.fillColorIndex = 11;
          param.fillType = CanvasKit.PesFillType.FILL_TYPE_SATIN_COLUMN;
          param.strokeType = CanvasKit.PesStrokeType.STROKE_TYPE_NONE;
          pes.parameter = param;
          const unit_per_mm = 10;
          var s = (1/300) * (pes.parameter.fontSize*unit_per_mm);
          pes.scale(s, s);

          // reset cumulative scale
          param = pes.parameter;
          param.lastFontSize = param.fontSize;
          param.ppefScaleX = param.ppefScaleY = 1;
          pes.parameter = param;

          pes.applyPPEFFill();
          if(pes.getBoundingBox().getCenter().x!=0 || pes.getBoundingBox().getCenter().y!=0){
            pes.translate(-pes.getBoundingBox().getCenter().x, -pes.getBoundingBox().getCenter().y);
          }
            var cnt = CanvasKit.pesDocument.getInstance().addObject(pes);
            if(cnt>0){
              const lastIndex = cnt-1;
              pesNodeCanvas._updateImageCache(lastIndex);
              pesNodeCanvas.setSelectedIndex(lastIndex);
            }
        }
      }
//      elm.removeEventListener('click', onPPEFFontCreate, true);
      $('#embFontModal').modal('hide');
    }
    // elm.addEventListener("click", onPPEFFontCreate, true);
    elm.onclick = onPPEFFontCreate;

    // selectToolElm.click();
    document.getElementById('embFontModal').onhidden = function(){
      selectToolElm.click();
    }
  }
}
ppefFontToolElm.addEventListener("click", onPPEFFontTool, true);

onSatinColumnTool = function(){
  var allow = pesNodeCanvas.requestTool('satinColumnTool');
  if(allow[0]=='satinColumnTool' && allow[1]){
    deactivateTools();
    satinColumnToolElm.className += " active";
  }
}
satinColumnToolElm.addEventListener("click", onSatinColumnTool, true);

onDrawingTool = function(){
  var allow = pesNodeCanvas.requestTool('drawingTool');
  if(allow[0]=='drawingTool' && allow[1]){
    deactivateTools();
    drawingToolElm.className += " active";
  }
}
drawingToolElm.addEventListener("click", onDrawingTool, true);


zoomSelectElm.oninput = function() {
  var value = zoomSelectElm.value;
  if(value!='Set Zoom Level'){
    var zoomLevel = parseFloat(value);
    if(zoomLevel!==NaN){
      pesNodeCanvas.setZoomLevel(zoomLevel);
    }
  }
}

canvasZoomIn= function() {
  var zoomLevel = pesNodeCanvas.zoomIn();
  if(zoomLevel!=undefined){
    var idx = zoomLevel[1];
    zoomSelectElm.options.selectedIndex = idx+1;
  }
}
zoomInElm.addEventListener("click", canvasZoomIn, true);

canvasZoomOut= function() {
  var zoomLevel = pesNodeCanvas.zoomOut();
  if(zoomLevel!=undefined){
    var idx = zoomLevel[1];
    zoomSelectElm.options.selectedIndex = idx+1;
  }
}
zoomOutElm.addEventListener("click", canvasZoomOut, true);


openfileElm.onchange = function(){
  var pesDoc = CanvasKit.pesDocument.getInstance();
  var f = openfileElm.files[0];
	var r = new FileReader();
	r.onload = function () {
    var view = new Uint8Array(r.result);
    var pbuff = CanvasKit.pesFactory.MakeBuffer();
    pbuff.set(view);
    if(pesNodeCanvas.loadPPES(pbuff)){
      for(let i=0; i<pesDoc.getObjectCount(); i++){
        var param = pesDoc.getDataObject(i).parameter;
        if(param.fillType==CanvasKit.PesFillType.FILL_TYPE_PATTERN && !param.patternFill.patternBuffer.size()){
          var name = param.patternFill.patternName;
          var url = './assets/PAS/' + name + '.pas';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);
            var param = pesDoc.getDataObject(i).parameter.copy();
            var patternFill = param.patternFill;
            patternFill.patternBuffer = pbuff;
            param.patternFill = patternFill;
            pesDoc.getDataObject(i).parameter = param;
            pesNodeCanvas._updateImageCache(i);
            pbuff.delete();
          });
        }
        if(param.fillType==CanvasKit.PesFillType.FILL_TYPE_MOTIF && !param.motifFill.patternBuffer.size()){
          var name = param.motifFill.patternName;
          var url = './assets/PMF/' + name + '.pmf';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);
            var param = pesDoc.getDataObject(i).parameter.copy();
            var motifFill = param.motifFill;
            motifFill.patternBuffer = pbuff;
            param.motifFill = motifFill;
            pesDoc.getDataObject(i).parameter = param;
            pesNodeCanvas._updateImageCache(i);
            pbuff.delete();
          });
        }
        if(param.strokeType==CanvasKit.PesStrokeType.STROKE_TYPE_MOTIF && !param.motif.patternBuffer.size()){
          var name = param.motif.patternName;
          var url = './assets/PMF/' + name + '.pmf';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);
            var param = pesDoc.getDataObject(i).parameter.copy();
            var motif = param.motif;
            motif.patternBuffer = pbuff;
            param.motif = motif;
            pesDoc.getDataObject(i).parameter = param;
            pesNodeCanvas._updateImageCache(i);
            pbuff.delete();
          });
        }
      }
      pesNodeCanvas.updateImageCache();
      console.log('Load .ppes: OK');
    }
    pbuff.delete();
  }
  r.readAsArrayBuffer(f);
}

var openElm = document.getElementById('menubar-open-button');
onOpen = function(){
  var pesDoc = CanvasKit.pesDocument.getInstance();
  if(pesDoc.getObjectCount()){
    if(confirm("Open new document will delete all objects. Are you sure?")){
      openfileElm.click();
    }
  }else{
    openfileElm.click();
  }
}
openElm.addEventListener("click", onOpen, true);


importfileElm.onchange = function(){
  var f = importfileElm.files[0];
	var r = new FileReader();
	r.onload = function () {
    var view = new Uint8Array(r.result);
    var pbuff = CanvasKit.pesFactory.MakeBuffer();
    pbuff.set(view);

    var data = pbuff.getData();
    var doc = CanvasKit.pesDocument.getInstance();
    if(data[0]==0x23 && data[1]==0x50 && data[2]==0x45 && data[3]==0x53){
      console.log("#PES");
      var pes = new CanvasKit.pesData();
      if(pes.loadPESFromBuffer(pbuff, true)){
        doc.addObject(pes);
        pesNodeCanvas.updateImageCache();
      }
      pes.delete();
    }
    else if(data[0]==0x23 && data[1]==0x50 && data[2]==0x49 && data[3]==0x4E && data[4]==0x4E){
      console.log("#PINN");
      var pinn = new CanvasKit.pesData();
      if(pinn.loadPINNFromBuffer(pbuff, true, false)){
        doc.addObject(pinn);
        pesNodeCanvas.updateImageCache();
      }
      pinn.delete();
    }
    else if(data[0]==0x3C){
      console.log("SVG??");
      var svg = new CanvasKit.pesData();
      if(svg.loadSVGFromBuffer(pbuff, true)){
        doc.addObject(svg);
        pesNodeCanvas.updateImageCache();
      }
      svg.delete();
    }
    pbuff.delete();
  }
  r.readAsArrayBuffer(f);
}

var importElm = document.getElementById('menubar-import-button');
onImport = function(){
  importfileElm.click();
}
importElm.addEventListener("click", onImport, true);



importfileClassifyElm.onchange = function(){
  var f = importfileClassifyElm.files[0];
	var r = new FileReader();
	r.onload = function () {
    var view = new Uint8Array(r.result);
    var pbuff = CanvasKit.pesFactory.MakeBuffer();
    pbuff.set(view);

    var data = pbuff.getData();
    var doc = CanvasKit.pesDocument.getInstance();
    if(data[0]==0x23 && data[1]==0x50 && data[2]==0x45 && data[3]==0x53){
      console.log("#PES");
      var pes = new CanvasKit.pesData();
      if(pes.loadPESFromBuffer(pbuff, true)){
       // doc.addObject(pes);
        var objectCnt = doc.classifyObject(pes);
        pesNodeCanvas.updateImageCache();
        console.log('classified obj count: ' + objectCnt);
        // for(let i=0; i< objectCnt; i++){
        //   if(doc.getDataParameter(i).fillType==CanvasKit.PesFillType.FILL_TYPE_NORMAL){
        //     doc.clipPathForObject(i);
        //     pesNodeCanvas._updateImageCache(i);
        //     console.log('clipPathForObject: '  + i);
        //     console.log(doc.getDataObject(i).paths);
        //   }
        // }
      }
      pes.delete();
    }
    else if(data[0]==0x23 && data[1]==0x50 && data[2]==0x49 && data[3]==0x4E && data[4]==0x4E){
      console.log("#PINN");
      var pinn = new CanvasKit.pesData();
      if(pinn.loadPINNFromBuffer(pbuff, true, false)){
        //doc.addObject(pinn);
        doc.classifyObject(pinn);
        pesNodeCanvas.updateImageCache();
      }
      pinn.delete();
    }
    pbuff.delete();
  }
  r.readAsArrayBuffer(f);
}

var importClassifyElm = document.getElementById('menubar-import-classify-button');
onImportClassify = function(){
  importfileClassifyElm.click();
}
importClassifyElm.addEventListener("click", onImportClassify, true);

function savePPES5() {  
  var ppesbuff = CanvasKit.pesDocument.getInstance().getPPESBuffer();
  var arraybuff = CanvasKit.pesUtility.PesBufferToByteArray(ppesbuff);
  var blob = new Blob([arraybuff]);
  var a = document.createElement("a");
  document.body.appendChild(a);
  a.href = window.URL.createObjectURL(blob);
  a.download = "untitled.ppes5";
  a.onclick = function () {
    setTimeout(function () {
      window.URL.revokeObjectURL(a.href);
    }, 1500);
  };
  a.click();
}
savefileElm.addEventListener("click", savePPES5, true);

function newDocument(){
  if(CanvasKit.pesDocument.getInstance().getObjectCount()){
    if(confirm("Delete all document's objects. Are you sure?")){
      CanvasKit.pesDocument.getInstance().newDocument();
      pesNodeCanvas.deSelectObject();
      pesNodeCanvas.clearAllImages();
      var elm = document.getElementById('layer-list-group');
      elm.innerHTML = "";
    }
  }
}
newDocElm.addEventListener("click", newDocument, true);

function exportPES(){
  var pesbuff = CanvasKit.pesDocument.getInstance().getPES5Buffer();
  var arraybuff = CanvasKit.pesUtility.PesBufferToByteArray(pesbuff);
  var blob = new Blob([arraybuff]);
  var a = document.createElement("a");
  document.body.appendChild(a);
  a.href = window.URL.createObjectURL(blob);
  a.download = "untitled.pes";
  a.onclick = function () {
    setTimeout(function () {
      window.URL.revokeObjectURL(a.href);
    }, 1500);
  };
  a.click();
}
exportfileElm.addEventListener("click", exportPES, true);

// Stitch Simulator
// function simulatorStart(){
//   pesNodeCanvas.getSimulator().start();
// }
// simStart.addEventListener("click", simulatorStart, true);

// function simulatorPause(){
//   pesNodeCanvas.getSimulator().pause();
// }
// simPause.addEventListener("click", simulatorPause, true);

// function simulatorStop(){
//   pesNodeCanvas.getSimulator().stop();
// }
// simStop.addEventListener("click", simulatorStop, true);


findNearestBrotherColor = function(srcColor){
  var currentClosestValue = 9999999;
  var closestIndex = -1;
  var red = srcColor.r, green = srcColor.g, blue = srcColor.b;
  var deltaRed, deltaGreen, deltaBlue;
  var dist;
  for(let i=1; i<=64; i++){
    var c = CanvasKit.pesUtility.GetBrotherColor(i);
    deltaRed = red - c.r;
    deltaBlue = green - c.g;
    deltaGreen = blue - c.b;
    dist = Math.sqrt((deltaRed * deltaRed) + (deltaBlue * deltaBlue) + (deltaGreen * deltaGreen));
    if(dist <= currentClosestValue)
    {
        currentClosestValue = dist;
        closestIndex = i;
    }
  }
  return [closestIndex, CanvasKit.pesUtility.GetBrotherColor(closestIndex)];
}

effectElm.onchange = function(){
  if(effectElm.selectedIndex<=6){
    document.getElementById("italic-group").hidden = false;
    document.getElementById("border-group").hidden = false;
  }
  else{
    document.getElementById("italic-group").hidden = true;
    document.getElementById("border-group").hidden = true;
  }
}

createPesEffect = function(data){
  var centerx = data.centerx;
  var centery = data.centery;
  var angle = data.parameter.angleValue;
  var radius = data.parameter.radiusValue;

  var baselineIndex = data.parameter.shapeIndex;
  var bItalic = data.parameter.italic;
  var bCreateBorder = data.parameter.border;
  var effect = null;

  if(baselineIndex==0){
      effect = new CanvasKit.pesEffectNormal();
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==1){
      effect = new CanvasKit.pesEffectArchTop();
      effect.angle = angle;
      effect.radius = radius;
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==2){
      effect = new CanvasKit.pesEffectArchBottom();
      effect.angle = angle;
      effect.radius = radius;
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==3){
      effect = new CanvasKit.pesEffectCircle();
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==4){
      effect = new CanvasKit.pesEffectSineWave();
      effect.magnitude = data.parameter.waveMagnitude;
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==5){
      var bChevronUp = true;
      effect = new CanvasKit.pesEffectChevron(bChevronUp);
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==6){
      var bChevronUp = false;
      effect = new CanvasKit.pesEffectChevron(bChevronUp);
      effect.borderGap = 100 + data.parameter.borderGap;
      effect.borderGapY = 100 + data.parameter.borderGapY;
  }
  else if(baselineIndex==7){
      var bSlantUp = true;
      effect = new CanvasKit.pesEffectSlant(bSlantUp);
      effect.angle = data.parameter.slantUpAngle;
  }
  else if(baselineIndex==8){
      var bSlantUp = false;
      effect = new CanvasKit.pesEffectSlant(bSlantUp);
      effect.angle = data.parameter.slantDownAngle;
  }
  else if(baselineIndex==9){
      effect= new CanvasKit.pesEffectTriangleUp();
  }
  else if(baselineIndex==10){
      effect= new CanvasKit.pesEffectTriangleDown();
  }
  else if(baselineIndex==11){
      effect = new CanvasKit.pesEffectFadeRight();
  }
  else if(baselineIndex==12){
      effect= new CanvasKit.pesEffectFadeLeft();
  }
  else if(baselineIndex==13){
      effect = new CanvasKit.pesEffectFadeUp();
      effect.slantFactor = data.parameter.fadeUpSlant;
  }
  else if(baselineIndex==14){
      effect = new CanvasKit.pesEffectFadeDown();
      effect.slantFactor = data.parameter.fadeDownSlant;
  }
  else if(baselineIndex==15){
      effect = new CanvasKit.pesEffectInflate();
  }

  if(effect){
    effect.bItalic = bItalic;
    effect.bCreateBorder = bCreateBorder;
  }
  return effect;
}

reproducePPEFText = function(pesdata){
  // var p = pesdata.parameter;
  // console.log('scale:' + p.ppefScaleX + ',' + p.ppefScaleY + ' fontsize:' + p.lastFontSize + '=>' + p.fontSize);

  var text = pesdata.parameter.text.trim();
  var name = pesdata.parameter.fontName;
  var url = './assets/' + name;
  pesResourceProvider.loadResource(name, url, function(buffer){
    var uints = new Uint8Array(buffer);
    var oldCenter = pesdata.getBoundingBox().getCenter();
    var ppef = new SQLJS();
    ppef.openPPEF(uints);
    ppef.readPPEFConfig();
    var shapes = ppef.getStringAsShapes(text, pesdata.parameter.extraLetterSpace, pesdata.parameter.extraSpace); 
    if(shapes.size()){
      var effect = createPesEffect(pesdata);
      if(effect){
        effect.applyPaths(shapes);
        effect.delete();
      }
      pesdata.clearPaths();
      pesdata.paths = shapes;
      const unit_per_mm = 10;
      var s = (1/300)*(pesdata.parameter.fontSize*unit_per_mm);
      var param = pesdata.parameter.copy();
      if(param.lastFontSize!=param.fontSize){
        param.lastFontSize = param.fontSize;
        param.ppefScaleX = param.ppefScaleY = 1.0;
      }
      pesdata.scale(s*param.ppefScaleX, s*param.ppefScaleY);
      pesdata.parameter = param;
      var newCenter = pesdata.getBoundingBox().getCenter();
      pesdata.translate(oldCenter.x-newCenter.x, oldCenter.y-newCenter.y);
      pesNodeCanvas._updateImageCache(pesNodeCanvas.getSelectedIndex());
    }
    ppef.closePPEF();
  });
}

reproduceTTFText = function(pesdata){
  var text = pesdata.parameter.text.trim();
  var name = pesdata.parameter.fontName;
  var url = './assets/' + name;
  pesResourceProvider.loadResource(name, url, function(buffer){
    // var uints = new Uint8Array(buffer);
    var oldCenter = pesdata.getBoundingBox().getCenter();
    const fontMgr = CanvasKit.SkFontMgr.RefDefault();
    const fontFace = fontMgr.MakeTypefaceFromData(buffer);
    const ptSize = pesdata.parameter.fontSize * 10;
    const ttf = new CanvasKit.SkFont(fontFace, ptSize);
    if(ttf){
      // var textPath = ttf.getPath(text, 0, 0);
      // var pes_path = CanvasKit.pesUtility.SKPathToPesPath(textPath);
      // var pes_paths = pes_path.getSubPath();
      // var fc = CanvasKit.pesFactory.MakeColor(); 
      // fc.r = 232; fc.g=169; fc.b=0; fc.a=255; // Deep Gold
      // var sc = CanvasKit.pesFactory.MakeColor();
      // sc.r=41; sc.g=49; sc.b=51; sc.a=255; // Dark Grey
      // for(let i=0; i<pes_paths.size(); i++){
      //   var p = pes_paths.get(i);
      //   p.setFilled(true);
      //   p.setStrokeWidth(2);
      //   p.setFillColor(fc); 
      //   p.setStrokeColor(sc); 
      //   pes_paths.set(i, p);
      // }
      // pesdata.clearPaths();
      // pesdata.paths = pes_paths;

      console.log('reproduceTTFText fixed!');
      var textPath = ttf.getPath(text, 0, 0);
      textPath.simplify();
      var pes_path = CanvasKit.pesUtility.SKPathToPesPath(textPath);
      var fc = CanvasKit.pesFactory.MakeColor(); 
      fc.r = 232; fc.g=169; fc.b=0; fc.a=255; // Deep Gold
      var sc = CanvasKit.pesFactory.MakeColor();
      sc.r=41; sc.g=49; sc.b=51; sc.a=255; // Dark Grey
      
      pes_path.setFilled(true);
      pes_path.setStrokeWidth(2);
      pes_path.setFillColor(fc); 
      pes_path.setStrokeColor(sc); 
      
      pesdata.clearPaths();
      var pes_paths = pesdata.paths;
      pes_paths.push_back(pes_path);
      pesdata.paths = pes_paths;

      var param = pesdata.parameter.copy();
      if(param.lastFontSize!=param.fontSize){
        param.lastFontSize = param.fontSize;
        param.ppefScaleX = param.ppefScaleY = 1.0;
      }
      pesdata.scale(param.ppefScaleX, param.ppefScaleY);
      pesdata.parameter = param;

      var newCenter = pesdata.getBoundingBox().getCenter();
      pesdata.translate(oldCenter.x-newCenter.x, oldCenter.y-newCenter.y);
      pesNodeCanvas._updateImageCache(pesNodeCanvas.getSelectedIndex());

      textPath.delete();
      pes_path.clear();
      pes_path.delete();
      ttf.delete();
    }
    fontFace.delete();
  });
}


strokeTypeElm.onchange = function(){
  var sel = strokeTypeElm.selectedIndex;
  if(sel>=1 && sel<=4){
    document.getElementById('stroke-running-group').hidden = false;
  }
  else{
    document.getElementById('stroke-running-group').hidden = true;
  }

  if(sel>=5 && sel<=7){
    document.getElementById('stroke-satin-group').hidden = false;
  }
  else{
    document.getElementById('stroke-satin-group').hidden = true;
  }

  if(sel==8){
    $('#motif-modal').modal('toggle');
    var okButtonElm = document.getElementById("motif-modal-ok-button");
    okButtonElm.onclick = function(){
      // do motif stroke
      // console.log('selectedIndex='+sel +' value='+strokeTypeElm.value);
      var listElm = document.getElementById('motif-list-group');
      if(listElm){
        console.log('List=>selectedIndex='+listElm.selectedIndex +' value='+listElm.value);
        var sel = pesNodeCanvas.getSelectedIndex();
        if(sel!=-1){
          var pesDoc = CanvasKit.pesDocument.getInstance();
          var param = pesDoc.getDataParameter(sel);
          var motif = param.motif;
          motif.patternIndex = listElm.selectedIndex;
          motif.patternName = listElm.value;
          var name = motif.patternName;
          var url = './assets/PMF/' + name + '.pmf';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);

            motif.patternBuffer = pbuff;
            param.motif = motif;
            pesDoc.getDataObject(sel).parameter = param;
            // console.log(pesDoc.getDataObject(sel).parameter);
          });
        }
      }
      

      $('#motif-modal').modal('hide');
      document.getElementById('stroke-motif-group').hidden = false;
    }
  }
  else{
    document.getElementById('stroke-motif-group').hidden = true;
  }
}

fillTypeElm.onchange = function(){
  var sel = fillTypeElm.selectedIndex;
  if(sel==1){
    document.getElementById('step-fill-group').hidden = false;
  }
  else{
    document.getElementById('step-fill-group').hidden = true;
  }

  if(sel==2){
    $('#pattern-modal').modal('toggle');
    var okButtonElm = document.getElementById("pattern-modal-ok-button");
    okButtonElm.onclick = function(){
      // do pattern fill
      var listElm = document.getElementById('pattern-list-group');
      if(listElm){
        var sel = pesNodeCanvas.getSelectedIndex();
        if(sel!=-1){
          var pesDoc = CanvasKit.pesDocument.getInstance();
          var param = pesDoc.getDataParameter(sel);
          var patternFill = param.patternFill;
          patternFill.patternIndex = listElm.selectedIndex;
          patternFill.patternName = listElm.value;
          var name = patternFill.patternName;
          var url = './assets/PAS/' + name + '.pas';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);

            patternFill.patternBuffer = pbuff;
            param.patternFill = patternFill;
            pesDoc.getDataObject(sel).parameter = param;
            pbuff.delete();
          });
        }
      }


      $('#pattern-modal').modal('hide');
      document.getElementById('pattern-fill-group').hidden = false;
    }
  }
  else{
    document.getElementById('pattern-fill-group').hidden = true;
  }

  if(sel==3){
    $('#motif-modal').modal('toggle');
    var okButtonElm = document.getElementById("motif-modal-ok-button");
    okButtonElm.onclick = function(){
      // do motif fill
      var listElm = document.getElementById('motif-list-group');
      if(listElm){
        console.log('List=>selectedIndex='+listElm.selectedIndex +' value='+listElm.value);
        var sel = pesNodeCanvas.getSelectedIndex();
        if(sel!=-1){
          var pesDoc = CanvasKit.pesDocument.getInstance();
          var param = pesDoc.getDataParameter(sel);
          var motifFill = param.motifFill;
          motifFill.patternIndex = listElm.selectedIndex;
          motifFill.patternName = listElm.value;
          var name = motifFill.patternName;
          var url = './assets/PMF/' + name + '.pmf';
          pesResourceProvider.loadResource(name, url, function(buffer){
            var uints = new Uint8Array(buffer);
            var pbuff = CanvasKit.pesFactory.MakeBuffer();
            pbuff.set(uints);

            motifFill.patternBuffer = pbuff;
            param.motifFill = motifFill;
            pesDoc.getDataObject(sel).parameter = param;
            pbuff.delete();
            // console.log(pesDoc.getDataObject(sel).parameter);
          });
        }
      }

      $('#motif-modal').modal('hide');
      document.getElementById('motif-fill-group').hidden = false;
    }
  }
  else{
    document.getElementById('motif-fill-group').hidden = true;
  }
}

// var interCharDecElm = document.getElementById("inter-char-dec");
// var interCharIncElm = document.getElementById("inter-char-inc");
// var spaceDecElm = document.getElementById("space-dec");
// var spaceIncElm = document.getElementById("space-inc");

const step = 1;
onInterCharDec = function(){
  var sel = pesNodeCanvas.getSelectedIndex();
  if(sel!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var param = pesDoc.getDataObject(sel).parameter.copy();
    param.extraLetterSpace = clamp(param.extraLetterSpace-step, -50, 50);
    pesDoc.getDataObject(sel).parameter = param;
    // console.log(pesDoc.getDataObject(sel).parameter.extraLetterSpace);
    onApplyButtonPressed(pesDoc.getDataObject(sel));
  }
}
interCharDecElm.addEventListener('click', onInterCharDec, true);

onInterCharInc = function(){
  var sel = pesNodeCanvas.getSelectedIndex();
  if(sel!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var param = pesDoc.getDataObject(sel).parameter.copy();
    param.extraLetterSpace = clamp(param.extraLetterSpace+step, -50, 50);
    pesDoc.getDataObject(sel).parameter = param;
    // console.log(pesDoc.getDataObject(sel).parameter.extraLetterSpace);
    onApplyButtonPressed(pesDoc.getDataObject(sel));
  }
}
interCharIncElm.addEventListener('click', onInterCharInc, true);

onSpaceDec = function(){
  var sel = pesNodeCanvas.getSelectedIndex();
  if(sel!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var param = pesDoc.getDataObject(sel).parameter.copy();
    param.extraSpace = clamp(param.extraSpace-step, -50, 50);
    pesDoc.getDataObject(sel).parameter = param;
    // console.log(pesDoc.getDataObject(sel).parameter.extraSpace);
    onApplyButtonPressed(pesDoc.getDataObject(sel));
  }
}
spaceDecElm.addEventListener('click', onSpaceDec, true);

onSpaceInc = function(){
  var sel = pesNodeCanvas.getSelectedIndex();
  if(sel!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var param = pesDoc.getDataObject(sel).parameter.copy();
    param.extraSpace = clamp(param.extraSpace+step, -50, 50);
    pesDoc.getDataObject(sel).parameter = param;
    // console.log(pesDoc.getDataObject(sel).parameter.extraSpace);
    onApplyButtonPressed(pesDoc.getDataObject(sel));
  }
}
spaceIncElm.addEventListener('click', onSpaceInc, true);


// Layer list item
reIndexLayerListItems = function(){
  var items = document.querySelectorAll('#layer-list-group li');
  for(let i=0; i<items.length; i++){
      items[i].onclick = function(){
        pesNodeCanvas.setSelectedIndex(i);
      };
  }
}

reIndexLayerListImageItems = function(){
  var imgs = document.querySelectorAll('#layer-list-group img');
  for(let i=0; i<imgs.length; i++){
      imgs[i].src = pesNodeCanvas.getPesImageData(i);
  }
}

updateLayerListTextItems = function(){
  var items = document.querySelectorAll('#layer-list-group p');
  for(let i=0; i<items.length; i++){
    var type = CanvasKit.pesDocument.getInstance().getDataObject(i).parameter.type;
    var str = pesNodeCanvas.objectTypeToString(type);
    var t = CanvasKit.pesDocument.getInstance().getDataObject(i).getTotalStitches();
    items[i].innerText = str;
    if(t>0){
      items[i].innerText += '( ' + t + ' )';
    }
  }
}

updateLayerList = function(){
  var items = document.querySelectorAll('#layer-list-group li');
  var cnt = CanvasKit.pesDocument.getInstance().getObjectCount();
  var diff = cnt - items.length;
  var listGroupElm = document.getElementById('layer-list-group');
  const listItemHtml = `
    <li class="list-group-item d-flex" style="height:70px">
    <div class="image-parent" style="margin-right:5px; min-width:70px; min-height:50px;">
        <img src="" alt="thumbnail" style="max-height:50px; max-width:70px;">
    </div>
    <p>Object</p>
    </li>
  `;
  if(diff>0){
    for(let i=0; i<diff; i++){
      listGroupElm.innerHTML += listItemHtml;
    }
  }
  else if(diff<0){
    // listGroupElm.innerHTML = "";
    // for(let i=0; i<cnt; i++){
    //   listGroupElm.innerHTML += listItemHtml;
    // }
    var num = Math.abs(diff);
    const lastIndex = items.length-1;
    for(let i=0; i<num; i++){
      items[lastIndex-i].remove();
    }
  }

  reIndexLayerListItems();
  reIndexLayerListImageItems();
  updateLayerListTextItems();
}


// Layer Toolbar Button
onMoveUp = function(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var hasChange = pesDoc.moveObjectBack(selectedIndex);
    if(hasChange){
      pesNodeCanvas.swabPesImageAndData(selectedIndex-1, selectedIndex);
      reIndexLayerListItems();
      reIndexLayerListImageItems();
      pesNodeCanvas.setSelectedIndex(selectedIndex-1)
    }
  }
}
moveUpElm.addEventListener("click", onMoveUp, true);

onMoveDown = function(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var hasChange = pesDoc.moveObjectFront(selectedIndex);
    if(hasChange){
      pesNodeCanvas.swabPesImageAndData(selectedIndex, selectedIndex+1);
      reIndexLayerListItems();
      reIndexLayerListImageItems();
      pesNodeCanvas.setSelectedIndex(selectedIndex+1)
    }
  }
}
moveDownElm.addEventListener("click", onMoveDown, true);

onDuplicate = function(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var hasChange = pesDoc.duplicateObject(selectedIndex);
    if(hasChange){
      pesNodeCanvas.insertPesImageAndData(selectedIndex+1,
        null,
        null);
        pesNodeCanvas._updateImageCache(selectedIndex+1);
        updateLayerList();
        pesNodeCanvas.setSelectedIndex(selectedIndex+1);
    }
  }
}
duplicateElm.addEventListener("click", onDuplicate, true);

onDelete = function(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var hasChange = pesDoc.deleteObject(selectedIndex);
    if(hasChange){
      pesNodeCanvas.removePesImageAndData(selectedIndex);
      updateLayerList();
      pesNodeCanvas.setSelectedIndex(-1);
    }
  }
}
deleteElm.addEventListener("click", onDelete, true);

onMerge = function(){
  var selectedIndex = pesNodeCanvas.getSelectedIndex();
  if(selectedIndex!=-1){
    var pesDoc = CanvasKit.pesDocument.getInstance();
    var hasChange = pesDoc.mergeObject(selectedIndex);
    if(hasChange){
      pesNodeCanvas.removePesImageAndData(selectedIndex+1);
      pesNodeCanvas._updateImageCache(selectedIndex);
      updateLayerList();
      pesNodeCanvas.setSelectedIndex(selectedIndex);
    }
  }
}
mergeElm.addEventListener("click", onMerge, true);


