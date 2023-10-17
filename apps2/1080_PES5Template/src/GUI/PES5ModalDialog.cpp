//
//  PES5ModalDialog.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 5/10/2565 BE.
//

#include "PES5ModalDialog.hpp"
#include "PES5DocumentView.hpp"
#include "DecSpinner.hpp"
#include "Utils/PesTracingUtils.hpp"
#include "PES5Command.hpp"
#include "tools/Resources.h"
#include "pesPathUtility.hpp"

namespace sk_ui {

class Pes5TracingDialogPreview : public DecWidget{
public:
    Pes5TracingDialogPreview(){}
    virtual ~Pes5TracingDialogPreview() override {}
    virtual std::string type() override { return "Pes5TracingDialogPreview"; }
    virtual void _draw_content(SkCanvas& canvas) override {
        if (!_isVisible) {
            return;
        }
        
        if(_results.size()){
            float sx = _width / imgWidth;
            float sy = _height / imgHeight;
            float s = std::min(sx, sy);
            canvas.save();
            {
                canvas.scale(s, s);
                SkPaint paint;
                paint.setStrokeWidth(2);
                paint.setAntiAlias(true);
                for(int i=0; i<_results.size(); i++){
                    pesPath path = _results[i].getPathData();
                    SkPath skpath = toSk(path);
                    if (path.isFill()) {
                        pesColor c = path.getFillColor();
                        paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                        paint.setStyle(SkPaint::kFill_Style);
                        canvas.drawPath(skpath, paint);
                    }
                    if (path.isStroke()) {
                        pesColor c = path.getStrokeColor();
                        paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                        paint.setStrokeWidth(path.getStrokeWidth());
                        paint.setStyle(SkPaint::kStroke_Style);
                        canvas.drawPath(skpath, paint);
                    }
                    skpath.reset();
                }
            }
            canvas.restore();
        }
    }
    
