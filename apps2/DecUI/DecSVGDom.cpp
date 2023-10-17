#include <filesystem>
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkStream.h"
#include "tools/Resources.h"
#include "src/utils/SkOSPath.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

DecSVGDom::DecSVGDom( const char *uri, std::function<void(DecSVGDom &dom)> onload ) {
    background( false );
    border( false );
    _testDraw = false;
    fPath = "";
    fDom = NULL;

    _svgContentSize = false;

    _onLoad = onload;
    if( uri ) {
        load( uri );
    }
}

DecSVGDom::~DecSVGDom() {

}

DecSVGDom &SVGDom( const char *uri, std::function<void(DecSVGDom &dom)> onload ) {
    DecSVGDom *w = new DecSVGDom( uri, onload );
    return *w;
}

bool DecSVGDom::loadFromResources(const char* filename) {
    SkDebugf( "load from resources\n" );
    sk_sp<SkData> data = GetResourceAsData(filename);
    std::unique_ptr<SkStreamAsset> stream(new SkMemoryStream(std::move(data)));
    if (!stream) {
        SkDebugf("Could not open %s.\n", fPath.c_str());
        return false;
    }

    fWinSize = SkSize::Make(width(), height());

    auto rp = skresources::DataURIResourceProviderProxy::Make(
            skresources::FileResourceProvider::Make(
                    SkOSPath::Dirname(fPath.c_str()),
                    /*predecode=*/true),
            /*predecode=*/true);
    fDom = SkSVGDOM::Builder().setResourceProvider(std::move(rp)).make(*stream);
    if (fDom) {
        // fDom->setContainerSize(fWinSize);
        calcSVGScale();
        onLoad();
        if (_onLoad) {
            _onLoad(*this);
        }
    }    
    return true;
}

bool DecSVGDom::load(const char* filename, uintptr_t /* uint8_t*  */ iptr, size_t length) {
    fPath = SkString( filename );

    std::filesystem::path p = std::filesystem::path( filename );
    name = p.filename().replace_extension().string();

    if (iptr && length) {
        uint8_t* puint8 = reinterpret_cast<uint8_t*>(iptr);
        sk_sp<SkData> data = SkData::MakeFromMalloc(puint8, length);

        std::unique_ptr<SkStreamAsset> stream(new SkMemoryStream(std::move(data)));
        if (!stream) {
            SkDebugf("Could not open %s.\n", fPath.c_str());
            return false;
        }

        fWinSize = SkSize::Make(width(), height());

        auto rp = skresources::DataURIResourceProviderProxy::Make(
                skresources::FileResourceProvider::Make(SkOSPath::Dirname(fPath.c_str()),
                /*predecode=*/true),
                /*predecode=*/true);
        fDom = SkSVGDOM::Builder().setResourceProvider(std::move(rp)).make(*stream);
        if (fDom) {
            calcSVGScale();
            onLoad();
            if (_onLoad) {
                _onLoad(*this);
            }
        }

        return true;
    
    } else {
#ifdef __EMSCRIPTEN__
        emscriptenDownloadAsset(
                this,
                filename,
                [](struct emscripten_fetch_t* fetch) {
                    sk_sp<SkData> data = SkData::MakeWithCopy(fetch->data, fetch->numBytes);
                    DecSVGDom* widget = (DecSVGDom*)fetch->userData;

                    // auto stream = SkMemoryStream( data );
                    std::unique_ptr<SkStreamAsset> stream(new SkMemoryStream(std::move(data)));
                    if (!stream) {
                        SkDebugf("Could not open %s.\n", widget->fPath.c_str());
                        return;
                    }

                    widget->fWinSize = SkSize::Make(widget->width(), widget->height());

                    auto rp = skresources::DataURIResourceProviderProxy::Make(
                            skresources::FileResourceProvider::Make(
                                    SkOSPath::Dirname(widget->fPath.c_str()),
                                    /*predecode=*/true),
                            /*predecode=*/true);
                    widget->fDom =
                            SkSVGDOM::Builder().setResourceProvider(std::move(rp)).make(*stream);
                    if (widget->fDom) {
                        // widget->fDom->setContainerSize(widget->fWinSize);
                        widget->calcSVGScale();
                        widget->onLoad();
                        if (widget->_onLoad) {
                            widget->_onLoad(*widget);
                        }
                    }
                },
                [](struct emscripten_fetch_t* fetch) { SkDebugf("Download failed\n"); });
#else
        auto stream = SkStream::MakeFromFile(fPath.c_str());

        if (!stream) {
            SkDebugf("Could not open %s.\n", fPath.c_str());
            return false;
        }
        // SkDebugf( "Stream OK\n" );

        fWinSize = SkSize::Make(width(), height());
        SkDebugf( "Size: %i %i\n", (int)fWinSize.fWidth, (int)fWinSize.fHeight );

        auto rp = skresources::DataURIResourceProviderProxy::Make(
                skresources::FileResourceProvider::Make(SkOSPath::Dirname(fPath.c_str()),
                /*predecode=*/true),
                /*predecode=*/true);
        fDom = SkSVGDOM::Builder().setResourceProvider(std::move(rp)).make(*stream);
        if (fDom) {
            // SkDebugf( "SVG DOM ok\n" );
            calcSVGScale();
            onLoad();
            if (_onLoad) {
                _onLoad(*this);
            }
        }
#endif
        return true;
    }


}

