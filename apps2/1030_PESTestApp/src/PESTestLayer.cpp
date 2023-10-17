#include "include/core/SkStream.h"

#include "DeclarativeUI.h"
#include "PESTestLayer.h"
#include "tools/Resources.h"
#include <fstream>

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include "pesEngine.h"
#include "PesSystemUtils.hpp"

struct data_t {
    void* widget;
    int id;
};

const char* glassesFilenames[] = {
        "",
        "pes/CHARACTER 1/ACCESSORIES CHARACTER 1.pes",
        "pes/CHARACTER 1/ACCESSORIES CHARACTER 2.pes",
        "pes/CHARACTER 1/ACCESSORIES CHARACTER 3.pes",
        "pes/CHARACTER 1/ACCESSORIES CHARACTER 4.pes",
        "pes/CHARACTER 1/ACCESSORIES CHARACTER 5.pes",
};

const char* bodyFilenames[] = {
        "pes/CHARACTER 1/BODY CHARACTER 01.pes",
        "pes/CHARACTER 1/BODY CHARACTER 02.pes",
        "pes/CHARACTER 1/BODY CHARACTER 03.pes",
        "pes/CHARACTER 1/BODY CHARACTER 04.pes",
        "pes/CHARACTER 1/BODY CHARACTER 05.pes",
        "pes/CHARACTER 1/BODY CHARACTER 06.pes",
        "pes/CHARACTER 1/BODY CHARACTER 07.pes",
        "pes/CHARACTER 1/BODY CHARACTER 08.pes",
        "pes/CHARACTER 1/BODY CHARACTER 09.pes",
        "pes/CHARACTER 1/BODY CHARACTER 10.pes",
        "pes/CHARACTER 1/BODY CHARACTER 11.pes",
        "pes/CHARACTER 1/BODY CHARACTER 12.pes",
        "pes/CHARACTER 1/BODY CHARACTER 13.pes",
        "pes/CHARACTER 1/BODY CHARACTER 14.pes",
        "pes/CHARACTER 1/BODY CHARACTER 15.pes",
        "pes/CHARACTER 1/BODY CHARACTER 16.pes",
};

const char* eyeFilenames[] = {
        "pes/CHARACTER 1/EYES CHARACTER 01.pes", "pes/CHARACTER 1/EYES CHARACTER 02.pes",
        "pes/CHARACTER 1/EYES CHARACTER 03.pes", "pes/CHARACTER 1/EYES CHARACTER 04.pes",
        "pes/CHARACTER 1/EYES CHARACTER 05.pes", "pes/CHARACTER 1/EYES CHARACTER 06.pes",
        "pes/CHARACTER 1/EYES CHARACTER 07.pes", "pes/CHARACTER 1/EYES CHARACTER 08.pes",
        "pes/CHARACTER 1/EYES CHARACTER 09.pes", "pes/CHARACTER 1/EYES CHARACTER 10.pes",
        "pes/CHARACTER 1/EYES CHARACTER 11.pes", "pes/CHARACTER 1/EYES CHARACTER 12.pes",
        "pes/CHARACTER 1/EYES CHARACTER 13.pes", "pes/CHARACTER 1/EYES CHARACTER 14.pes",
        "pes/CHARACTER 1/EYES CHARACTER 15.pes", "pes/CHARACTER 1/EYES CHARACTER 16.pes",
        "pes/CHARACTER 1/EYES CHARACTER 17.pes", "pes/CHARACTER 1/EYES CHARACTER 18.pes",
        "pes/CHARACTER 1/EYES CHARACTER 19.pes", "pes/CHARACTER 1/EYES CHARACTER 20.pes",
};

const char* headFilenames[] = {
        "pes/CHARACTER 1/HEAD CHARACTER 01-D2.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-D3.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-D4.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-D5.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-D6.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-D7.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-D8.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-D9.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-D10.pes", "pes/CHARACTER 1/HEAD CHARACTER 01-D11.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-D12.pes", "pes/CHARACTER 1/HEAD CHARACTER 01-G1.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G2.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-G3.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G4.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-G5.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G6.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-G7.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G8.pes",  "pes/CHARACTER 1/HEAD CHARACTER 01-G9.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G10.pes", "pes/CHARACTER 1/HEAD CHARACTER 01-G11.pes",
        "pes/CHARACTER 1/HEAD CHARACTER 01-G12.pes",
};

const char* mouthFilenames[] = {
        "",
        "pes/CHARACTER 1/MOUTH CHARACTER 01.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 02.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 03.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 04.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 05.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 06.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 07.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 08.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 09.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 10.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 11.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 12.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 13.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 14.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 15.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 16.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 17.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 18.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 19.pes",
        "pes/CHARACTER 1/MOUTH CHARACTER 20.pes",
};