    std::vector<TracingEngineResult> _results;
    float imgWidth, imgHeight;
};

Pes5TracingDialogPreview & TracingPreview(){
    Pes5TracingDialogPreview *w = new Pes5TracingDialogPreview();
    return *w;
}

Pes5TracingDialog::Pes5TracingDialog() {

    size( 400, 320 );
    _( Column()
        .border(false)
        .padding(5, 5, 5, 5)
        ._( Text("Tracing...") )
        ._( Stack()
            .size( -1, -1 )
            .border( false )
            .background( false )
            .margin( 5, 5, 5, 5 )
            ._( Row()
                .border(false)
                .gap(5)
                ._( TracingPreview()
                    .size(200, 200)
                    .border(true)
                    .background(true)
                    .bg_color(0xffffff)
                    .ref((DecWidget**)&pTracingPreview)
                )
                ._( Column()
                    .border(false)
                    .gap(5)
                    ._( ComboBox().ref((DecWidget**)&pTraceTypeComboBox)
                       ._( ComboBoxItem( "Single (B&W)" ).value((int)TRACE_BRIGHTNESS) )
                       ._( ComboBoxItem( "Muti (Color)" ).value((int)TRACE_QUANT_COLOR) )
                    )
                   
                   ._( Text("Num color :") )
//                   ._( Slider(1, 16, 1).ref((DecWidget**)&pNumColorSlider) )
                   ._( Spinner(2, 1, 16, 1).ref((DecWidget**)&pNumColorSpinner) )
                   
                   ._( Text("Brigthness :") )
//                   ._( Slider(0, 100, 5).ref((DecWidget**)&pBrigthnessSlider) )
                   ._( Spinner(0.5, 0.0, 1.0, 0.05).ref((DecWidget**)&pBrigthnessSpinner) )
                   
//                   ._( CheckBox("Smooth").check(true, false).ref((DecWidget**)&pSmoothCheckBox) )
                   ._( CheckBox("Auto Embroidery").check(true, false).ref((DecWidget**)&pAutoEmbCheckBox) )
                )
            )
        )
        ._( Row()
            .size( -1, 0 )
            .border( false )
            .background( false )
            .margin( 10, 0, 20, 0  )
            .gap( 10 )

            ._(Spacer().size( -1, 0 ))
           
           
            ._(Button("Apply", 0, [=](DecButton &btn) {
                onApply();
            }).size( 100, 0 ))
           
           ._(Button("OK", 0, [=](DecButton &btn) {
               onOK();
           }).size( 100, 0 ))
           
           
            ._(Spacer().size( -1, 0 ))
        )
    );

    resetParameter();
}

Pes5TracingDialog::~Pes5TracingDialog() {
}

void Pes5TracingDialog::onOK() {
    int n = _results.size();
    if(n){
        pesData pes;

        pes.parameter.fillColorIndex = 65;
        pes.parameter.colorIndex = 65;

        pes.parameter.useColorFromPicker = !pAutoEmbCheckBox->isChecked();
        for(int i=0; i<n; i++){
            pesPath p = _results[i].getPathData();
            p.setFilled(true);
            p.setStrokeColor(p.getFillColor());
            p.setStrokeWidth(0.5);
            pes.paths.push_back(p);
        }
        pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
        if(pAutoEmbCheckBox->isChecked()){
            pes.parameter.fillTypeIndex = 1; // 0:None 1:Fill Normal Stitch
            pes.parameter.fillType = pesData::FILL_TYPE_NORMAL;
            pes.applyFill();
        }
        pesVec2f center = pes.getBoundingBox().getCenter();
        pes.translate(-center.x, -center.y);
//        int cnt = pesGetDocument()->addObject(pes);
//        if(cnt){
//            PES5_UpdatePesImage(cnt-1);
//            PES5_UpdateGUIRightPanel();
//        }
        PES5_AddPesData(pes);
    }
    hide();
}

void Pes5TracingDialog::onApply() {
    PesTracingUtils tracer;
    int v = (int)pTraceTypeComboBox->value();
    if(v==1){
        tracer.setTraceType(TRACE_QUANT_COLOR);
    }
    else{
        tracer.setTraceType(TRACE_BRIGHTNESS);
    }
    tracer.setMultiScanNrColors((int)pNumColorSpinner->value());
    tracer.setBrightnessThreshold(pBrigthnessSpinner->value());
    bool bAutoEmb = pAutoEmbCheckBox->isChecked();
    
    _results.clear();
    _results = tracer.trace(_bitmap);
    
    pTracingPreview->_results = _results;
    pTracingPreview->imgWidth = _bitmap.width();
    pTracingPreview->imgHeight = _bitmap.height();
}

void Pes5TracingDialog::setBitmap(const SkBitmap & bitmap){
    _bitmap = bitmap;
}

void Pes5TracingDialog::resetParameter(){
//    tracer = PesTracingUtils();
    pTraceTypeComboBox->select_comboboxItem(0);
    pNumColorSpinner->value(2);
    pBrigthnessSpinner->value(0.45);
    pAutoEmbCheckBox->check(true, false);
}

Pes5TracingDialog & TracingDialog() {
    Pes5TracingDialog *w = new Pes5TracingDialog();
    return *w;
}




class Pes5ShapeDialogPreview : public DecWidget{
public:
    Pes5ShapeDialogPreview(){}
    virtual ~Pes5ShapeDialogPreview() override {}
    virtual std::string type() override { return "Pes5ShapeDialogPreview"; }
    virtual void _draw_content(SkCanvas& canvas) override {
        if (!_isVisible) {
            return;
        }
        
        if(pes.paths.size()){
            float s = 0.25;
            canvas.save();
            {
                canvas.translate(_width/2, _height/2);
                canvas.scale(s, s);
                SkPaint paint;
                paint.setStrokeWidth(2);
                paint.setAntiAlias(true);
                for(int i=0; i<pes.paths.size(); i++){
                    pesPath path = pes.paths[i];
                    SkPath skpath = toSk(path);
                    if (path.isFill()) {
                        pesColor c = path.getFillColor();
                        paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                        paint.setStyle(SkPaint::kFill_Style);
                        canvas.drawPath(skpath, paint);
                    }
                    if (path.isStroke()) {
                        pesColor c = path.getStrokeColor();
                        paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                        paint.setStrokeWidth(path.getStrokeWidth());
                        paint.setStyle(SkPaint::kStroke_Style);
                        canvas.drawPath(skpath, paint);
                    }
                    skpath.reset();
                }
            }
            canvas.restore();
        }
    }
    
