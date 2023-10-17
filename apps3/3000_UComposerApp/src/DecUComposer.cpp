#include "include/core/SkPaint.h"
#include "include/core/SkString.h"
#include "DeclarativeUI.h"
#include "DecUComposer.h"
#include "DecTestWidget.h"
#include "DecUTestPanel.h"
#include "PesSystemUtils.hpp"

static DecUComposer *__ucomposer = nullptr;

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

DecUComposer::DecUComposer() : DecStackContainer() {
    size( -1, -1 );
    border( false );
    background( false );

    doc_width = 1000;
    doc_height = 1000;

    selectedElementIndex = -1;

#ifdef __EMSCRIPTEN__
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

DecUComposer::~DecUComposer(){
} 

DecUComposer &UComposer() {
    __ucomposer = new DecUComposer();
    return *__ucomposer;
}

DecUComposer* DecUComposer::getInstance(){
    return __ucomposer;
}

void DecUComposer::init() {
    DecStackContainer::init();
    //SkDebugf( "UCompose Init\n" );
}

void DecUComposer::update() {
    DecStackContainer::init();
    //SkDebugf( "Update\n" );
    //SkDebugf( "%i %i %i %i\n", width(), _workspace->width(), _widget->width(), _controler->width() );
}

void DecUComposer::NewDocument() {
    _children.clear();
}

void DecUComposer::_draw_children(SkCanvas& canvas) {
    canvas.save();

    if( !_isVisible ) {
        return;
    }

    SkRect rectClip;
    bool bClip = canvas.getLocalClipBounds( &rectClip );

    if (rectClip.width() == 0) 
        return;
    if (rectClip.height() == 0) 
        return;

    SkPaint paint;
    paint.setStyle( SkPaint::Style::kStroke_Style );
    paint.setStrokeWidth( 1 );
    paint.setColor( 0xFFA0A0A0 );

    float sx = width() / (doc_width + 100);
    float sy = height() / (doc_height + 100);
    float ss = std::min( sx, sy );

    float tx = (width() - (doc_width * ss)) / 2;
    float ty = (height() - (doc_height * ss)) / 2;

    canvas.translate( tx, ty );
    canvas.scale( ss, ss );

    SkRect rect = SkRect::MakeXYWH( 0, 0, doc_width, doc_height );
    canvas.drawRect( rect, paint );

    
    //conresizedst SkIRect bounds = SkIRect::MakeXYWH(0, 0, rectClip.width(), rectClip.height());

    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( !w->_isVisible ) {
            continue;
        }

        if (w->width() == 0) 
            continue;
        if (w->height() == 0) 
            continue;
        
        /*
        if( w->y() > rectClip.fBottom ) {
            continue;
        }
        if( w->y() + w->height() < rectClip.fTop ) {
            continue;
        }
        if( w->x() > rectClip.fRight ) {
            continue;
        }
        if( w->x() + w->width() < rectClip.fLeft ) {
            continue;
        }
        */

        //SkIRect cBounds = SkIRect::MakeXYWH(0, 0, w->width(), w->height());
        //if (!cBounds.intersect(bounds)) {
        //    SkDebugf("[%.1f, %.1f] [%d, %d]\n", rectClip.width(), rectClip.height(), w->width(), w->height());
        //    continue;
        //}
        w->pre_draw( canvas );
        w->draw( canvas, _drawLevel );
        w->post_draw( canvas );
    }  
      

    canvas.restore();
}

void DecUComposer::selectElement( int index ) {
    selectedElementIndex = index;

    SkDebugf( "Select Element %i\n", index );
    if( index >= 0 && _children[index]->type() == "SVGDom" ) {
        SkDebugf( "CreateSVGTRee\n" );
        createSVGTree( index );
    }

    onContentChanged( EVENT_SELECT_CHANGED, selectedElementIndex );
}

void DecUComposer::createSVGTree( int index ) {
    SkDebugf( "In createSVG Tree\n" );
    if( index < 0 || index >= _children.size() ) {
        SkDebugf( "Index %i, Children %i\n", index, (int)_children.size() );
        return;
    }

    if( _children[index]->type() != "SVGDom" ) {
        SkDebugf( "Child Type %s\n", _children[index]->type().c_str() );
        return;
    }

    SkDebugf( "Geting DOM\n" );
    DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
    sk_sp<SkSVGDOM>fDom = pSVGDom->fDom;
    if( fDom ) {
        SkDebugf( "Got DOM\n" );
        SkSVGSVG *root = fDom->getRoot();  
        if( root->hasChildren() ) {
            int numChildren = (int)root->fChildren.size();
            SkDebugf( "DOM children %i\n", numChildren );

            mSVGGroupMap.clear();
            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> _node = root->fChildren.at( i );
                SkSVGNode* node = _node.get();
                SkSVGTag tag = node->tag();
                SkDebugf( "Tag: %s\n", tagName[(int)tag] );
                if( tag == SkSVGTag::kG ) {
                    const SkSVGProperty<SkSVGStringType, false> &propData = node->getId();
                    const SkSVGStringType* data = propData.getMaybeNull();
                    const char* cpchData = data ? data->c_str() : "";
                    if( !data ) {
                        SkDebugf( "No Name, try ID\n" );
                        const SkSVGProperty<SkSVGStringType, false> &propId = node->getDataName();
                        data = propId.getMaybeNull();
                        cpchData = data ? data->c_str() : "";
                    }
                    if( data ) {
                        SkDebugf( "SVG node: %s\n", cpchData );
                        mSVGGroupMap[cpchData] = node;
                    }
                    else {
                        SkDebugf( "No name\n" );
                    }
                    SkSVGG* pG = (SkSVGG*)node;
                    for( int j=0; j<pG->fChildren.size(); j++ ) {
                        sk_sp<SkSVGNode> _subnode = pG->fChildren.at( j );
                        SkSVGNode* subnode = _subnode.get();                        
                        parseSVGSubTree( 0, subnode );
                    }
                }
            }
        }
    }
    else {
        SkDebugf( "No Dom\n" );
    }
}