void DecSVGDom::calcSVGScale() {
    //SkDebugf( "=== Calculate SVG Scale ===\n" );
    SkSize szC = fDom->containerSize();   
    //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );            

    SkSVGSVG *root = fDom->getRoot();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    //SkDebugf( "SVG Root Size (72 DPI): %0.2f %0.2f\n", szR.fWidth, szR.fHeight );
    //SkDebugf( "SVG Root Inches: %0.2f %0.2f\n", szR.fWidth/72.0, szR.fHeight/72.0 ); 

    _svgContentSize = true;
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        //SkDebugf( "No Intrinsic Size\n" );
        _svgScaleX = 1.0;
        _svgScaleY = 1.0;

        _svgDPIX = 72.0;
        _svgDPIY = 72.0;

        if( szC.fWidth == 0 || szC.fHeight == 0 ) {
            _svgContentSize = false;

            //SkDebugf( "No Container Size\n" );

            //SkDebugf( "Size: %i %i\n", width(), height() );
            fWinSize = SkSize::Make(width(), height());
            //SkDebugf( "Set size to window size: %0.2f, %0.2f\n", fWinSize.fWidth, fWinSize.fHeight );

            fDom->setContainerSize( fWinSize );
            szC = fDom->containerSize();
            //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );              
        }

        _svgWidth72 = szC.fWidth;
        _svgHeight72 = szC.fHeight;

        _svgWidth = szC.fWidth;
        _svgHeight = szC.fHeight;        
    }
    else {
        _svgWidth72 = szR.fWidth;
        _svgHeight72 = szR.fHeight;

        if( szC.fWidth == 0 || szC.fHeight == 0 ) {
            SkDebugf( "No Container Size\n" );
            SkDebugf( "Set size to intrinsic size\n" );
            fDom->setContainerSize( szR );
            SkSize szC = fDom->containerSize();
            SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );              
        }        

        _svgWidth = szC.fWidth;
        _svgHeight = szC.fHeight;             

        _svgScaleX = szC.fWidth / szR.fWidth;
        _svgScaleY = szC.fHeight / szR.fHeight;

        _svgDPIX = 72.0 * _svgScaleX;
        _svgDPIY = 72.0 * _svgScaleY;
    }
//
    //SkDebugf( "SVG Size: %0.2f, %0.2f\n", _svgWidth72, _svgHeight72 );
    //SkDebugf( "SVG Size (72dpi): %0.2f, %0.2f\n", _svgWidth, _svgHeight );
    //SkDebugf( "SVG Scale: %0.2f, %0.2f\n", _svgScaleX, _svgScaleY );
    //SkDebugf( "SVG DPI: %0.2f, %0.2f\n", _svgDPIX, _svgDPIY );
}

void DecSVGDom::onResized( int w, int h ) {
    fWinSize = { (float)w, (float)h };
    if (fDom) {
        if( !_svgContentSize ) {
            SkDebugf( "Set size to window size\n" );

            fDom->setContainerSize( fWinSize );
            SkSize szC = fDom->containerSize();
            SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );            

            SkSVGSVG *root = fDom->getRoot();
            SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
            //SkDebugf( "SVG Root Size: %0.2f %0.2f\n", szR.fWidth, szR.fHeight );
            //SkDebugf( "SVG Root Inches: %0.2f %0.2f\n", szR.fWidth/72.0, szR.fHeight/72.0 );
        }
    }
}

bool DecSVGDom::onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    //DecWidget::onDropFile(ix, iy, dropped_file);
    //return load(dropped_file);
    
//#ifdef __EMSCRIPTEN__
//    EM_ASM(var stream = FS.open(filename, 'r');
//        var buf = new Uint8Array(4);
//        FS.read(stream, buf, 0, 4, 0);
//        FS.close(stream);
//    );
//#else
    bool ret = load(dropped_file, iptr, length);
    SkDebugf("DecSVGDom::onDropFile:%s, ret: %d\n", dropped_file, ret);
    return ret;
//#endif
}