    pesData pes;
};

Pes5ShapeDialogPreview & ShapePreview(){
    Pes5ShapeDialogPreview *w = new Pes5ShapeDialogPreview();
    return *w;
}

Pes5ShapeDialog::Pes5ShapeDialog(){
    pShapePreview = NULL;
    pList = NULL;
    std::string shapeFilenames[] = {
        "triangle",
        "rectangle",
        "diamond",
        "hexagon",
        "octagon",
        "star",
        "heart",
        "circle",
        "round-rectangle",
        "ribbon",
        "chevron",
    };
    
    size(400, 320);
    _( Column()
       .border(false)
       .padding(5, 5, 5, 5)
       ._( Row()
           .border(false)
           .gap(5)
           ._( ShapePreview()
               .size(256, 256)
               .border(true)
               .background(true)
               .bg_color(0xffffff)
               .ref((DecWidget**)&pShapePreview)
              )
           ._( List()
               .size(-1, 256)
               .ref((DecWidget**)&pList)
               ._( ListItem("triangle") )
               ._( ListItem("rectangle") )
               ._( ListItem("diamond") )
               ._( ListItem("hexagon") )
               ._( ListItem("octagon") )
               ._( ListItem("star") )
               ._( ListItem("heart") )
               ._( ListItem("circle") )
               ._( ListItem("round-rectangle") )
               ._( ListItem("ribbon") )
               ._( ListItem("chevron") )
               .onValueChanged( [=](DecWidget &widget){
//                   SkDebugf( "Shape Changed %i - %s\n", (int)widget.value(), shapeFilenames[(int)widget.value()].c_str());
                   int selectedIndex = (int)widget.value();
                   if(pShapePreview){
                       std::string filepath = "SHAPE/" + shapeFilenames[selectedIndex] + ".svg";
                       PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data){
                           pesBuffer buff((const char*)data->data(), data->size());
                           pShapePreview->pes.loadSVGFromBuffer(buff);
                       });
                   }
               })
              )
          )
       ._( Row()
           .size( -1, 0 )
           .border( false )
           .background( false )
           .gap( 10 )
           ._(Spacer().size( -1, 0 ))
           ._(Button("Cancle", 0, [=](DecButton &btn) {
               onCancel();
           }).size( 100, 0 ))
           ._(Button("OK(Add)", 0, [=](DecButton &btn) {
               onOK();
           }).size( 100, 0 ))
           ._(Spacer().size( -1, 0 ))
          )
       ._( Spacer().size(-1, 10) )
      );
    
    if(pList)
        pList->select_listItem(0);
}

Pes5ShapeDialog::~Pes5ShapeDialog(){
    
}

void Pes5ShapeDialog::onCancel(){
    hide();
}

void Pes5ShapeDialog::onOK(){
    if(pShapePreview){
        // TODO: 
        // should be get colors from svg path attributes 
        // and map with pesGetNearestBrotherColorIndex
        pShapePreview->pes.parameter.fillColorIndex = 37;
        pShapePreview->pes.parameter.colorIndex = 20;
        pShapePreview->pes.parameter.text = "Shape";
//        int cnt = pesGetDocument()->addObject(pShapePreview->pes);
//        if(cnt){
//            PES5_UpdatePesImage(cnt-1);
//            PES5_UpdateGUIRightPanel();
//        }
        PES5_AddPesData(pShapePreview->pes);
    }
    hide();
}

Pes5ShapeDialog & ShapeDialog() {
    Pes5ShapeDialog *w = new Pes5ShapeDialog();
    return *w;
}

///////////////////////////
// PatternFill
///////////////////////////
class Pes5PatternPreview : public DecWidget{
public:
    Pes5PatternPreview(){}
    virtual ~Pes5PatternPreview() override {}
    virtual std::string type() override { return "Pes5PatternPreview"; }
    virtual void _draw_content(SkCanvas& canvas) override {
        if (!_isVisible) {
            return;
        }
        
        if(patternPath.getCommands().size()){
            float s = 0.25;
            canvas.save();
            {
//                canvas.translate(_width/2, _height/2);
                canvas.scale(s, s);
                SkPaint paint;
                paint.setStrokeWidth(2);
                paint.setAntiAlias(true);
                
                pesPath & path = patternPath;
                SkPath skpath = toSk(path);
                if (path.isFill()) {
                    pesColor c = path.getFillColor();
                    paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                    paint.setStyle(SkPaint::kFill_Style);
                    canvas.drawPath(skpath, paint);
                }
                if (path.isStroke()) {
                    pesColor c = path.getStrokeColor();
                    paint.setColor(SkColorSetRGB(c.r, c.g, c.b));
                    paint.setStrokeWidth(path.getStrokeWidth());
                    paint.setStyle(SkPaint::kStroke_Style);
                    canvas.drawPath(skpath, paint);
                }
                skpath.reset();

            }
            canvas.restore();
        }
    }
    
    pesBuffer patternBuffer;
    pesPath patternPath;
};

Pes5PatternPreview & PatternPreview(){
    Pes5PatternPreview *w = new Pes5PatternPreview();
    return *w;
}

