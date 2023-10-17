#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "include/svg/SkSVGCanvas.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "include/encode/SkPngEncoder.h"
#include "src/utils/SkOSPath.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "MagicPatternSVG.h"

DecMagicPatternSVG::DecMagicPatternSVG( const char *uri, std::function<void(DecSVGDom &dom)> onload ) :
DecSVGDom( uri, onload ) {
    root = NULL;
    fScale = 1.0;

    imgLogo = new DecImage();
    imgLogo->load( "images/MP-LOGO-M.png");
}

DecMagicPatternSVG::~DecMagicPatternSVG() {

}

DecMagicPatternSVG &MagicPatternSVG( const char *uri, std::function<void(DecSVGDom &dom)> onload ) {
    DecMagicPatternSVG *w = new DecMagicPatternSVG( uri, onload );
    return *w;
}

void DecMagicPatternSVG::_draw_content(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }
    //SkDebugf( "SVG:draw_content\n" );

    if (fDom) {
        if( !root ) {
            root = fDom->getRoot();
        }

        SkSize sz = fDom->containerSize();

        //SkDebugf( "Scale: %0.2f\n", fScale );

        float scale = fScale;
        float dw = ((width() - sz.fWidth * scale)/2) / scale;
        float dh = ((height() - sz.fHeight * scale)/2) / scale;

        //SkDebugf( "Scale: %0.2f\n", scale );
        canvas.save();
        canvas.scale( scale, scale );
        canvas.translate( dw, 0 );

        fDom->render(&canvas);
        canvas.restore();
    }
    else {
        SkDebugf( "Loading...\n" );
        _text = "Loading...";

        SkFont font;
        font.setTypeface(_font->typeface(FONT_REGULAR));
        font.setSubpixel(true);
        font.setSize(24);

        SkFontMetrics metrics;
        font.getMetrics(&metrics);

        SkRect rect;
        int ww = font.measureText( _text.c_str(), _text.length(), SkTextEncoding::kUTF8, &rect ); 

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(_text_color);

        auto blob = SkTextBlob::MakeFromText(_text.c_str(), _text.length(), font);
        canvas.drawTextBlob(blob, (width()-ww)/2, height()/2, paint);        
    }
}

void DecMagicPatternSVG::exportPNG( int pid, const char *filename, float dpi, float _scale, std::function<void(SkCanvas &canvas)>hook  ) {
    SkSize szC = fDom->containerSize();  

    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }    

    float exportW = szR.fWidth* dpi / 72.0;
    float exportH = szR.fHeight * dpi / 72.0;

    auto surface = SkSurface::MakeRasterN32Premul(exportW, exportH);

    float dw = 0;
    float dh = 0;

    float scale = (exportW / szC.fWidth) * _scale;

    SkCanvas *canvas = surface->getCanvas();
    canvas->clear( 0xFFFFFFFF );
    canvas->save();
    canvas->translate( 0-(((szC.fWidth * scale) - exportW) / 2), 0 );
    canvas->scale( scale, scale );
    fDom->render( canvas );
    if( hook ) {
        hook( *canvas );
    } 
    canvas->restore();

    canvas->save();
    canvas->translate( exportW - imgLogo->width() - 10, 8 );
    imgLogo->_draw_content( *canvas );   
    canvas->restore();

    sk_sp<SkTypeface> typeface = _font->typeface(FONT_REGULAR);
    SkFont font;
    font.setTypeface(typeface);
    font.setSubpixel(true);
    font.setSize(16);

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0xFF808080);

    char sz[64];
    sprintf( sz, "#%04i", pid );

    int textwidth = ceil( font.measureText(sz, strlen(sz), SkTextEncoding::kUTF8) );
    canvas->drawSimpleText( sz, strlen(sz), SkTextEncoding::kUTF8, exportW-textwidth-10, exportH-10, font, paint );


    SkPixmap pixmap;
    surface->peekPixels(&pixmap);

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

void DecMagicPatternSVG::exportSVG(const char* filename) {
    if (!fDom) {
        return;
    }

    // SkSize containerSize = fDom->containerSize();
    //SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 95.997395));
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
    if (data.get() == nullptr) return;
    //SkDebugf("rawOutput:%.*s\n", (int)data->size(), (const char*)data->data());


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
    if (!file.isValid()) {
        SkDebugf("Could not open export file: error on create file.\n");
        return;
    }
    file.write(data->bytes(), data->size());

}