void DecUComposer::parseSVGSubTree( int level, SkSVGNode* node ) {
    SkSVGTag tag = node->tag();
    if( tag == SkSVGTag::kG ) {
        const SkSVGProperty<SkSVGStringType, false> &propData = node->getId();
        const SkSVGStringType* data = propData.getMaybeNull();
        const char* cpchData = data ? data->c_str() : "";
        if( !data ) {
            SkDebugf( "No Name, try ID\n" );
            const SkSVGProperty<SkSVGStringType, false> &propId = node->getDataName();
            data = propId.getMaybeNull();
            cpchData = data ? data->c_str() : "";
        }
        if( data ) {
            char chDeep[] = "--------------------------------------------------";
            chDeep[level*2+2] = 0;
            SkDebugf( "%s SVG node: %s\n", chDeep, cpchData );
            mSVGGroupMap[cpchData] = node;
        }
        SkSVGG* pG = (SkSVGG*)node;
        for( int i=0; i<pG->fChildren.size(); i++ ) {
            sk_sp<SkSVGNode> _subnode = pG->fChildren.at( i );
            SkSVGNode* subnode = _subnode.get();                        
            parseSVGSubTree( level+1, subnode );
        }       
    }
}

void DecUComposer::LoadSVG( std::string uri, uintptr_t /* uint8_t*  */ iptr, size_t length ) {
    DecSVGDom *pSVGDom = nullptr;
    _( SVGDom( nullptr, [=](DecSVGDom &dom) {
        if( dom._svgWidth == 0 && dom._svgHeight == 0 ) {
            dom.size( 144, 144 );
        }
        else {
            dom.size( dom._svgWidth, dom._svgHeight );
        }
        vElements.push_back( &dom );
        onContentChanged( EVENT_SVG_CHANGED, vElements.size() );
        selectElement( vElements.size() - 1 );
    }).ref((DecWidget **)&pSVGDom).border(false) );  

    pSVGDom->load( uri.c_str(), iptr, length );  
}