void DecSVGDom::_draw_content(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }
    if( _testDraw ) {
        DecWidget::_draw_content(canvas);
        return;
    }

    SkSize sz = fDom->containerSize();

    float scale = std::min( width()/sz.fWidth, height()/sz.fHeight );
    float dw = ((width() - sz.fWidth * scale)/2) / scale;
    float dh = ((height() - sz.fHeight * scale)/2) / scale;

    //SkDebugf( "Scale: %0.2f\n", scale );
    canvas.save();
    canvas.scale( scale, scale );
    canvas.translate( dw, dh );

    if (fDom) {
        fDom->render(&canvas);
    }

    canvas.restore();

    //char str[64];
    //sprintf( str, "win size: %i %i", width(), height() );
    //SkPaint paint;
    //paint.setStyle( SkPaint::Style::kFill_Style );
    //paint.setColor( 0xFF000000 );
    //SkFont font;

    //sk_sp<SkTypeface> typeface = _font->typeface(0);
    //
    //font.setTypeface(typeface);
    //font.setSubpixel(true);
    //font.setSize(16);

    //canvas.drawSimpleText( str, strlen(str), SkTextEncoding::kUTF8, 200, 200, font, paint );
    //canvas.drawRect( SkRect::MakeXYWH( 100, 100, 200, 200), paint );
}

SkPixmap DecSVGDom::getPixmap( float dpi, std::function<void(SkCanvas &canvas)>hook ) {
    SkSVGSVG *root = fDom->getRoot();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));

    float exportW = szR.fWidth * dpi / 72.0;
    float exportH = szR.fHeight * dpi / 72.0;

    SkSize sz = fDom->containerSize();

    auto surface = SkSurface::MakeRasterN32Premul(exportW, exportH);

    float scale = std::min( exportW/(szR.fWidth * _svgScaleX), exportH/(szR.fHeight * _svgScaleY) );

    float dw = 0;
    float dh = 0;

    SkCanvas *canvas = surface->getCanvas();
    canvas->scale( scale, scale );
    canvas->translate( 0-(0 * _svgScaleX), 0-(0 * _svgScaleY) );
    fDom->render( canvas );
    if( hook ) {
        hook( *canvas );
    }    

    SkPixmap pixmap;
    surface->peekPixels(&pixmap);
    return pixmap;
}

sk_sp<SkData> DecSVGDom::getSVGData() {
    if (!fDom) {
        return nullptr;
    }
    SkDebugf( "GetSVGData\n" );
    const sk_sp<SkSVGSVG> svg = fDom->fRoot;

    svg->setAttribute("xmlns",          "http://www.w3.org/2000/svg");
    svg->setAttribute("xmlns:xlink",    "http://www.w3.org/1999/xlink");
    
    // default -1: single line, no indent
    std::string str = svg->toString(0);

    //SkDebugf("%s\n", str.c_str());
    
    sk_sp<SkData> data = SkData::MakeWithCopy(str.c_str(), str.length());
    return data;
}


sk_sp<SkData> DecSVGDom::getSVGRenderData() {
    if (!fDom) {
        return nullptr;
    }
    SkDebugf( "GetSVGRenderData\n" );

    // SkSize containerSize = fDom->containerSize();
    //SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 95.997395));
    SkSVGSVG *root = fDom->getRoot();
    SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 96));
    SkDebugf("containerSize:[%.03f,%.03f]\n",
             containerSize.width(),
             containerSize.height());
    SkRect bounds = SkRect::MakeSize(containerSize);
    SkDynamicMemoryWStream writer;
    { // block for SkXMLStreamWriter(inside SkSVGCanvas obj) flush() on destrutor
        std::unique_ptr<SkCanvas> canvas = SkSVGCanvas::Make(bounds, &writer);
        SkCanvas* pcanvas = canvas.get();

        // this->_draw_content(*pcanvas);
        fDom->render(pcanvas);
    }
    
    sk_sp<SkData> data = writer.detachAsData();
    return data;
}

sk_sp<SkData> DecSVGDom::getPNGData( float dpi ) {
    SkPixmap pixmap = getPixmap( dpi );

    SkDynamicMemoryWStream buf;
    bool success = SkPngEncoder::Encode(&buf, pixmap, SkPngEncoder::Options());
    return success ? buf.detachAsData()  : nullptr;    
}

void DecSVGDom::exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook  ) {
    SkPixmap pixmap = getPixmap( dpi, hook );

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream out(filename);
    if (!out.isValid()) {
        SkDebugf( "Could not open export file: error on create file.\n" );
        return;
    }

    // Use default encoding options.
    SkPngEncoder::Options png_options;

    if (!SkPngEncoder::Encode(&out, pixmap, png_options)) {
        SkDebugf( "PNG encoding failed.\n" );
        return;
    }    
}

void DecSVGDom::saveSVG( const char *filename ) {
    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);

    sk_sp<SkData> data = getSVGData();
    file.write(data->bytes(), data->size()); 
}

void DecSVGDom::exportSVG( const char *filename ) {
    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);

    sk_sp<SkData> data = getSVGRenderData();
    file.write(data->bytes(), data->size()); 
}