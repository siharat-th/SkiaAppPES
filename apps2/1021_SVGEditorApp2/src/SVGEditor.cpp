#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "SVGDomEditor2.h"
#include "SVGEditor.h"
#include "SVGTreeItem.h"
#include "PesSystemUtils.hpp"
// for SK_ARRAY_COUNT
#include "include/core/SkTypes.h"
// for test annotation
#include <src/core/SkAnnotationKeys.h>

#define DEBUG_NODE = 1;

const int paperWidth[] = { 52, 74, 105, 148, 210, 297, 420, 594, 841 };
const int paperHeight[] = { 74, 105, 148, 210, 297, 420, 594, 841, 1188 };
const char *paperName[] = { "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", "n/a" };
const int numPaperType = 9;

const char *tagName[] = {
    "Circle",
    "ClipPath",
    "Defs",
    "Ellipse",
    "FeBlend",
    "FeColorMatrix",
    "FeComposite",
    "FeDiffuseLighting",
    "FeDisplacementMap",
    "FeDistantLight",
    "FeFlood",
    "FeGaussianBlur",
    "FeImage",
    "FeMorphology",
    "FeOffset",
    "FePointLight",
    "FeSpecularLighting",
    "FeSpotLight",
    "FeTurbulence",
    "Filter",
    "G",
    "Image",
    "Line",
    "LinearGradient",
    "Mask",
    "Path",
    "Pattern",
    "Polygon",
    "Polyline",
    "RadialGradient",
    "Rect",
    "Stop",
    "Svg",
    "Text",
    "TextLiteral",
    "TextPath",
    "TSpan",
    "Use"
};

const char *visibilityType[] = {
    "Visible",
    "Hidden",
    "Collapse",
    "Inherit"
};