void DecUComposer::OpenSVGFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
        {
            var instance = $0;
            let input = document.createElement('input');
            input.type = 'file';
            input.accept = '.svg';
            input.onchange = function(){
                var uComposer = CanvasKit['DecUComposer']['getInstance']();
                var f = input.files[0];
                console.debug("filename:", f.name);
                var r = new FileReader();
                r.onload = function () {
                    var data = new Uint8Array(r.result);
                    var iptr = CanvasKit._malloc(data.byteLength);
                    CanvasKit.HEAPU8.set(data, iptr);
                    uComposer['LoadSVG'](
                        f.name,
                        iptr,
                        data.byteLength
                    );
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
        LoadSVG( result.getPath().c_str() );
    }
#endif
}

void DecUComposer::onContentChanged(int event_type, int index) {
    //SkDebugf( "OnContentChanged: %i %i \n", event_type, index );
#ifdef __EMSCRIPTEN__
    EM_ASM(
        {
            var instance = $0;
            var changetype = $1;
            var index = $2;

            //console.log( "Fire event: ", instance, changetype, index );

            window.dispatchEvent(new CustomEvent('ucomposerchanged', {
                detail: {
                    instance: instance,
                    event: changetype,
                    index: index
                }
            }) );     
        }, this, event_type, index
    );
#endif
}

sk_sp<SkData> DecUComposer::GetSVGData( int index ) {
    if( _children[index]->type() == "SVGDom" ) {
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        return pSVGDom->getSVGData( );
    }
    else {
        SkDebugf( "Could not Gen SVG Data of Element Type: %s\n", _children[index]->type().c_str() );
    }
    return nullptr;
}

sk_sp<SkData> DecUComposer::GetSVGRenderData( int index ) {
    if( _children[index]->type() == "SVGDom" ) {
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        return pSVGDom->getSVGRenderData( );
    }
    else {
        SkDebugf( "Could not Gen SVG Render Data of Element Type: %s\n", _children[index]->type().c_str() );
    }
    return nullptr;
}

sk_sp<SkData> DecUComposer::GetPNGData( int index, int dpi ) {
    if( _children[index]->type() == "SVGDom" ) {
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        return pSVGDom->getPNGData( dpi );
    }
    else {
        SkDebugf( "Could not Gen PNG Data of Element Type: %s\n", _children[index]->type().c_str() );
    }
    return nullptr;
}

void DecUComposer::ExportPNGFile( int index, std::string filename ) {
    //pSVGDomEditor->exportPNG( "_export/test.png", 300 );
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        let index = $1;
        var savefilename = UTF8ToString($2);
        
        (async function() {
            var uComposer = CanvasKit['DecUComposer']['getInstance']();
            let arraybuff = uComposer['GetPNGBytes']( index, 72 );
            let blob = new Blob([arraybuff]);
            
            //data.clear();
            //data.delete();
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
    index,
    filename.c_str());
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename.c_str(), "Export PNG File");
    if(result.bSuccess){
        SkDebugf( "Export %i PNG to: %s\n", index, result.getPath().c_str() );
        
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        pSVGDom->exportPNG( result.getPath().c_str(), 72, nullptr );
    }
#endif
}    

void DecUComposer::SaveSVGFile( int index, std::string filename ) {
    //pSVGDomEditor->exportPNG( "_export/test.png", 300 );
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        let index = $1;
        var savefilename = UTF8ToString($2);
        
        (async function() {
            var uComposer = CanvasKit['DecUComposer']['getInstance']();
            let arraybuff = uComposer['GetSVGBytes']( index );
            let blob = new Blob([arraybuff]);
            
            //data.clear();
            //data.delete();
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
    index,
    filename.c_str());
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename.c_str(), "Save SVG File");
    if(result.bSuccess){
        SkDebugf( "Save %i SVG to: %s\n", index, result.getPath().c_str() );
        
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        pSVGDom->saveSVG( result.getPath().c_str() );
    }
#endif
}    

void DecUComposer::ExportSVGFile( int index, std::string filename ) {
    //pSVGDomEditor->exportPNG( "_export/test.png", 300 );
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        let instance = $0;
        let index = $1;
        var savefilename = UTF8ToString($2);
        
        (async function() {
            var uComposer = CanvasKit['DecUComposer']['getInstance']();
            let arraybuff = uComposer['GetSVGRenderBytes']( index );
            let blob = new Blob([arraybuff]);
            
            //data.clear();
            //data.delete();
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
    index,
    filename.c_str());
#else
    PesFileDialogResult result = PesSystemSaveDialog(filename.c_str(), "Export SVG File");
    if(result.bSuccess){
        SkDebugf( "Export %i SVG to: %s\n", index, result.getPath().c_str() );
        
        DecSVGDom *pSVGDom = (DecSVGDom *)_children[index];
        pSVGDom->exportSVG( result.getPath().c_str() );
    }
#endif
}  

void DecUComposer::startExportPDF() {
    _pdfStream = new SkDynamicMemoryWStream();
    if (_pdfStream == nullptr) 
        return;
    _pdfDoc = SkPDF::MakeDocument(_pdfStream);
}

sk_sp<SkData> DecUComposer::endExportPDF() {
    if (_pdfDoc.get() == nullptr) 
        return nullptr;

    _pdfDoc->close();
    sk_sp<SkData> data = _pdfStream->detachAsData();

    return data;
}

void DecUComposer::setDarkMode( bool dark ) {
    //SkDebugf( "Dark mode: %i\n", dark );
    if( dark ) {
        background( true );
        bg_color( 0xFF374151 );
    }
    else {
        background( true );
        bg_color( 0xe5e7eb );
    }
}

std::string DecUComposer::GetGroupList() {
    std::vector<std::string> groups;
    std::string res = "[";

    for (const auto& [key, _] : mSVGGroupMap) {
        res += std::string("\"") + std::string(key) + "\",";
    }
    if( res[res.size()-1] == ',' ) {
        res = res.substr(0,res.size()-1);
    }
    res = res + std::string("]");
    SkDebugf( "GroupList: %s\n", res.c_str() );
    return res;
}

void DecUComposer::DisplayPartType( std::string _part, std::string _type ) {
    //SkDebugf( "DisplayPartType: %s %s\n", _part.c_str(), type.c_str() );

    SkString part = SkString(_part );
    SkString type = SkString(_type );
    SkString type2 = SkString(_type );
    type2.append( "_" );
    for (const auto& [key, _] : mSVGGroupMap) {
        SkTArray<SkString>out;

        SkStrSplit(key.c_str(), "/", &out);
        if( out.size() == 2 ) {
            if( out[0] == part ) {
                SkSVGNode *pNode = mSVGGroupMap[key];
                if( out[1] == type || out[1] == type2 ) {
                    //SkDebugf( "Show: %s %s\n", part.c_str(), type.c_str() );
                    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;
                    pNode->setAttribute("visibility", "visible");                   
                }
                else {
                    //SkDebugf( "Hide: %s %s\n", part.c_str(), out[1].c_str() );
                    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kHidden;
                    pNode->setAttribute("visibility", "hidden");
                }
            }
        }
    }    
}
