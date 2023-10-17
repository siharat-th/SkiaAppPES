#ifndef __DEC_IMAGE_H__
#define __DEC_IMAGE_H__

#include "DecWidget.h"

class DecImage : public DecWidget {
public:    
    DecImage();
    virtual ~DecImage() override;

    virtual std::string type() override { return "Image"; }

    bool load( const char *filename );
    bool setData( sk_sp<SkData> data );
    bool setImage( sk_sp<SkImage> img );

    void enableFilter( bool f ) { _isFilterEnabled = f; }
    void setFilterColor( uint32_t c ) { _filterColor = c; }

    virtual int contentWidth() override;
    virtual int contentHeight() override;     

    sk_sp<SkImage> image;

    virtual void _draw_content(SkCanvas &canvas) override;
protected:

    uint32_t _filterColor;
    bool _isFilterEnabled;
};

DecImage &Image( const char *uri );

#endif //__DEC_IMAGE_H__