Pes5PatternFillDialog::Pes5PatternFillDialog(){
    pPatternPreview = NULL;
    pList = NULL;
    std::string filenames[] = {
        "net1",
        "net2",
        "net3",
        "net4",
        "pat01",
        "pat02",
        "pat03",
        "pat04",
        "pat05",
        "pat06",
        "pat07",
        "pat08",
        "pat09",
        "pat10",
        "pat11",
        "pat12",
        "pat13",
        "pat14",
        "pat15",
        "pat16",
        "pat17",
        "stamp1",
        "stamp2",
        "stamp3",
        "stamp4",
        "stamp5",
        "stamp6",
        "stamp7",
        "stampA",
        "stampB",
        "stampC",
        "stampD",
        "stampE",
        "stampF",
        "stampG",
        "stampH",
        "stampI",
        "stampJ",
        "stampK",
        "stampL",
        "stampM",
        "stampN",
        "stampO",
        "stampP",
        "stampQ",
        "stampR",
        "stampS",
        "stampT",
        "stampU",
        "stampV",
        "stampW",
        "stampX",
        "stampY",
        "stampZ",
        "tatami1",
        "tatami2",
        "tatami3",
        "tatami4",
        "tatami5",
        "wave1",
        "wave2",
        "wave3",
        "wave4",
    };
    
    size(400, 320);
    _( Column()
       .border(false)
       .padding(5, 5, 5, 5)
       ._( Row()
           .border(false)
           .gap(5)
           ._( PatternPreview()
               .size(256, 256)
               .border(true)
               .background(true)
               .bg_color(0xffffff)
               .ref((DecWidget**)&pPatternPreview)
              )
           ._( List()
               .size(-1, 256)
               .ref((DecWidget**)&pList)
               .onValueChanged( [=](DecWidget &widget){
                   int selectedIndex = (int)widget.value();
                   if(pPatternPreview && selectedIndex!=-1){
                       std::string filepath = "Pattern/" + filenames[selectedIndex] + ".pas";
                       //sk_sp<SkData> data = PES5_GetCachePatternAndMotif(filepath.c_str());
                       sk_sp<SkData> data = SkData::MakeWithoutCopy(embeded_pattern::datas[selectedIndex], embeded_pattern::sizes[selectedIndex]);
                       if (data.get()) {
                           pesBuffer buff((const char*)data->data(), data->size());
                           pPatternPreview->patternBuffer = buff;
                           pPatternPreview->patternPath.clear();
                           pPatternPreview->patternPath = pesLoadPASFromBuffer(buff);
                           pPatternPreview->patternPath.setStrokeWidth(1);
                           pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                       } else {
                           PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                               PES5_SetCachePatternAndMotif(filepath.c_str(), data);
                               pesBuffer buff((const char*)data->data(), data->size());
                               pPatternPreview->patternBuffer = buff;
                               pPatternPreview->patternPath.clear();
                               pPatternPreview->patternPath = pesLoadPASFromBuffer(buff);
                               pPatternPreview->patternPath.setStrokeWidth(1);
                               pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                           });
                       }
                   }
               })
              )
          )
       ._( Row()
           .size( -1, 0 )
           .border( false )
           .background( false )
           .gap( 10 )
           ._(Spacer().size( -1, 0 ))
           ._(Button("Cancle", 0, [=](DecButton &btn) {
               onCancel();
           }).size( 100, 0 ))
           ._(Button("OK(Add)", 0, [=](DecButton &btn) {
               onOK();
           }).size( 100, 0 ))
           ._(Spacer().size( -1, 0 ))
          )
       ._( Spacer().size(-1, 10) )
      );
    
    if(pList){
        int cnt = SK_ARRAY_COUNT(filenames);
        for(int i=0; i<cnt; i++){
            pList->_( ListItem(filenames[i].c_str()) );
        }
        pList->select_listItem(0);
    }
}

Pes5PatternFillDialog::~Pes5PatternFillDialog(){
    
}

void Pes5PatternFillDialog::onCancel(){
    hide();
}

void Pes5PatternFillDialog::onOK(){
    if(pPatternPreview && pPatternPreview->patternBuffer.size()){
        int index = PES5_GetObjectSelectedIndex();
        pesData * pes = pesGetDocument()->getDataObject(index).get();
        if(pes){
            pes->parameter.fillType = pesData::FILL_TYPE_PATTERN;
            pes->parameter.fillTypeIndex = 2; // 2=Fill Pattern
            pes->parameter.patternFill.patternIndex = (int)pList->value();
            pes->parameter.patternFill.patternName = pList->text();
            pes->parameter.patternFill.patternBuffer = pPatternPreview->patternBuffer;
            pes->applyFill();
            PES5_UpdatePesImage(index);
        }
    }
    hide();
}