DecSVGEditor::DecSVGEditor() {
    _isFocusable = true;
    _border = true;
    _borderWidth = 1;
    _border_color = 0xFF808080;
    _focus_borderWidth = 2;
    _focus_border_color = SK_ColorGREEN;

    fDom = NULL;
    root = NULL;
    _pTextDocumentSize = NULL;

    size( -1, -1 );
    border( false );
    background( false );

    //_( Container()
    //    .size( 24, -1 )
    //    .border(false)
    //    .background(false)
    //);
    // 
    //._( SVGEditor( "resources/svg/HBAN-01.svg") );
    //._( SVGEditor( "resources/svg/MagicPattern-01.svg") );
    //_(SVGDomEditor("resources/svg/Blouse-01.svg").ref((DecWidget**)&pSVGDomEditor).cache(true));
    _( SVGDomEditor().ref((DecWidget**)&pSVGDomEditor).cache(true));
    _( FileDialogBox()
        .ref( (DecWidget **)&_pFileDialog )
    );
    _( Column()
        .size( 360, -1 )
        .border(false)
        .background(false)
        .gap( 10 )
        .padding( 10, 10, 10, 10 ) 
        
        ._( Container()
            .size( -1, 360 )
            .border_color( 0xFF808080 )
            ._( Tree()
                .border( false )
                .ref( (DecWidget **)&pNodeTree )
            )
        )
        ._( Text( "" )
            .ref( (DecWidget **)&_pTextDocumentSize )
        )
        ._( Column()
            .size( -1, 0 )
            .border( false )
            .gap( 5 )
            ._( Text( "Load: " ) )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("Load SVG", 0, [=]( DecButton &btn ) {
                    SkDebugf( "Load SVG\n" );
                    OpenSVGFile();
                }))
            )
            ._( Text( "Export: " ).margin_top( 5 ) )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("Save SVG", 0, [=]( DecButton &btn ) {
                    SkDebugf( "SvgNode to String\n" );
                    saveSVGFile();
                }))
                ._( Button("Export SVG", 0, [=]( DecButton &btn ) {
                    SkDebugf( "Export SVG\n" );
                    exportSVGFile();
                }))
                ._( Button("Test PDF Link", 0, [=]( DecButton &btn ) {
                    SkDebugf( "Test PDF Link\n" );
                    SkDynamicMemoryWStream outStream;
                    auto doc = SkPDF::MakeDocument(&outStream);

                    SkFont font(_font->typeface(0), 25);
                    SkFontMetrics metrics;
                    font.getMetrics(&metrics);
                    float fontHeight = -metrics.fAscent + metrics.fDescent;

                    auto fnDrawTextRectReturnBound = [&](SkCanvas* canvas, const char* pStr, SkRect& r, bool bDrawRect=true) {
                        const float x = r.x();
                        const float y = r.y();
                        SkPaint paint;
                        paint.setColor(SK_ColorBLACK);
                        canvas->drawString(pStr, x, y, font, paint);
                        int ww = font.measureText(pStr, strlen(pStr), SkTextEncoding::kUTF8, &r);
                        r.fLeft =   x + r.fLeft    + 0;
                        r.fRight =  x + r.fRight   + 0;
                        r.fTop =    y + r.fTop     + 0;
                        r.fBottom = y + r.fBottom  + 0;

                        if (bDrawRect) {
                            auto rect = r;
                            rect.outset(4, 4);

                            paint.setColor(0xC000A0FF);
                            paint.setStyle(SkPaint::Style::kStroke_Style);
                            paint.setStrokeWidth(1.0f);
                            canvas->drawRect(rect, paint);
                        }
                    };
                    
                    {
                        // first page content link named
                        SkCanvas* canvas = doc->beginPage(612.0f, 792.0f);

                        SkRect r = SkRect::MakeXYWH(0, 0, 0, 0);

                        r.offset(72, 72);
                        fnDrawTextRectReturnBound(canvas, "Target example1", r, false);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        sk_sp<SkData> example1(SkData::MakeWithCString("example1"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::Define_Named_Dest_Key(), example1.get());

                        r.offset(0, 96);
                        fnDrawTextRectReturnBound(canvas, "Link to example2", r);
                        sk_sp<SkData> example2(SkData::MakeWithCString("example2"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::Link_Named_Dest_Key(), example2.get());

                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to URL: #nameddest=example2", r);
                        sk_sp<SkData> urlexample2(SkData::MakeWithCString("#nameddest=example2"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::URL_Key(), urlexample2.get());

                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to URL: #page=2", r);
                        sk_sp<SkData> urlpage2(SkData::MakeWithCString("#page=2"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::URL_Key(), urlpage2.get());

                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to URL: pdflink.htm", r);
                        sk_sp<SkData> urlpdflink(SkData::MakeWithCString("pdflink.htm"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::URL_Key(), urlpdflink.get());

                        doc->endPage();
                    }
                    {
                        // next page content destination named
                        SkCanvas* canvas = doc->beginPage(612.0f, 792.0f);

                        SkRect r = SkRect::MakeXYWH(0, 0, 0, 0);

                        r.offset(72, 72);
                        fnDrawTextRectReturnBound(canvas, "Target example2", r, false);
                        sk_sp<SkData> example2(SkData::MakeWithCString("example2"));
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        canvas->drawAnnotation(r, SkAnnotationKeys::Define_Named_Dest_Key(), example2.get());
                        
                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to example1", r);
                        sk_sp<SkData> example1(SkData::MakeWithCString("example1"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::Link_Named_Dest_Key(), example1.get());

                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to URL: #nameddest=example1", r);
                        sk_sp<SkData> urlexample1(SkData::MakeWithCString("#nameddest=example1"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::URL_Key(), urlexample1.get());

                        r.offset(0, 96);
                        r.setXYWH(r.x(), r.y(), 0, 0);
                        fnDrawTextRectReturnBound(canvas, "Link to URL: #page=1", r);
                        sk_sp<SkData> urlpage1(SkData::MakeWithCString("#page=1"));
                        canvas->drawAnnotation(r, SkAnnotationKeys::URL_Key(), urlpage1.get());
                    }

                    // end document, close 
                    doc->close();

                    // make stream data buffer
                    sk_sp<SkData> data = outStream.detachAsData();

                    {
                        // save stream data buffer to pdflink.pdf file
                        char filename[] = "resources/pdf/pdflink.pdf";

#ifdef __EMSCRIPTEN__
                        std::filesystem::path _path = std::filesystem::path(filename);
                        std::string strFilename = _path.filename();
                        if (strFilename.empty()) return;

                        EM_ASM(
                        {
                            var pbytes = $0;
                            var byteslen = $1;
                            var dataUint8Array = CanvasKit['decUtility']['getBytes'](pbytes, byteslen);
                            var blob = new Blob([dataUint8Array]);
                            var a = document.createElement("a");
                            document.body.appendChild(a);
                            a.href = window.URL.createObjectURL(blob);
                            a.download = UTF8ToString($2);
                            a.onclick = function() {
                                setTimeout(
                                        function() {
                                            window.URL.revokeObjectURL(a.href);
                                            document.body.removeChild(a);
                                        },
                                        1500);
                            };
                            a.click();
                        },
                        data->bytes(),
                        data->size(),
                        strFilename.c_str());
#else
                        std::filesystem::path _path = std::filesystem::path(filename);
                        if (_path.has_parent_path()) {
                            std::string str_path_dir = _path.parent_path().string();
                            if (!std::filesystem::exists(str_path_dir)) {
                                if (!std::filesystem::create_directories(str_path_dir)) {
                                    SkDebugf(
                                            "Could not open export file: error on create "
                                            "directories.\n");
                                    return;
                                }
                            }
                        }

                        SkFILEWStream file(filename);
                        if (!file.isValid()) {
                            SkDebugf("Could not open export file: error on create file.\n");
                            return;
                        }
                        file.write(data->bytes(), data->size());
#endif
                    }
                }))
            )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("PNG 300 DPI", 0, [=]( DecButton &btn ) {
                    //SkDebugf( "Export PNG\n" );
                    exportPNGFile();
                }))
                ._( Button("PDF Document", 0, [=]( DecButton &btn ) {
                    exportPDFFile();
                }))
            )
        )

    );
#ifdef __EMSCRIPTEN__
    char szx[512];
    sprintf(szx, "if( window.pes5SetCallback ){ pes5SetCallback('%lu'); }", (unsigned long)this);
    emscripten_run_script(szx);

    EM_ASM({
        eval(`
            window.fnSplitFilenameExt = function(pathfile) { 
                var filename = pathfile.substring(Math.max(pathfile.lastIndexOf('/'), pathfile.lastIndexOf('\\\\\\\\')) + 1); 
                var dotIndex = filename.lastIndexOf('.'); 
                if (dotIndex > 0) { 
                    return [ filename.substring(0, dotIndex), filename.substring(dotIndex) ]; 
                } 
                return [ filename, '']; 
            };
        `);

        window['fnFileHandleWriteBlob'] = async function(handle, blobOrResponse) {
            console.debug('handle:', handle);
            const writable = await handle.createWritable();
            if ('stream' in blobOrResponse) {
                var stream = blobOrResponse.stream();
                await stream.pipeTo(writable);
                return handle;
            } else if ('body' in blobOrResponse) {
                await blobOrResponse.body.pipeTo(writable);
                return handle;
            }
            await writable.write(blobOrResponse);
            await writable.close();
            return handle;
        };

        window['fnSaveFilePicker'] = async function(filename, blob, types) {
            if (window.showSaveFilePicker) {
                console.debug('filename:', filename);
                var options = {'suggestedName' : filename};
                if (types) {
                    options['types'] = types;
                    options['excludeAcceptAllOption'] = true;
                }
                var filehandle = await window.showSaveFilePicker(options)
                                         .then((f) => window['fnFileHandleWriteBlob'](f, blob))
                                         .catch((e) => console.debug('onrejected:', e));
                return filehandle;
            }
            return undefined;
        };

        window['fnSaveFileLegacy'] = function(filename, blob) {
            var a = document.createElement('a');
            document.body.appendChild(a);
            a.href = window.URL.createObjectURL(blob);
            a.download = filename;
            a.onclick = function() {
                setTimeout(
                        function() {
                            window.URL.revokeObjectURL(a.href);
                            document.body.removeChild(a);
                        },
                        1500);
            };
            a.click();
        };

        window['fnDirectoryPicker'] = async function(readonly) {
            if (window.showDirectoryPicker) {
                var options = {'mode' : readonly ? 'read' : 'readwrite'};
                var dirhandle = await window.showDirectoryPicker(options)
                    .catch((e) => console.debug('onreject:', e));
                return dirhandle;
            }
            return undefined;
        };
    });
#endif      
}

DecSVGEditor::~DecSVGEditor() {

}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}

