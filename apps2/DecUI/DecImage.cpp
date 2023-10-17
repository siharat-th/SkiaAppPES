#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#include "include/core/SkColorFilter.h"
#include "tools/Resources.h"
#include "DecImage.h"
#include "DecUI.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

DecImage::DecImage() {
    // just-test-remove-later
    //cache(true);
    _isFilterEnabled = false;
    _filterColor = 0xFFFFFFFF;
}

DecImage::~DecImage() {

}

DecImage &Image( const char *uri ) {
    DecImage *w = new DecImage();
    w->load( uri );
    // just-test-remove-later
    w->text(uri);
    return *w;    
}

bool DecImage::setImage( sk_sp<SkImage> img ) {
    image = img;

    if (this->image) {
        if( _ow == 0 ) {
            _width = contentWidth();
        }
        if( _oh == 0 ) {
            _height = contentHeight();
        }
        return true;
    }
    return false;
}

bool DecImage::setData( sk_sp<SkData> data ) {
    sk_sp<SkImage> img = SkImage::MakeFromEncoded( data, std::nullopt );
    if( !img ) {
        return false;
    }
    return setImage(img);
}

bool DecImage::load( const char *filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecImage *widget = (DecImage *)fetch->userData;
            widget->setData( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename);
    return setData( data );
#endif
}

int DecImage::contentWidth() {
    if( !this->image ) {
        return 0;
    }
    return this->image->width();
}

int DecImage::contentHeight() {
    if( !this->image ) {
        return 0;
    }    
    return this->image->height();
}

void DecImage::_draw_content(SkCanvas &canvas) {
    if( image ) {
        SkPaint paint;
        paint.setAntiAlias(true);

        if( _isFilterEnabled ) {
            paint.setColorFilter(SkColorFilters::Blend(_filterColor, SkBlendMode::kModulate));
        }

        if( _ow == 0 || _oh == 0 ) {
            canvas.drawImage(image, (_width - image->width()) / 2, (_height - image->height()) / 2, SkSamplingOptions(), &paint);
            //just-test-remove-later
            //SkDebugf("drawImage:%s\n", _text.c_str());
        }
        else {
            float pw = (float)_width / image->width();
            float ph = (float)_height / image->height();
            float sc = std::min( pw, ph );
            float ww = image->width() * sc;
            float hh = image->height() * sc;
            SkRect rect = SkRect::MakeXYWH( (_width-ww)/2, (_height-hh)/2, ww, hh );
            canvas.drawImageRect( image, rect, SkSamplingOptions(SkFilterMode::kLinear), &paint );
            // just-test-remove-later
            //SkDebugf("drawImageRect:_width:%d, _height:%d, %s\n", _width, _height, _text.c_str());
        }
    }
}