Pes5PatternFillDialog & PatternFillDialog() {
    Pes5PatternFillDialog *w = new Pes5PatternFillDialog();
    return *w;
}


///////////////////////
// MotifFill
///////////////////////
Pes5MotifFillDialog::Pes5MotifFillDialog(){
    pPatternPreview = NULL;
    pList = NULL;
    std::vector<std::string> filenames;
    // pom - crossstitch
//    filenames.push_back("crossstitch");
    
    for(int i=1; i<=90; i++){
        char fn[10];
        sprintf(fn, "motif%03d", i);
        filenames.push_back(fn);
    }
    
    size(400, 320);
    _( Column()
       .border(false)
       .padding(5, 5, 5, 5)
       ._( Row()
           .border(false)
           .gap(5)
           ._( PatternPreview()
               .size(256, 256)
               .border(true)
               .background(true)
               .bg_color(0xffffff)
               .ref((DecWidget**)&pPatternPreview)
              )
           ._( List()
               .size(-1, 256)
               .ref((DecWidget**)&pList)
               .onValueChanged( [=](DecWidget &widget){
                   int selectedIndex = (int)widget.value();
                   if(pPatternPreview && selectedIndex!=-1){
                       std::string filepath = "Pattern/" + filenames[selectedIndex] + ".pmf";
                       //sk_sp<SkData> data = PES5_GetCachePatternAndMotif(filepath.c_str());
                       sk_sp<SkData> data = SkData::MakeWithoutCopy(embeded_motif::datas[selectedIndex], embeded_motif::sizes[selectedIndex]);
                       if (data.get()) {
                           pesBuffer buff((const char*)data->data(), data->size());
                           pPatternPreview->patternBuffer = buff;
                           pPatternPreview->patternPath.clear();
                           pPatternPreview->patternPath = pesLoadPMFFromBuffer(buff);
                           pPatternPreview->patternPath.setStrokeWidth(1);
                           pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                       } else {
                           PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                               PES5_SetCachePatternAndMotif(filepath.c_str(), data);
                               pesBuffer buff((const char*)data->data(), data->size());
                               pPatternPreview->patternBuffer = buff;
                               pPatternPreview->patternPath.clear();
                               pPatternPreview->patternPath = pesLoadPMFFromBuffer(buff);
                               pPatternPreview->patternPath.setStrokeWidth(1);
                               pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                           });
                       }
                   }
               })
              )
          )
       ._( Row()
           .size( -1, 0 )
           .border( false )
           .background( false )
           .gap( 10 )
           ._(Spacer().size( -1, 0 ))
           ._(Button("Cancle", 0, [=](DecButton &btn) {
               onCancel();
           }).size( 100, 0 ))
           ._(Button("OK(Add)", 0, [=](DecButton &btn) {
               onOK();
           }).size( 100, 0 ))
           ._(Spacer().size( -1, 0 ))
          )
       ._( Spacer().size(-1, 10) )
      );
    
    if(pList){
        int cnt = (int)filenames.size();
        for(int i=0; i<cnt; i++){
            pList->_( ListItem(filenames[i].c_str()) );
        }
        pList->select_listItem(0);
    }
}

Pes5MotifFillDialog::~Pes5MotifFillDialog(){
}

void Pes5MotifFillDialog::onCancel(){
    hide();
}

void Pes5MotifFillDialog::onOK(){
    if(pPatternPreview && pPatternPreview->patternBuffer.size()){
        int index = PES5_GetObjectSelectedIndex();
        pesData * pes = pesGetDocument()->getDataObject(index).get();
        if(pes){
            pes->parameter.fillType = pesData::FILL_TYPE_MOTIF;
            pes->parameter.fillTypeIndex = 3; // 3=Fill Motif
            pes->parameter.motifFill.patternIndex = (int)pList->value();
            pes->parameter.motifFill.patternName = pList->text();
            pes->parameter.motifFill.patternBuffer = pPatternPreview->patternBuffer;
            pes->applyFill();
            PES5_UpdatePesImage(index);
        }
    }
    hide();
}

Pes5MotifFillDialog & MotifFillDialog() {
    Pes5MotifFillDialog *w = new Pes5MotifFillDialog();
    return *w;
}