void DecSVGEditor::onLoad() {


    updateNodeTree();
}

void DecSVGEditor::init() {
    //SkDebugf("DecSVGEditor::init\n");
    //SkDebugf("Document path:%s\n", pSVGDomEditor ? pSVGDomEditor->fPath.c_str() : "");

    //std::filesystem::path fullpath( "resources/svg/HBAN-A1=2-01.svg"  );

    //_filepath = fullpath.string();
    //_filename = fullpath.filename().string();
    //_filebasename = fullpath.stem().string();

    //pNodeTree->clear();
    //bool ret = pSVGDomEditor->load(_filepath.c_str());

    //this->load("resources/svg/path-cubic01.svg");
    this->load("resources/svg/paths-arcs03.svg");
    //this->load("_magicpattern/V2-Blouse-Dress-SVG/WPAN-A1=1-01.svg");
    this->onLoad();

}

void DecSVGEditor::onResized(int w, int h) {
    if (pSVGDomEditor) {
        pSVGDomEditor->dirty();
    }
    this->dirty();
}

bool DecSVGEditor::load(const char* filepath, uintptr_t /* uint8_t*  */ iptr, size_t length) {
    // trim (trailing) spaces
    char* szPath = (char*)filepath;
    for (int len = strlen(szPath), i = len - 1; i >= 0; i--) {
        const char c = szPath[i];
        if (c > 0 && c < 0x20) {
            szPath[i] = 0;
        } else {
            break;
        }
    }

    SkDebugf("Open File[%zu]:'%s'\n", strlen(filepath), filepath);
    bool ret = false;
    std::filesystem::path fullpath = std::filesystem::path( filepath );
    _filepath = fullpath.string();
    _filename = fullpath.filename().string();
    _filebasename = fullpath.stem().string();

    if (pNodeTree) {
        pNodeTree->clear();
    }
    if (pSVGDomEditor) {
        //pSVGDomEditor->load(filepath);
        ret = pSVGDomEditor->load(_filepath.c_str(), iptr, length); 
        pSVGDomEditor->dirty();
    }
    return ret;
}