const int numGlasses = SK_ARRAY_COUNT(glassesFilenames);
const int numBody = SK_ARRAY_COUNT(bodyFilenames);
const int numEye = SK_ARRAY_COUNT(eyeFilenames);
const int numHead = SK_ARRAY_COUNT(headFilenames);
const int numMouth = SK_ARRAY_COUNT(mouthFilenames);

int glassesIndex = 0;
int bodyIndex = 0;
int eyeIndex = 0;
int headIndex = 0;
int mouthIndex = 0;

static sk_ui::DecPesWidget* g_pPesWidget = NULL;

namespace sk_ui {

void PesTestApp_UpdateImages(){
    if(g_pPesWidget){
        g_pPesWidget->updatePesImages();
    }
}

void PesTestApp_UpdateTextImage(){
    if(g_pPesWidget){
        g_pPesWidget->updatePesTextImage();
    }
}

DecPesWidget& PesWidget() {
    DecPesWidget* w = new DecPesWidget();
    g_pPesWidget = w;
    return *w;
}

DecPesEditor& PesEditor() {
    DecPesEditor* w = new DecPesEditor();
    return *w;
}

DecPesLayer& PesLayer() {
    DecPesLayer* w = new DecPesLayer();
    return *w;
}

DecPesLayer::DecPesLayer() {
    UI().font(Font().Regular("fonts/Kanit-Regular.ttf")
                      .Bold("fonts/Kanit-Bold.ttf")
                      .Italic("fonts/Kanit-Italic.ttf")
                      .BoldItalic("fonts/Kanit-BoldItalic.ttf")
                      .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
                      .Emoji("fonts/NotoColorEmoji.ttf"))
            .root(
                Column()
                    .size(-1, -1)
                    ._(
                        Row().size(-1, 40)
                                .bg_color(0xFFF0F0F0)
                                .border_color(0xFF808080)
                                ._(Spacer().size(-1, -1))
                                ._(Text("PES Character").fontSize(18).align(ALIGN_CENTER))
                                ._(Spacer().size(-1, -1))
                    )
                    ._(PesEditor().size(-1, -1)
                                .background(false)
                                .border_color(0xFF808080)
                    )
                    //._(Row().size(-1, 20)
                    //            .bg_color(0xFFF0F0F0)
                    //            .border_color(0xFF808080)
                    //)
            );
}

DecPesEditor::DecPesEditor() {
    //size(-1, -1);
    //border(false);
    //background(false);
    
    _pTextHead = nullptr;
    _pTextBody = nullptr;
    _pTextEye = nullptr;
    _pTextMouth = nullptr;
    _pTextGlasses = nullptr;
    
    srand(time(NULL));

    _(PesWidget().cache(true).size(-1, -1).ref((DecWidget**)&_pPesWidget));
    
    DecColumnContainer *container = new DecColumnContainer();

    container->size( 240, -1 );
    container->border( false );
    container->background( false );
    container->gap( 5 );
    container->padding(5, 5, 5, 5);

    container->_(TextEdit("ทดสอบภาษาไทย1",
                          [=](DecTextEdit& textedit) {
                              SkDebugf("PESEditor::_pTextEdit1::callback: %s\n", textedit.text());
                              _pTextEdit2->setText(textedit.text());
                          })
                         .size(-1, 0)
                         .ref((DecWidget**)&_pTextEdit1));

    container->_(TextEdit("ทดสอบภาษาไทย2",
                          [=](DecTextEdit& textedit) {
                              SkDebugf("PESEditor::_pTextEdit2::callback: %s\n", textedit.text());
                              _pTextEdit1->setText(textedit.text());
                          })
                         .size(-1, 0)
                         .ref((DecWidget**)&_pTextEdit2));
    
    
    // Head
    container->_(
        Row()
            .size(-1, 32)
            .gap(5)
            .border(false)
            .background(false)
             ._( Button( "", 0xf060, [=](DecButton &btn) {
                 // do previous
                 if(--headIndex<0) headIndex =0;
                 else reloadPesFiles(1);
             } ).border_color(0xFF808080).gap(0) )
             ._( Spacer().size( -1, 0 ) )
             ._(Text( "Head 1" ).align( ALIGN_CENTER ).size(85, -1).ref((DecWidget**)&_pTextHead) )
             ._( Spacer().size( -1, 0 ) )
             ._( Button( "", 0xf061, [=](DecButton &btn) {
                 // do next
                 if(++headIndex>numHead-1) headIndex = numHead-1;
                 else reloadPesFiles(1);
             }).border_color(0xFF808080).gap(0) )
    );
    
    // Eye
    container->_(
        Row()
            .size(-1, 32)
            .gap(5)
            .border(false)
            .background(false)
             ._( Button( "", 0xf060, [=](DecButton &btn) {
                 // do previous
                 if(--eyeIndex<0) eyeIndex =0;
                 else reloadPesFiles(2);
             } ).border_color(0xFF808080).gap(0) )
             ._( Spacer().size( -1, 0 ) )
             ._(Text("Eye 5").align(ALIGN_CENTER).size(85, -1).ref((DecWidget**)&_pTextEye))
             ._( Spacer().size( -1, 0 ) )
             ._( Button( "", 0xf061, [=](DecButton &btn) {
                 // do next
                 if(++eyeIndex>numEye-1) eyeIndex = numEye-1;
                 else reloadPesFiles(2);
             }).border_color(0xFF808080).gap(0) )
    );
    
    // Mouth
    container->_(
        Row()
            .size(-1, 32)
            .gap(5)
            .border(false)
            .background(false)
             ._( Button( "", 0xf060, [=](DecButton &btn) {
                 // do previous
                 if(--mouthIndex<0) mouthIndex =0;
                 else reloadPesFiles(3);
             } ).border_color(0xFF808080).gap(0) )
             ._( Spacer().size( -1, 0 ) )
             ._(Text( "Mouth 13" ).align( ALIGN_CENTER ).size(85, -1).ref((DecWidget**)&_pTextMouth) )
             ._( Spacer().size( -1, 0 ) )
             ._( Button( "", 0xf061, [=](DecButton &btn) {
                 // do next
                 if(++mouthIndex>numMouth-1) mouthIndex = numMouth-1;
                 else reloadPesFiles(3);
             }).border_color(0xFF808080).gap(0) )
    );
    
    // Glasses
    container->_(
        Row()
            .size(-1, 32)
            .gap(5)
            .border(false)
            .background(false)
             ._( Button( "", 0xf060, [=](DecButton &btn) {
                 // do previous
                 if(--glassesIndex<0) glassesIndex =0;
                 else reloadPesFiles(4);
             } ).border_color(0xFF808080).gap(0) )
             ._( Spacer().size( -1, 0 ) )
             ._(Text( "Glasses 2" ).align( ALIGN_CENTER ).size(85, -1).ref((DecWidget**)&_pTextGlasses) )
             ._( Spacer().size( -1, 0 ) )
             ._( Button( "", 0xf061, [=](DecButton &btn) {
                 // do next
                 if(++glassesIndex>numGlasses-1) glassesIndex = numGlasses-1;
                 else reloadPesFiles(4);
             }).border_color(0xFF808080).gap(0) )
    );
    
    // Body
    container->_(
        Row()
            .size(-1, 32)
            .gap(5)
            .border(false)
            .background(false)
             ._( Button( "", 0xf060, [=](DecButton &btn) {
                 // do previous
                 if(--bodyIndex<0) bodyIndex =0;
                 else reloadPesFiles(0);
             } ).border_color(0xFF808080).gap(0) )
             ._( Spacer().size( -1, 0 ) )
             ._(Text( "Body 1" ).align( ALIGN_CENTER ).size(85, -1).ref((DecWidget**)&_pTextBody) )
             ._( Spacer().size( -1, 0 ) )
             ._( Button( "", 0xf061, [=](DecButton &btn) {
                 // do next
                 if(++bodyIndex>numBody-1) bodyIndex = numBody-1;
                 else reloadPesFiles(0);
             }).border_color(0xFF808080).gap(0) )
    );
    
    // Shuffle
    container->_(
         Button( "Random", 0xf0d0, [=](DecButton &btn) {
             // do shuffle
             bodyIndex = rand() % numBody;
             headIndex = rand() % numHead;
             eyeIndex = rand() % numEye;
             mouthIndex = rand() % numMouth;
             glassesIndex = rand() % numGlasses;

             if (_pPesWidget) {
                 _pPesWidget->reloadPesObject(0);
                 _pPesWidget->reloadPesObject(1);
                 _pPesWidget->reloadPesObject(2);
                 _pPesWidget->reloadPesObject(3);
                 _pPesWidget->reloadPesObject(4);
             }             

             updateLabels();

         } ).border_color(0xFF808080).gap(0).size(-1, 32)
    );
    
    container->_(
         Button( "Export", 0xf08b, [=](DecButton &btn) {
             // do export pes file
#ifdef __EMSCRIPTEN__
             // do javascript popup download inside EM_ASM()
             EM_ASM(
                 var pesbuff = CanvasKit['pesDocument']['getInstance']()['getPES5Buffer'](true);
                 var arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
                 var blob = new Blob([arraybuff]);
                 var a = document.createElement("a");
                 document.body.appendChild(a);
                 a.href = window.URL.createObjectURL(blob);
                 a.download = "character.pes";
                 a.onclick = function () {
                     setTimeout(function () {
                         window.URL.revokeObjectURL(a.href);
                         document.body.removeChild(a);
                     }, 1500);
                 };
                 a.click();
             );
#else
             PesFileDialogResult result = PesSystemSaveDialog("character.pes", "Export PES File");
             if(result.bSuccess){
                 _pPesWidget->exportPES(result.getPath());
             }
#endif
         } ).border_color(0xFF808080).gap(0).size(-1, 32)
    );

    _(*container);
    
    updateLabels();
}

DecPesEditor::~DecPesEditor() {}

void DecPesEditor::onResized(int w, int h) {
    SkDebugf("DecPesEditor::onResized:[%d, %d], fWigetSize:[%d, %d], [_width, _height]:[%d, %d]\n", w, h, (int)fWigetSize.fWidth, (int)fWigetSize.fHeight, _width, _height);
    fWigetSize.set(w, h);
    // fixed: pes widget not update on window height change.
    _pPesWidget->dirty(true, true);
}

void DecPesEditor::adjustWidth(int w) {
    if (_ow == 0 || _ow == -1) {
        if (fWigetSize.fWidth > 0) {
            // fixed: cache & dirty
            _width = fWigetSize.fWidth;
        } else {
            fWigetSize.fWidth = _width;
        }
        if (w != _width) {
            dirty(true, true);
            _width = w;
            // fixed: cache & dirty
            fWigetSize.fWidth = w;
        }
        if (_width > _maxWidth) {
            _width = _maxWidth;
            // fixed: cache & dirty
            fWigetSize.fWidth = _maxWidth;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecPesEditor::adjustHeight(int h) {
    if (_oh == 0 || _oh == -1) {
        if (fWigetSize.fHeight > 0) {
            // fixed: cache & dirty
            _height = fWigetSize.fHeight;
        } else {
            fWigetSize.fHeight = _height;
        }
        if (h != _height) {
            dirty(true, true);
            _height = h;
            // fixed: cache & dirty
            fWigetSize.fHeight = h;
        }
        if (_height > _maxHeight) {
            _height = _maxHeight;
            // fixed: cache & dirty
            fWigetSize.fHeight = _maxHeight;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecPesEditor::reloadPesFiles(int id /*= -1*/) {
    const std::string bodyFilepath = bodyFilenames[bodyIndex];
    const std::string headFilepath = headFilenames[headIndex];
    const std::string eyeFilepath = eyeFilenames[eyeIndex];
    const std::string mouthFilepath = mouthFilenames[mouthIndex];
    const std::string glassesFilepath = glassesFilenames[glassesIndex];

    if (id > -1) {
        _pPesWidget->reloadPesObject(id);
        //_pPesWidget->reloadPesObjectAsync(id); 
    } else {
        _pPesWidget->reloadFiles(bodyFilepath, headFilepath, eyeFilepath, mouthFilepath, glassesFilepath);
    }
    
    updateLabels();
}

void DecPesEditor::updateLabels(){
    char strLabel[32];
    strLabel[0] = 0;
    sprintf(strLabel, "Head %i/%i", (headIndex+1), numHead);
    if(_pTextHead){
        _pTextHead->setText(strLabel);
        _pTextHead->dirty();
    }
    
    sprintf(strLabel, "Body %i/%i", (bodyIndex+1), numBody);
    if(_pTextBody){
        _pTextBody->setText(strLabel);
        _pTextBody->dirty();
    }
    
    sprintf(strLabel, "Eye %i/%i", (eyeIndex+1), numEye);
    if(_pTextEye){
        _pTextEye->setText(strLabel);
        _pTextEye->dirty();
    }
    
    sprintf(strLabel, "Mouth %i/%i", (mouthIndex+1), numMouth);
    if(_pTextMouth){
        _pTextMouth->setText(strLabel);
        _pTextMouth->dirty();
    }
    
    sprintf(strLabel, "Glasses %i/%i", (glassesIndex+1), numGlasses);
    if(_pTextGlasses){
        _pTextGlasses->setText(strLabel);
        _pTextGlasses->dirty();
    }
}

DecPesWidget::DecPesWidget() {
    //size(-1, -1);
    //border(true);
    //background(true);
    //cache(true);

    reset();


//    // Character
//    importPES("pes/CHARACTER 1/BODY CHARACTER 01.pes");
//    importPES("pes/CHARACTER 1/HEAD CHARACTER 01-D2.pes");
//    importPES("pes/CHARACTER 1/EYES CHARACTER 05.pes");
//    importPES("pes/CHARACTER 1/MOUTH CHARACTER 13.pes");
//    importPES("pes/CHARACTER 1/ACCESSORIES CHARACTER 2.pes");
//      
//    auto body = pesGetDocument()->getDataObject(0);
//    auto head = pesGetDocument()->getDataObject(1);
//    auto eye = pesGetDocument()->getDataObject(2);
//    auto mouse = pesGetDocument()->getDataObject(3);
//    auto glasses = pesGetDocument()->getDataObject(4);
//    
//    head->translate(0, -500);
//    eye->translate(0, -420);
//    mouse->translate(0, -300);
//    glasses->translate(0, -420);
//    
//    updatePesImages();

    // create pes's objects
    pesGetDocument()->addObject(pesData());
    pesGetDocument()->addObject(pesData());
    pesGetDocument()->addObject(pesData());
    pesGetDocument()->addObject(pesData());
    pesGetDocument()->addObject(pesData());
    // id=5 pes font object
    pesGetDocument()->addObject(pesData());

    // create pes's images
    updatePesImages();

    // reload pes's resources
    reloadPesObject(0);
    reloadPesObject(1);
    reloadPesObject(2);
    reloadPesObject(3);
    reloadPesObject(4);
    
    // load pes font for pes object id=5
    insertPPEFText();
}

DecPesWidget::~DecPesWidget() {}


void DecPesWidget::onResized(int w, int h) {
    SkDebugf("DecPesWidget::onResized:[%d, %d], fWigetSize:[%d, %d], [_width, _height]:[%d, %d]\n", w, h, (int)fWigetSize.fWidth, (int)fWigetSize.fHeight, _width, _height);
    fWigetSize.set(w, h);
    //dirty(true, true);
}

void DecPesWidget::adjustWidth(int w) {
    if (_ow == 0 || _ow == -1) {
        // fixed: cache & dirty
        if (fWigetSize.fWidth > 0) {
            // fixed: cache & dirty
            _width = fWigetSize.fWidth;
        } else {
            fWigetSize.fWidth = _width;
        }
        if (w != _width) {
            dirty(true, true);
            _width = w;
            // fixed: cache & dirty
            fWigetSize.fWidth = w;
        }
        if (_width > _maxWidth) {
            _width = _maxWidth;
            // fixed: cache & dirty
            fWigetSize.fWidth = _maxWidth;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecPesWidget::adjustHeight(int h) {
    if (_oh == 0 || _oh == -1) {
        if (fWigetSize.fHeight > 0) {
            // fixed: cache & dirty
            _height = fWigetSize.fHeight;
        } else {
            fWigetSize.fHeight = _height;
        }
        if (h != _height) {
            dirty(true, true);
            _height = h;
            // fixed: cache & dirty
            fWigetSize.fHeight = h;
        }
        if (_height > _maxHeight) {
            _height = _maxHeight;
            // fixed: cache & dirty
            fWigetSize.fHeight = _maxHeight;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecPesWidget::reset() {
    pesGetDocument()->newDocument();
}

void DecPesWidget::updatePesImages(){
    pesImages.clear();
    auto pesDoc = pesGetDocument();
    int cnt = pesDoc->getObjectCount();
    for(int i=0; i<cnt; i++){
        if(pesDoc->getDataObject(i)->isScalable()){
            pesImages.push_back(pesDoc->makePesScalableImageSnapshot(i));
        }
        else{
            pesImages.push_back(pesDoc->makePesImageSnapshot2(i));
        }
    }
    dirty();
}

void DecPesWidget::updatePesTextImage(){
    auto pesDoc = pesGetDocument();
    pesImages[5] = pesDoc->makePesImageSnapshot2(5);
    dirty();
}

void DecPesWidget::loadPPES(const std::string & path){
    sk_sp<SkData> data = GetResourceAsData(path.c_str());
    pesBuffer buff((const char*)data->data(), data->size());
    pesGetDocument()->loadPPESFromBuffer(buff);
}

void DecPesWidget::savePPES(const std::string & path){
    pesBuffer buffer = pesGetDocument()->getPPESBuffer();
    if(buffer.size()){
        std::fstream fs;
        fs.open(path.c_str(), std::ios::out | std::ios::binary);
        fs.write(buffer.getData(), buffer.size());
    }
}

bool DecPesWidget::importPES(const std::string & path){
    sk_sp<SkData> data = GetResourceAsData(path.c_str());
    pesBuffer buff((const char*)data->data(), data->size());
    pesData pes;
    if(pes.loadPESFromBuffer(buff)){
        pesGetDocument()->addObject(pes);
        return true; // load success
    }
    return false; // load fail
}

void DecPesWidget::exportPES(const std::string & path){
    bool orderByColor = true;
    pesBuffer buffer = pesGetDocument()->getPES5Buffer(orderByColor);
    if(buffer.size()){
        std::fstream fs;
        fs.open(path.c_str(), std::ios::out | std::ios::binary);
        fs.write(buffer.getData(), buffer.size());
    }
}

void DecPesWidget::reloadFiles(const std::string & bodyFilepath, const std::string & headFilepath,
                               const std::string & eyeFilepath, const std::string & mouthFilepath,
                               const std::string & glassesFilepath)
{
    reset();
    
//    head->translate(0, -500);
//    eye->translate(0, -420);
//    mouse->translate(0, -300);
//    glasses->translate(0, -420);
    
    if(importPES(bodyFilepath)){
        auto body = pesGetDocument()->getDataObjects().back();
        body->translate(0, 0);
    }
    if(importPES(headFilepath)){
        auto head = pesGetDocument()->getDataObjects().back();
        head->translate(0, -500);
    }
    if(importPES(eyeFilepath)){
        auto eye = pesGetDocument()->getDataObjects().back();
        eye->translate(0, -420);
    }
    if(importPES(mouthFilepath)){
        auto mouth = pesGetDocument()->getDataObjects().back();
        mouth->translate(0, -300);
    }
    if(importPES(glassesFilepath)){
        auto glassed = pesGetDocument()->getDataObjects().back();
        glassed->translate(0, -420);
    }
    
    updatePesImages();
}


void DecPesWidget::reloadPesObject(int id, sk_sp<SkData> data) {
    if (id > -1) {
        if (data == nullptr) {
            const char* resources[] = {
                    bodyFilenames[bodyIndex],
                    headFilenames[headIndex],
                    eyeFilenames[eyeIndex],
                    mouthFilenames[mouthIndex],
                    glassesFilenames[glassesIndex],
            };

            const char* resource = resources[id];

            // just clear, ie. none-glasses
            if (strcmp(resource, "") == 0 || strcmp(resource, "none.pes") == 0) {
                std::shared_ptr<pesData> pes = pesGetDocument()->getDataObject(id);
                pes->clear();
                if (pes->isScalable()) {
                    pesImages[id] = pesGetDocument()->makePesScalableImageSnapshot(id);
                } else {
                    pesImages[id] = pesGetDocument()->makePesImageSnapshot2(id);
                }
                if (asyncReadyAll(-1))
                    dirty();
                return;
            }

#ifdef __EMSCRIPTEN__
            data_t* pdata = new data_t();
            pdata->widget = this;
            pdata->id = id;

            emscriptenDownloadAsset(
                pdata,
                resource,
                [](struct emscripten_fetch_t* fetch) {
                    data_t* pdata = (data_t*)fetch->userData;
                    DecPesWidget* widget = (DecPesWidget*)pdata->widget;
                    int id = pdata->id;
                    delete pdata;

                    sk_sp<SkData> data = SkData::MakeWithCopy(fetch->data, fetch->numBytes);
                    if (data == nullptr) {
                        if (widget->asyncReadyAll(id)) 
                            widget->dirty();
                        return;
                    }
                    widget->reloadPesObject(id, data);
                },
                [](struct emscripten_fetch_t* fetch) {
                    data_t* pdata = (data_t*)fetch->userData;
                    DecPesWidget* widget = (DecPesWidget*)pdata->widget;
                    int id = pdata->id;
                    delete pdata;
                    SkDebugf("Download failed\n");
                    if (widget->asyncReadyAll(id)) 
                        widget->dirty();
                }
            );
            reloadPesObjectAsync(id, data);
            return;
#else
            data = GetResourceAsData(resource);
            // data = SkData::MakeWithCopy(data->data(), data->size());
            reloadPesObjectAsync(id, data);
            return;
#endif            
        }

        pesBuffer buff((const char*)data->data(), data->size());
        std::shared_ptr<pesData> pes = pesGetDocument()->getDataObject(id);
        if (pes->loadPESFromBuffer(buff)) {
            float offsetX = 0;
            float offsetY = 0;

            if (id == 0) {
                if (bodyIndex == 0) {
                    offsetX = 5.0f;
                } else if (bodyIndex == 1) {
                    offsetX = 5.0f;
                } else if (bodyIndex == 2) {
                    offsetX = 5.0f;
                } else if (bodyIndex == 3) {
                    offsetX = 35.0f;
                } else if (bodyIndex == 11) {
                    offsetX = 15.0f;
                } else if (bodyIndex > 11) {
                    offsetX = 5.0f;
                }
            } else if (id == 1) {
                if (headIndex == 4) {
                    offsetX = 25.0f;
                } else if (headIndex == 11 || headIndex == 12) {
                    offsetY = 115.0f;
                } else if (headIndex == 14) {
                    offsetX = 70.0f;
                    offsetY = 30;
                } else if (headIndex == 18) {
                    offsetY = -55.0f;
                }
            }

            static const float aTranslateY[] = {0, -500, -420, -300, -400};
            pes->translate(0 + offsetX, aTranslateY[id] + offsetY);

            if (pes->isScalable()) {
                pesImages[id] = pesGetDocument()->makePesScalableImageSnapshot(id);
            } else {
                pesImages[id] = pesGetDocument()->makePesImageSnapshot2(id);
            }

            if(asyncReadyAll(id))
                dirty();
        
            auto b = pes->getBoundingBox();
            SkDebugf("object:%d, bounding:[%.02f, %.02f, %.02f, %.02f]\n", id, b.x, b.y, b.width, b.height);
        }
    }
}

void DecPesWidget::reloadPesObjectAsync(int id, sk_sp<SkData> data) {
#ifdef __EMSCRIPTEN__
    if (tasks < 0) 
        tasks = 0;
    ++tasks;
#else
    futWorkerTasks[id] = std::async(std::launch::async, &DecPesWidget::reloadPesObject, this, id, data);
#endif
}

bool DecPesWidget::asyncReadyAll(int skip) {
#ifdef __EMSCRIPTEN__
    if (skip >= 0 && skip < 5) 
        --tasks;
    if (tasks < 0) 
        tasks = 0;
    return tasks == 0;
#else
    const int tasks = SK_ARRAY_COUNT(futWorkerTasks);
    for (int i = 0; i < tasks; i++) {
        if (i == skip) 
            continue;
        const std::future<void>& task = futWorkerTasks[i];
        if (task.valid()) {
            if (std::future_status::ready != task.wait_for(0ns)) {
                return false;
            }
        }
    }
    return true;
#endif
}

void DecPesWidget::_draw_content(SkCanvas& canvas) {
    if (!_isVisible) {
        return;
    }

    const int cnt = pesImages.size();
    if (cnt <= 0) {
        return;
    }
    pesRectangle bounds = pesGetDocument()->getDataObject(0)->getBoundingBox();
    for (int i = 1; i < cnt; i++) {
        bounds.growToInclude(pesGetDocument()->getDataObject(i)->getBoundingBox());
    }

    const float padding = 10.0f;
    const float padding2 = padding * 2.0f;
    const float w = _width - padding2;
    const float h = _height - padding2;
    const float scale = std::min(w / bounds.width, h / bounds.height);

    const float bodyH = pesGetDocument()->getDataObject(0)->getBoundingBox().height;
    const float moveY = (bounds.height - bodyH) / 2.0f;

    canvas.save();
    {
        canvas.translate(_width / 2, _height / 2 + scale * moveY);
        canvas.scale(scale, scale);
        for (int i = 0; i < cnt; i++) {
            canvas.save();
            {
                const auto img = pesImages[i];
                const pesRectangle bound = pesGetDocument()->getDataObject(i)->getBoundingBox();
                SkRect dst = SkRect::MakeLTRB(-bound.width/2, -bound.height/2, bound.width/2, bound.height/2);
                canvas.translate(bound.getCenter().x, bound.getCenter().y);
                const float rotateDegree = pesGetDocument()->getDataObject(i)->parameter.rotateDegree;
                canvas.rotate(rotateDegree);
                canvas.drawImageRect(img, dst, SkSamplingOptions());
            }
            canvas.restore();
        }
    }
    canvas.restore();
}

void DecPesWidget::insertPPEFText(){
#ifdef __EMSCRIPTEN__
             // do load ppef using SQLJS inside EM_ASM()
             EM_ASM(
                    var pesResourceProvider = new window['PesResourceProvider']();
//                    pesResourceProvider.loadResource('PPEF/Thai001.ppef', '/resources/PPEF/Thai001.ppef',
//                                        function(buffer){
//                                            // open ppef
//                                            var uints = new window['Uint8Array'](buffer);
//                                            var defaultPPEF = new window['SQLJS']();
//                                            defaultPPEF['openPPEF'](uints);
//                                            defaultPPEF['readPPEFConfig']();
//
//                                            // query
//                                            var text = "ภิญญ์จักรปัก";
//                                            var shapes = defaultPPEF['getStringAsShapes'](text, 0, 0);
//                                            if(shapes['size']()){
//                                                var pes = CanvasKit['pesDocument']['getInstance']()['getDataObject'](5);
//                                                pes['clear']();
//                                                pes['paths'] = shapes;
//                                                var param = CanvasKit['pesFactory']['MakeParam']();
//                                                param['setType'](CanvasKit['PesObjectType']['OBJECT_TYPE_SCALABLE_PPEF_TEXT']);
//                                                param['text'] = text;
//                                                param['isPPEFPath'] = true;
//                                                param['fontName'] = "PPEF/Thai001.ppef";
//                                                param['fillColorIndex'] = 11;
//                                                param['fillType'] = CanvasKit['PesFillType']['FILL_TYPE_SATIN_COLUMN'];
//                                                param['strokeType'] = CanvasKit['PesStrokeType']['STROKE_TYPE_NONE'];
//                                                pes['parameter'] = param;
//                                                const unit_per_mm = 10;
//                                                var s = (1/300) * (pes['parameter']['fontSize']*unit_per_mm);
//                                                pes['scale'](s, s);
//
//                                                // reset cumulative scale
//                                                param = pes['parameter'];
//                                                param['lastFontSize'] = param['fontSize'];
//                                                param['ppefScaleX'] = param['ppefScaleY'] = 1;
//                                                pes['parameter'] = param;
//
//                                                pes['applyPPEFFill']();
//                                                var center = pes['getBoundingBox']()['getCenter']();
//                                                pes['translate'](-center.x, - center.y);
//                                            }
//
//                                            // update image
//                                            CanvasKit['PesTestApp']['UpdateTextImage']();
//                                        });
                    
                    var url = '/resources/PPEF/Thai001.ppef';
                    fetch(url)
                    .then((response) => response.arrayBuffer())
                    .then((buffer) => {
                        // open ppef
                        var uints = new Uint8Array(buffer);
                        var defaultPPEF = new window['SQLJS']();
                        defaultPPEF['openPPEF'](uints);
                        defaultPPEF['readPPEFConfig']();
                        
                        // query
                        var text = "ภิญญ์จักรปัก";
                        var shapes = defaultPPEF['getStringAsShapes'](text, 0, 0);
                        if(shapes['size']()){
                            var pes = CanvasKit['pesDocument']['getInstance']()['getDataObject'](5);
                            pes['clear']();
                            pes['paths'] = shapes;
                            var param = CanvasKit['pesFactory']['MakeParam']();
                            param['setType'](CanvasKit['PesObjectType']['OBJECT_TYPE_SCALABLE_PPEF_TEXT']);
                            param['text'] = text;
                            param['isPPEFPath'] = true;
                            param['fontName'] = "PPEF/Thai001.ppef";
                            param['fillColorIndex'] = 11;
                            param['fillType'] = CanvasKit['PesFillType']['FILL_TYPE_SATIN_COLUMN'];
                            param['strokeType'] = CanvasKit['PesStrokeType']['STROKE_TYPE_NONE'];
                            pes['parameter'] = param;
                            const unit_per_mm = 10;
                            var s = (1/300) * (pes['parameter']['fontSize']*unit_per_mm);
                            pes['scale'](s, s);
                            
                            // reset cumulative scale
                            param = pes['parameter'];
                            param['lastFontSize'] = param['fontSize'];
                            param['ppefScaleX'] = param['ppefScaleY'] = 1;
                            pes['parameter'] = param;

                            pes['applyPPEFFill']();
                            var center = pes['getBoundingBox']()['getCenter']();
                            pes['translate'](-center.x, - center.y);
                            var pesBody = CanvasKit['pesDocument']['getInstance']()['getDataObject'](0);
                            var cy = pesBody['getBoundingBox']()['y'] + pesBody['getBoundingBox']()['height']
                                     + pes['getBoundingBox']()['height']/2;
                            pes['translate'](0, cy);
                        }
                        
                        // update image
                        CanvasKit['PesTestApp']['UpdateTextImage']();
                    });
                    
             );
#else
             // do load ppef using SQLiteCPP
#endif
}


}