////////////////////////////
// MotifStroke
////////////////////////////
Pes5MotifStrokeDialog::Pes5MotifStrokeDialog(){
    pPatternPreview = NULL;
    pList = NULL;
    std::vector<std::string> filenames;
    for(int i=1; i<=90; i++){
        char fn[10];
        sprintf(fn, "motif%03d", i);
        filenames.push_back(fn);
    }
    
    size(400, 320);
    _( Column()
       .border(false)
       .padding(5, 5, 5, 5)
       ._( Row()
           .border(false)
           .gap(5)
           ._( PatternPreview()
               .size(256, 256)
               .border(true)
               .background(true)
               .bg_color(0xffffff)
               .ref((DecWidget**)&pPatternPreview)
              )
           ._( List()
               .size(-1, 256)
               .ref((DecWidget**)&pList)
               .onValueChanged( [=](DecWidget &widget){
                   int selectedIndex = (int)widget.value();
                   if(pPatternPreview && selectedIndex!=-1){
                       std::string filepath = "Pattern/" + filenames[selectedIndex] + ".pmf";
                       //sk_sp<SkData> data = PES5_GetCachePatternAndMotif(filepath.c_str());
                       sk_sp<SkData> data = SkData::MakeWithoutCopy(embeded_motif::datas[selectedIndex], embeded_motif::sizes[selectedIndex]);
                       if (data.get()) {
                           pesBuffer buff((const char*)data->data(), data->size());
                           pPatternPreview->patternBuffer = buff;
                           pPatternPreview->patternPath.clear();
                           pPatternPreview->patternPath = pesLoadPMFFromBuffer(buff);
                           pPatternPreview->patternPath.setStrokeWidth(1);
                           pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                       } else {
                           PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                               PES5_SetCachePatternAndMotif(filepath.c_str(), data);
                               pesBuffer buff((const char*)data->data(), data->size());
                               pPatternPreview->patternBuffer = buff;
                               pPatternPreview->patternPath.clear();
                               pPatternPreview->patternPath = pesLoadPMFFromBuffer(buff);
                               pPatternPreview->patternPath.setStrokeWidth(1);
                               pPatternPreview->patternPath.setStrokeHexColor(0x0000FF);
                           });
                       }
                   }
               })
              )
          )
       ._( Row()
           .size( -1, 0 )
           .border( false )
           .background( false )
           .gap( 10 )
           ._(Spacer().size( -1, 0 ))
           ._(Button("Cancle", 0, [=](DecButton &btn) {
               onCancel();
           }).size( 100, 0 ))
           ._(Button("OK(Add)", 0, [=](DecButton &btn) {
               onOK();
           }).size( 100, 0 ))
           ._(Spacer().size( -1, 0 ))
          )
       ._( Spacer().size(-1, 10) )
      );
    
    if(pList){
        int cnt = (int)filenames.size();
        for(int i=0; i<cnt; i++){
            pList->_( ListItem(filenames[i].c_str()) );
        }
        pList->select_listItem(0);
    }
}

Pes5MotifStrokeDialog::~Pes5MotifStrokeDialog(){
}

void Pes5MotifStrokeDialog::onCancel(){
    hide();
}

void Pes5MotifStrokeDialog::onOK(){
    if(pPatternPreview && pPatternPreview->patternBuffer.size()){
        int index = PES5_GetObjectSelectedIndex();
        pesData * pes = pesGetDocument()->getDataObject(index).get();
        if(pes){
            pes->parameter.strokeType = pesData::STROKE_TYPE_MOTIF;
            pes->parameter.strokeTypeIndex = 6; // 6=Stroke Motif
            //pes->parameter.strokeTypeIndex = (int) pesData::STROKE_TYPE_MOTIF;
            pes->parameter.motif.patternIndex = (int)pList->value();
            pes->parameter.motif.patternName = pList->text();
            pes->parameter.motif.patternBuffer = pPatternPreview->patternBuffer;
            pes->applyStroke();
            PES5_UpdatePesImage(index);
        }
    }
    hide();
}

Pes5MotifStrokeDialog & MotifStrokeDialog() {
    Pes5MotifStrokeDialog *w = new Pes5MotifStrokeDialog();
    return *w;
}