bool DecSVGEditor::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool DecSVGEditor::onDropFile(
        int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    //DecWidget::onDropFile(ix, iy, dropped_file);
    if (pSVGDomEditor && pSVGDomEditor->hitTest(ix, iy)) {
        bool success = load(dropped_file, iptr, length);
        return true;
    }
    return false;
}

void DecSVGEditor::OpenSVGFile(){

#ifdef __EMSCRIPTEN__
    EM_ASM(
        {
            var instance = $0;
            let input = document.createElement('input');
            input.type = 'file';
            input.accept = '.svg';
            input.onchange = function(){
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               console.debug("filename:", f.name);
               var r = new FileReader();
               r.onload = function () {
                    var data = new Uint8Array(r.result);
                    var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                    pbuff['set'](data);

                    console.log( "Hello from JS" );
                    CanvasKit["SVGEditorAPI"]["LoadSVG"]( instance, pbuff );

                    /*
                    CanvasKit._setWidgetFromData(
                        parseInt(instance),
                        iptr,
                        data.byteLength
                    );
                    */
                   /*
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   
                   var pes = new CanvasKit['pesData']();
                   if(pes['loadSVGFromBuffer'](pbuff, true)){
                       pesDoc['addObject'](pes);
                       CanvasKit['PES5Template']['UpdateImages']();
                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                   }
                   pes['delete']();
                   pbuff['delete']();
                   */
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
        }, this
    );
#else
        std::vector<std::string> accept;
        accept.push_back("svg");
        //_pFileDialog->getFile([=](DecFileDialogBox& dlg, const char* filepath) { 

        PesFileDialogResult result = PesSystemLoadDialog("Import SVG File", false, "", accept);
        if(result.bSuccess){
            SkDebugf("result.getPath(): %s\n", result.getPath().c_str());
            SkDebugf("filename:%s\n", result.fileName.c_str());
            this->load(result.getPath().c_str());
            //PES5_ImportSVG(result.getPath());
            //PES5_UpdatePesImages();
        }
#endif
}

void DecSVGEditor::exportPDFFile(const char* filename) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        var savefilename = UTF8ToString($1);
        
        (async function() {
            
            let pesbuff = CanvasKit['SVGEditorAPI']['GetPDFBuffer']( instance );
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            console.debug("saving file:", savefilename);

            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/pdf' : '.pdf'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                window['fnSaveFileLegacy'](savefilename, blob);
            }
            
        })();  
             
    },
    this,
    filename);
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename, "Export PEF File");
    if(result.bSuccess){
        pSVGDomEditor->exportPDF(result.getPath().c_str());
    }
#endif    
}

void DecSVGEditor::saveSVGFile(const char* filename) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        var savefilename = UTF8ToString($1);
        
        (async function() {
            
            let pesbuff = CanvasKit['SVGEditorAPI']['GetSVGBuffer']( instance );
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            console.debug("saving file:", savefilename);

            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'image/svg+xml' : '.svg'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                window['fnSaveFileLegacy'](savefilename, blob);
            }
            
        })();  
             
    },
    this,
    filename);
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename, "Save SVG File");
    if(result.bSuccess){
        pSVGDomEditor->saveSVG(result.getPath().c_str());
    }
#endif  
}

void DecSVGEditor::exportSVGFile(const char* filename) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        var savefilename = UTF8ToString($1);
        
        (async function() {
            
            let pesbuff = CanvasKit['SVGEditorAPI']['GetSVGRenderBuffer']( instance );
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            console.debug("saving file:", savefilename);

            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'image/svg+xml' : '.svg'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                window['fnSaveFileLegacy'](savefilename, blob);
            }
            
        })();  
             
    },
    this,
    filename);
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename, "Save SVG File");
    if(result.bSuccess){
        pSVGDomEditor->exportSVG(result.getPath().c_str());
    }
#endif  
}

void DecSVGEditor::exportPNGFile(const char* filename) {
    //pSVGDomEditor->exportPNG( "_export/test.png", 300 );
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        var savefilename = UTF8ToString($1);
        
        (async function() {
            
            let pesbuff = CanvasKit['SVGEditorAPI']['GetPNGBuffer']( instance, 300 );
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            console.debug("saving file:", savefilename);

            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'image/png' : '.png'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                window['fnSaveFileLegacy'](savefilename, blob);
            }
            
        })();  
             
    },
    this,
    filename);
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename, "Export PNG File");
    if(result.bSuccess){
        pSVGDomEditor->exportPNG(result.getPath().c_str(), 300);
    }
#endif
}    



void DecSVGEditor::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREE_SELECT_CHANGED ) {
        selectNode( (DecTreeItem *)data );
    }
}

void DecSVGEditor::selectNode( DecTreeItem *pItem ) {
    if( pSVGDomEditor ) {
        if( pItem ) {
            pSVGDomEditor->selectNode( (SkSVGNode *)pItem->_externalData );
        }
        else {
            pSVGDomEditor->selectNode( NULL );
        }
    }
}


void DecSVGEditor::setNodeVisibility( DecTreeItem *treeItem, bool bVisible ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;
    if( bVisible ) {
        vType = SkSVGVisibility::Type::kVisible;
        node->setAttribute("visibility", "visible");
    }
    else {
        vType = SkSVGVisibility::Type::kHidden;
        node->setAttribute("visibility", "hidden");
    }

    auto visibleTypeValue = SkSVGVisibility(vType);
    SkSVGProperty<SkSVGVisibility, true> propVisibility(visibleTypeValue);
    //node->setVisibility(propVisibility);

    DecIcon *icon = (DecIcon *)treeItem->_externalWidget;
    icon->text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 );

    std::vector<DecWidget*>children = treeItem->getChildrenNodes(); 
    for( int i=0; i<children.size(); i++ ) {
        DecTreeItem *w = (DecTreeItem *)children[i];
        setNodeVisibility( w, bVisible );
    }
    pSVGDomEditor->dirty();
}

bool DecSVGEditor::getNodeVisibility( DecTreeItem *treeItem ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    bool bVisible = true;
    if( visibility.isValue() ) {
        vType = visibility->type();
    }
    if( vType == SkSVGVisibility::Type::kVisible ) {
        bVisible = true;
    }
    else {
        bVisible = false;
    }
    return bVisible;
}