///////////////////////
// Setting
///////////////////////
Pes5SettingDialog::Pes5SettingDialog(){
    pShowJump = NULL;
    pHoop = NULL;
    pDrawingMode = NULL;
    size(300, 200);
    _( Column()
       .border(false)
       .padding(5, 5, 5, 5)
       ._( Text("Hoop size:") )
       ._( ComboBox()
           ._( ComboBoxItem("10 x 10 cm").value( 100100) )
           ._( ComboBoxItem("3.3 x 7.5 cm").value(33075) )  ._( ComboBoxItem("7.5 x 3.3 cm").value(75033) )
           ._( ComboBoxItem("3.8 x 4.4 cm").value(38044) )  ._( ComboBoxItem("4.4 x 3.8 cm").value(44038) )
           ._( ComboBoxItem("4.1 x 7 cm").value(41070) )    ._( ComboBoxItem("7 x 4.1 cm").value(70041) )
           ._( ComboBoxItem("5 x 5 cm").value(50050) )
           ._( ComboBoxItem("6 x 2 cm").value(60020) )      ._( ComboBoxItem("2 x 6 cm").value(20060) )
           ._( ComboBoxItem("15 x 15 cm").value(150150) )
           ._( ComboBoxItem("15 x 26 cm").value(150260) )   ._( ComboBoxItem("26 x 15 cm").value(260150) )
           ._( ComboBoxItem("13 x 18 cm").value(130180) )   ._( ComboBoxItem("18 x 13 cm").value(180130) )
           ._( ComboBoxItem("18 x 10 cm").value(180100) )   ._( ComboBoxItem("10 x 18 cm").value(100180) )
           ._( ComboBoxItem("20 x 20 cm").value(200200) )
           ._( ComboBoxItem("23.5 x 10 cm").value(235100) ) ._( ComboBoxItem("10 x 23.5 cm").value(100235) )
           ._( ComboBoxItem("26 x 16 cm").value(260160) )   ._( ComboBoxItem("16 x 26 cm").value(160260) )
           ._( ComboBoxItem("30 x 10 cm").value(300100) )   ._( ComboBoxItem("10 x 30 cm").value(100300) )
           ._( ComboBoxItem("36 x 20 cm").value(360200) )   ._( ComboBoxItem("20 x 36 cm").value(200360) )
           ._( ComboBoxItem("36 x 36 cm").value(360360) )
           // sinsim
            ._( ComboBoxItem("7 x 4 cm").value(70040) )         ._( ComboBoxItem("4 x 7 cm").value(40070) )
            ._( ComboBoxItem("8 x 8 cm").value(80080) )
            ._( ComboBoxItem("9 x 9 cm").value(90090) )
            ._( ComboBoxItem("12 x 12 cm").value(120120) )
            ._( ComboBoxItem("12.5 x 12.5 cm").value(125125) )
            ._( ComboBoxItem("13 x 7 cm").value(130070) )       ._( ComboBoxItem("7 x 13 cm").value(70130) )
            ._( ComboBoxItem("17 x 10.5 cm").value(170105) )    ._( ComboBoxItem("10.5 x 17 cm").value(105170) )
            ._( ComboBoxItem("17 x 17 cm").value(170170) )
            ._( ComboBoxItem("19 x 19 cm").value(190190) )
            ._( ComboBoxItem("22 x 22 cm").value(220220) )
            ._( ComboBoxItem("27 x 70 cm").value(270700) )      ._( ComboBoxItem("70 x 27 cm").value(700270) )
            ._( ComboBoxItem("27.5 x 27.5 cm").value(275275) )
            ._( ComboBoxItem("27.5 x 7 cm").value(275070) )     ._( ComboBoxItem("7 x 27.5 cm").value(70275) )
            ._( ComboBoxItem("28 x 20 cm").value(280200) )      ._( ComboBoxItem("20 x 28 cm").value(200280) )
            ._( ComboBoxItem("30 x 30 cm").value(300300) )
            ._( ComboBoxItem("35 x 50 cm").value(350500) )      ._( ComboBoxItem("50 x 35 cm").value(500350) )
            ._( ComboBoxItem("36 x 20 cm").value(360200) )      ._( ComboBoxItem("20 x 36 cm").value(200360) )
            ._( ComboBoxItem("45 x 40 cm").value(450400) )      ._( ComboBoxItem("40 x 45 cm").value(400450) )

           .onValueChanged( [=](DecWidget &widget) {
               int selectedIndex = (int)widget.value();
               int value = (int)pHoop->getChildren()[selectedIndex]->value();

#ifdef __EMSCRIPTEN__
                EM_ASM({
                  let fn = window['setDocViewBoxToFitAppViewPort'];
                  if(fn) {
                    fn($0, true);
                  }
                }, value);
#else
               int hw = value / 1000;
               int hh = value % 1000;
               pesGetDocument()->setHoopSizeInMM(pesVec2f(hw, hh));
               //               PES5_ResetPan();
               //               PES5_ResetZoomScale();

               float docw = GetPesDocumentView()->width();
               float doch = GetPesDocumentView()->height();
               //               SkDebugf("w=%d h=%d\n", docw, doch);
               float sx = docw / (hw * 10);
               float sy = doch / (hh * 10);
               float s = std::min(sx, sy);
               PES5_ResetPan();
               PES5_ResetZoomScale();
               PES5_SetZoomScale(s * 0.95);
#endif        
           })
           .ref((DecWidget **)&pHoop)
          )
       ._( CheckBox("Show jump stitch").ref((DecWidget **)&pShowJump) )
       
      // Drawing Options
      ._( Text("Drawing options:") )
      ._( ComboBox()
          ._( ComboBoxItem("Line") )
          ._( ComboBoxItem("Texture") )
          .ref((DecWidget **)&pDrawingMode)
        )
      
       ._( Spacer().size(-1, 20) )
       ._(Button("OK", 0, [=](DecButton &btn) {
             onOK();
        }).size( 100, 0 ).align(ALIGN_BOTTOMRIGHT)  )
      );
    initUi();
}