void DecSVGEditor::toggleNodeVisibility( DecTreeItem *treeItem ) {
    bool bVisible = getNodeVisibility(treeItem);

    bVisible = !bVisible;
    setNodeVisibility( treeItem, bVisible );
}

void DecSVGEditor::addSubNodeToTreeItem(DecTreeItem& treeNode, SkSVGNode* node, int deep) {
    DecTreeItem *new_node;

    char sz[128];

    SkSVGTag tag = node->tag();

    if( tag == SkSVGTag::kTextLiteral ) {
        return;
    }

    // maxdeep:50
    char chDeep[] = "--------------------------------------------------";
    chDeep[deep] = 0;

    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;


    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    const SkSVGProperty<SkSVGStringType, false> &propData = node->getDataName();
    const SkSVGStringType* data = propData.getMaybeNull();
    const char* cpchData = data ? data->c_str() : "";

   

    if( visibility.isValue() ) {
        vType = visibility->type();

#ifdef DEBUG_NODE
        int numChildren = 0;
        if (node->hasChildren()) {
            if (tag == SkSVGTag::kG) {
                SkSVGG* pG = (SkSVGG*)node;
                numChildren = (int)pG->fChildren.size();
            }
        }
        SkDebugf("%snode: %s(child:%d), has Visibility[%s], data-name:'%s' \n",
                 chDeep,
                 tagName[(int)tag],
                 numChildren,
                 visibilityType[(int)vType],
                 cpchData);
        /*
        for( int ii=0; ii<numChildren; ii++ ) {
            SkSVGG* pG = (SkSVGG*)node;
            sk_sp<SkSVGNode> nd = pG->fChildren.at( ii );
            SkSVGTag tg = node->tag();
            SkDebugf( "Child Tag: %s\n", tagName[(int)tg] );
        }
        */
#endif
    } else {
        if (tag == SkSVGTag::kG) {
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kVisible);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            // can remove >>>
#ifdef DEBUG_NODE
            int numChildren = 0;
            if (node->hasChildren()) {
                if (tag == SkSVGTag::kG) {
                    SkSVGG* pG = (SkSVGG*)node;
                    numChildren = (int)pG->fChildren.size();
                }
            }
            SkDebugf("%snode: %s(child:%d) set Visibility[Visible], data-name:'%s' \n",
                     chDeep,
                     tagName[(int)tag],
                     numChildren,
                     cpchData);
            /*
            for( int ii=0; ii<numChildren; ii++ ) {
                SkSVGG* pG = (SkSVGG*)node;
                sk_sp<SkSVGNode> nd = pG->fChildren.at( ii );
                SkSVGTag tg = node->tag();
                SkDebugf( "Child Tag: %s\n", tagName[(int)tg] );
            } 
            */                    
#endif
            // can remove <<<
        }
        else {
#ifdef DEBUG_NODE
            if( data ) {
                SkDebugf("%snode: %s data-name:'%s' \n",
                        chDeep,
                        tagName[(int)tag],
                        cpchData);                
            }
            else {
                SkDebugf("%snode: %s\n",
                        chDeep,
                        tagName[(int)tag]);  
            }
#endif

        }
    }

    sprintf(sz, "%s %s ", node->tagName(), cpchData);

    treeNode._(SVGTreeItem(sz).ref((DecWidget**)&new_node), true);  // add to front to display like Adobe Illustrator
    new_node->_externalData = node;
    node->_extraInfo = (void *)new_node;
    new_node->addWidget( Icon( 0xf06e)
        .ref( (DecWidget **)&new_node->_externalWidget )
        .externalData( new_node )
        .text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 )
        .onMousePressed( [=](DecWidget &widget) -> bool {
            toggleNodeVisibility( (DecTreeItem *)widget._externalData );
            return true;
        })
    );
    if( node->hasChildren() ) {
        //if( tag == SkSVGTag::kG ) {
        //    SkSVGG *pG = (SkSVGG *)node;
        //    int numChildren = (int)pG->fChildren.size();
        //    for( int i=0; i<numChildren; i++ ) {
        //        sk_sp<SkSVGNode> child_node = pG->fChildren.at( i );
        //
        //        //rootItem._( TreeItem( sz ) );
        //        addSubNodeToTreeItem( *new_node, child_node.get(), deep+1 );
        //    }
        //}
        SkSVGNode* pNode = node;
        bool bTextContainer = false;
        switch (pNode->tag()) {
            case SkSVGTag::kText:
            case SkSVGTag::kTextPath:
            case SkSVGTag::kTSpan:
                bTextContainer = true;
                break;
            default:
                break;
        }

        if (bTextContainer) {
            const auto& cfChildren = static_cast<SkSVGTextContainer*>(pNode)->fChildren;
            for (int i = 0; i < cfChildren.size(); ++i) {
                const auto& cNode = cfChildren[i];
                sk_sp<SkSVGNode> child_node = cfChildren.at(i);
                addSubNodeToTreeItem(*new_node, child_node.get(), deep + 1);
            }
        } 
        else {
            const auto& cfChildren = static_cast<SkSVGContainer*>(pNode)->fChildren;
            for (int i = 0; i < cfChildren.size(); ++i) {
                sk_sp<SkSVGNode> child_node = cfChildren.at(i);
                addSubNodeToTreeItem(*new_node, child_node.get(), deep + 1);
            }
        }
    }
    treeNode.collapse();
}