Pes5SettingDialog::~Pes5SettingDialog(){
}

void Pes5SettingDialog::onOK(){
    if(pShowJump){
        bool checked = pShowJump->isChecked();
#ifdef __EMSCRIPTEN__
        EM_ASM({
          let key = 'setShowJumpStitch';
          if(window['settings'] && window['settings'][key]) {
            let[v0] = window['settings'][key];
            if (v0 != $0) {
              window['settings'][key] = [$0];
              window['saveSettings']();
              window['setSettings'](key);
            }
          }
        }, checked);
#else
        pesGetDocument()->setShowJumpStitch(checked);
#endif
    }
    if(pDrawingMode){
        bool b = ((int)pDrawingMode->value()==1); // 0=Line  1=Texture
#ifdef __EMSCRIPTEN__
        EM_ASM({
          let key = 'setDrawStitchTexture';
          if(window['settings'] && window['settings'][key]) {
            let[v0] = window['settings'][key];
            if (v0 != $0) {
              window['settings'][key] = [$0];
              window['saveSettings']();
              window['setSettings'](key);
            }
          }
        }, b);
#else
        pesGetDocument()->setDrawStitchTexture(b);
#endif
    }

    PES5_UpdatePesImages();
    PES5_UpdateGUIRightPanel();
    hide();
}

void Pes5SettingDialog::initUi() {
//    if (pHoop) pHoop->select_comboboxItem(0);
    if (pHoop){
        pesVec2f hoop = pesGetDocument()->getHoopSizeInMM();
        int value = (hoop.x * 1000) + hoop.y;
        pHoop->selectByChildValue(value, false);
    }
    if (pShowJump) pShowJump->check(pesGetDocument()->isShowJumpStitch());

    if (pDrawingMode) {
        bool b = pesGetDocument()->isDrawStitchTexture();
        if (b) {
            pDrawingMode->select_comboboxItem(1);
        } else {
            pDrawingMode->select_comboboxItem(0);
        }
    }
}

Pes5SettingDialog & SettingDialog() {
    Pes5SettingDialog *w = new Pes5SettingDialog();
    return *w;
}

///////////////////////
// Outline Shape
///////////////////////
Pes5OutlineShapeDialog::Pes5OutlineShapeDialog(){
    size(300, 170);
    _( Column()
        .border(false)
        .padding(5, 5, 5, 5)
        ._( Text("Outline Shape") )
      
        ._( Spacer().size(-1, 5) )
        ._( Text("Distance(mm.) :") )
        ._( Spinner(1.0, 0.0, 20.0, 0.1).ref((DecWidget**)&pInsetSpinner) )
        ._( Spacer().size(-1, 10) )
        ._( CheckBox("Remove Hole").check(true, false).ref((DecWidget**)&pRemoveHoleCheckBox) )
        ._( Spacer().size(-1, 15) )
      ._( Row()
          .size( -1, 0 )
          .border( false )
          .background( false )
          .gap( 10 )
          ._(Spacer().size( -1, 0 ))
          ._(Button("Cancle", 0, [=](DecButton &btn) {
              onCancel();
          }).size( 100, 0 ))
          ._(Button("OK(Add)", 0, [=](DecButton &btn) {
              onOK();
          }).size( 100, 0 ))
          ._(Spacer().size( -1, 0 ))
         )
      );
}

Pes5OutlineShapeDialog::~Pes5OutlineShapeDialog(){
    
}

void Pes5OutlineShapeDialog::onCancel(){
    hide();
}

void Pes5OutlineShapeDialog::onOK(){
    float inset = pInsetSpinner->value();
    bool removeHole = pRemoveHoleCheckBox->isChecked();
//    SkDebugf("inset=%.2f simplify=%d\n", inset, simplify);
    PES5_AddObjectOutlineShape(inset, removeHole);
    hide();
}

Pes5OutlineShapeDialog &OutlineShapeDialog(){
    Pes5OutlineShapeDialog * w = new Pes5OutlineShapeDialog();
    return *w;
}

}