const char *DecSVGEditor::getFitPaperSize( float w, float h ) {
    int fW = (int)(w * 25.4);
    int fH = (int)(h * 25.4);
    if( fW > fH ) { // landscape
        fW = (int)(h * 25.4);
        fH = (int)(w * 25.4);
    }
    for( int i=0; i<numPaperType; i++ ) {
        if( paperWidth[i] >= fW && paperHeight[i] >= fH ) {
            return paperName[i];
        }
    }
    return paperName[numPaperType];
}

void DecSVGEditor::updateNodeTree() {
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;

        fDom = pSVGDomEditor->fDom;
        if( fDom ) {
            root = fDom->getRoot();

            //fDom->findNodeById()

            SkSize szC = fDom->containerSize();
            //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );

            SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
            //SkDebugf( "SVG Root Size: %0.2f %0.2f\n", szR.fWidth, szR.fHeight );
            //SkDebugf( "SVG Root Inches: %0.2f %0.2f\n", szR.fWidth/72.0, szR.fHeight/72.0 );
            if( szR.fWidth == 0 || szR.fHeight == 0 ) {
                szR.fWidth = szC.fWidth;
                szR.fHeight = szC.fHeight;
            }

            char sz[256];
            sprintf( sz, "Paper Size: %0.2f x %0.2f inches (%s)", szR.fWidth/72.0, szR.fHeight/72.0,
                getFitPaperSize( szR.fWidth/72.0, szR.fHeight/72.0 ) );
            if( _pTextDocumentSize ) {
                _pTextDocumentSize->setText( sz );
            }

            //SkSVGTag rtag = root->tag();
            sprintf(sz, "%s", root->tagName());

            DecTreeItem &rootItem = TreeItem( sz );
            rootItem._externalData = root;
            root->_extraInfo = (void*)&rootItem;
            if( pNodeTree ) {
                pNodeTree->_( rootItem );
            }

            if( root->hasChildren() ) {
                int numChildren = (int)root->fChildren.size();

                if( pNodeTree ) {
                    for( int i=0; i<numChildren; i++ ) {
                        sk_sp<SkSVGNode> node = root->fChildren.at( i );

                        //rootItem._( TreeItem( sz ) );
                        addSubNodeToTreeItem( rootItem, node.get(), 1 );


#ifdef DEBUG_NODE
/*
                        SkSVGTag tag = node->tag();
                        if( tag == SkSVGTag::kTextLiteral ) {
                            SkSVGTextFragment* pNode = (SkSVGTextFragment*)(node.get());
                        }
                        else if( tag == SkSVGTag::kG ) {
                            if( node->hasChildren() ) {
                                for( int ii=0; ii<numChildren; ii++ ) {
                                    sk_sp<SkSVGNode> nd = root->fChildren.at( ii );
                                    SkSVGTag tg = node->tag();
                                    SkDebugf( "Child Tag: %s\n", tagName[(int)tg] );
                                }
                            }
                        }
*/
#endif

                    }
                }
            }
        }

    }
}
