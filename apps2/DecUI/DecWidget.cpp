#include <chrono>
#include <fstream>
#include <string>
#include <vector>
#include "DecUI.h"
#include "DecWidget.h"
#include "DecJob.h"
#include "include/core/SkSurface.h"
#include "include/core/SkEncodedImageFormat.h"
#include "include/core/SkData.h"
#include "include/core/SkRRect.h"
#include "tools/Resources.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "modules/skparagraph/include/TypefaceFontProvider.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

//std::vector<DecWidget *>DecWidget::widgets_storage;
DecWidget *DecWidget::_activeWidget = NULL;
DecWidget *DecWidget::_focusWidget = NULL;
DecWidget *DecWidget::_mouseCaptureWidget = NULL;
DecWidget *DecWidget::_mouseMoveWatcherHorizontal = NULL;
DecWidget *DecWidget::_mouseMoveWatcherVertical = NULL;

DecWidget::DecWidget( const char *_name ) {
    if( _name ) {
        name = _name;
    }

    _ui = NULL;
    _parent = NULL;
    _logicalParent = NULL;

    _x = 0; 
    _y = 0;
    _width = 0;
    _height = 0;    

    _ax = 0;
    _ay = 0;    

    __w = 0;
    __h = 0;

    _ox = 0;
    _oy = 0;
    _ow = 0;
    _oh = 0;  

    _tag = -1;

    // generic purpose - no main functionality
    _index = -1;
    _flag = 0;

    _maxWidth = 100000;
    _maxHeight = 100000;

    _round = 0;

    _align = ALIGN_TOPLEFT;

    _minWidth = 0;
    _minHeight = 0;  

    _textSize = 14;

    _border = true;
    _borderWidth = 1;
    _border_color = 0xFF000000;
    _focus_borderWidth = 3;
    _focus_border_color = SK_ColorGREEN;

    _background = true;
    _bg_color = 0xFFFFFFFF;  
    _bg_color_pressed = 0;

    _text_color = 0xFF000000;

    _draw_func = NULL;

    _isTimeoutActive = false;

    _isActive = false;
    _isFocus = false;
    _isFocusable = false;
    _isVisible = true;
    _isMouseCapture = false;
    _isMouseMoveWatcherHorizontal = false;
    _isMouseMoveWatcherVertical = false;
    _isDraggableX = false;
    _isDraggableY = false;
    _isDragging = false;
    _isLimitPosn = false;
    _isSubWidget = false;
    _isPressed = false;
    _isSelected = false;
    _isCache = false;
    _isDirty = false;
    _isDirtyLayout = 1;
    _isModal = false;

    _captureNextFrame = false;
    _captureBorder = true;
    _captureChildren = true;
    _captureFilename = "_images/test.png";

    _minX = -10000;
    _maxX = 10000;
    _minY = -10000;
    _maxY = 10000;

    _font = NULL;
    _ofont = NULL;

    _testDraw = true;

    _value = 0;

    _externalData = NULL;
    _externalWidget = NULL;

    _onMousePressed = nullptr;
    _onMouseReleased = nullptr;
    _onValueChanged = nullptr;

    _jobFrameSkip = 20;
    _jobFrameCount = 0;

    _isJobActive = false;
    _jobOnFinishCallback = nullptr;

    for( int i=0; i<4; i++ ) {
        _margin[i] = 0;
        _padding[i] = 0;
    }
}

DecWidget::~DecWidget() {

}

DecWidget &Widget( const char *_name ) {
    DecWidget *w = new DecWidget(_name);
    return *w;
}

void DecWidget::init() {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->init();
    }
}

DecWidget &DecWidget::onMousePressed(std::function<bool(DecWidget &widget)> callback) {
    _onMousePressed = callback;
    return *this;
}

DecWidget &DecWidget::onMouseReleased(std::function<bool(DecWidget &widget)> callback) {
    _onMouseReleased = callback;
    return *this;
}

DecWidget& DecWidget::onValueChanged(std::function<void(DecWidget& widget)> callback) {
    _onValueChanged = callback;
    return *this;    
}

void DecWidget::setTimeOut( double milliseconds, std::function<void(DecWidget &widget)>callback ) {
    _timeoutCallback = callback;

    _isTimeoutActive = true;
    _timeoutPeriod = milliseconds / 1000.0;
    _beginTimeout = std::chrono::high_resolution_clock::now();
}

DecWidget& DecWidget::cache(bool b) {
    if (_isCache) {
        if (_cacheSurface) {
            if (_cacheSurface->width() != _width || _cacheSurface->height() != _height) {
                //if (type() == "DecSVGDomEditor")
                //    SkDebugf("DecWidget::cache: %s, '%s', size:[%d, %d] _isCache:%d, _isDirty:%d, _cacheSurface:%d.\n", type().c_str(), _text.c_str(), width(), height(), _isCache, _isDirty, !!_cacheSurface );

                _cacheSurface = nullptr;
            }
        }
    }
    if (b) {
        //_cacheSurface = DecMakeRasterSurface(width(), height());

        // just-test-remove-later
        //if (!_cacheSurface) {
        //    //SkDebugf("widget: %s, size:[%d, %d] make surface fail.\n", type().c_str(), width(), height());
        //}
        dirty(true, true);
    }
    _isCache = b;
    return *this;
}

DecWidget& DecWidget::hideOnFlag( bool flag ) {
#ifdef __EMSCRIPTEN__
    if( flag  ) {
        visible( false );
    }
#endif
    return *this; 
}

DecWidget& DecWidget::hideOnWeb() { 
#ifdef __EMSCRIPTEN__
    visible(false); 
#endif
    return *this; 
}

bool DecWidget::cascadeAction( int actionID, DecWidget *sender, void *data ) {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( !w->cascadeAction(actionID, sender, data ) ) {
            return false;
        }
    }
    return true;
}

void DecWidget::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( _parent ) {
        _parent->bubbleAction( actionID, sender, data );
    }
}

void DecWidget::setParent( DecWidget *c ) {
    if( _parent ) {
        _parent->removeChild( this );
    }
    _parent = c;
    
    if( _parent ) {
        setUI( _parent->_ui );
        setFont( _parent->font() );
        _ax = _parent->_ax + _x;
        _ay = _parent->_ay + _y;
    }
    else {
        setUI( NULL );
        _ax = _x;
        _ay = _y;
    }
}

DecWidget &DecWidget::font( DecFont &font ) {
    _font = &font;
    _ofont = &font;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->setFont( _font );
    }
    return *this;
}

void DecWidget::moveModal(DecStackContainer *modalParent) {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w->_isModal ) {
            modalParent->_(*w);
            i--;
        }
    }
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->moveModal(modalParent);
    }    
}

void DecWidget::setFont( DecFont *f ) {
    if( _ofont ) {
        return;
    }
    _font = f;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->setFont( _font );
    }
}

DecWidget &DecWidget::x( int ix ) { 
    if( _parent ) {
        _ax = _parent->_ax + ix;
    }
    else {
        _ax = ix;
    }
    _x = ix; 
    _ox = ix;
    return *this; 
}

DecWidget &DecWidget::y( int iy ) { 
    if( _parent ) {
        _ay = _parent->_ay + iy;
    }
    else {
        _ay = iy;
    }
    _y = iy; 
    _oy = iy;
    return *this; 
}

DecWidget &DecWidget::width( int w ) { 
    if( w == 0 ) {
        _width = contentWidth() + _padding[1] + _padding[3];
    }
    else {
        _width = w;
    }
    _ow = w;
    return *this; 
}

DecWidget &DecWidget::height( int h ) { 
    if( h == 0 ) {
        _height = contentHeight() + _padding[0] + _padding[2];
    }
    else {
        _height = h;
    }    
    _oh = h;
    return *this; 
}

int DecWidget::contentX() {
    return _padding[3];
}

int DecWidget::contentY() {
    return _padding[0];
}

int DecWidget::contentWidth() { 
    if( _ow == 0 ) {
        int cw = 0;
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];
            if( w->_isVisible == false ) {
                continue;
            }
            int ww = w->x() + w->width();
            if( w->_align != ALIGN_ABSOLUTE && w->_align != ALIGN_CENTER ) {
                ww += w->_margin[1];
            }
            if( ww > cw ) {
                cw = ww;
            }
        }
        return cw;        
    }
    return _width - (_padding[1] + _padding[3]); 
}

int DecWidget::contentHeight() { 
    if( _oh == 0 ) {
        int ch = 0;
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];
            if( w->_isVisible == false ) {
                continue;
            }
            int wh = w->y() + w->height();
            if( w->_align != ALIGN_ABSOLUTE && w->_align != ALIGN_CENTER ) {
                wh += w->_margin[2];
            }            
            if( wh > ch ) {
                ch = wh;
            }
        }
        return ch;        
    }    
    return _height - (_padding[0] + _padding[2]); 
}   

DecWidget &DecWidget::align( int a ) {
    _align = a;
    return *this;     
}

void DecWidget::adjustX( int ix ) {
    _x = ix;
    if( _parent ) {
        _ax = _parent->_ax + _x;
    }
    else {
        _ax = _x;
    }    
}

void DecWidget::adjustY( int iy ) {
    _y = iy;
    if( _parent ) {
        _ay = _parent->_ay + _y;
    }
    else {
        _ay = _y;
    }    
}

void DecWidget::adjustWidth( int w ) {
    if( _ow == 0 || _ow == -1 ) {
        if( w != _width ) {
            //dirty( true, true );
        }
        _width = w;
        if( _width > _maxWidth ) {
            _width = _maxWidth;
        }      
        _updateChildrenUI( 0 );
        _updateChildrenAlignment(); 

    }
}

void DecWidget::adjustHeight( int h ) {
    if( _oh == 0 || _oh == -1 ) {
        if( h != _height ) {
            //dirty( true, true );
        }
        _height = h;
        if( _height > _maxHeight ) {
            _height = _maxHeight;
        }             
        _updateChildrenUI( 0 );
        _updateChildrenAlignment();   
    }
}

DecWidget &DecWidget::size( int w, int h ) { 
    width( w );
    height( h );
    //onResized(w, h);
    return *this; 
}

DecWidget &DecWidget::bounds( int ix, int iy, int w, int h ) { 
    x(ix); 
    y(iy); 
    width( w );
    height( h );
    return *this; 
}

DecWidget &DecWidget::gap( int g ) {
    _setGap( g );
    return *this; 
}

DecWidget &DecWidget::round( int r ) {
    _round = r;
    return *this;
}

DecWidget &DecWidget::onDraw(void (*draw_func)(DecWidget &widget, SkCanvas &canvas)) {
    _draw_func = draw_func;
    return *this;
}

void DecWidget::setUI( DecUI *ui ) { 
    _ui = ui; 
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->setUI( _ui );
    }    
}

DecWidget &DecWidget::child( DecWidget &c, bool front ) {
    if( c._isModal ) {
        if( _ui && _ui->modalWidget ) {
            if( this != _ui->modalWidget ) {
                _ui->modalWidget->_(c, front);
                return *this;
            }
        }
    }
    
    c.setParent( this );
    if( front ) {
        _children.insert(_children.begin(), 1, &c);
    }
    else {
        _children.push_back( &c );
    }
    fireOnResized();
    return *this;
}

void DecWidget::removeChild( DecWidget *c ) {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w == c ) {
            c->_parent = NULL;
            _children.erase(std::remove(_children.begin(), _children.end(), c), _children.end());
            return;
        }
    }
}

DecWidget &DecWidget::margin( int t, int r, int b, int l ) {
    _margin[0] = t;
    _margin[1] = r;
    _margin[2] = b;
    _margin[3] = l;
    return *this;    
}

DecWidget &DecWidget::margin_top( int t ) {
    _margin[0] = t;
    return *this;    
}

DecWidget &DecWidget::margin_right( int r ) {
    _margin[1] = r;
    return *this;    
}

DecWidget &DecWidget::margin_bottom( int b ) {
    _margin[2] = b;
    return *this;    
}

DecWidget &DecWidget::margin_left( int l ) {
    _margin[3] = l;
    return *this;    
}

DecWidget &DecWidget::padding( int t, int r, int b, int l ) {
    _padding[0] = t;
    _padding[1] = r;
    _padding[2] = b;
    _padding[3] = l;
    return *this;    
}

DecWidget &DecWidget::padding_top( int t ) {
    _padding[0] = t;
    return *this;    
}

DecWidget &DecWidget::padding_right( int r ) {
    _padding[1] = r;
    return *this;    
}

DecWidget &DecWidget::padding_bottom( int b ) {
    _padding[2] = b;
    return *this;    
}

DecWidget &DecWidget::padding_left( int l ) {
    _padding[3] = l;
    return *this;    
}

void DecWidget::moveX( int ix ) {
    x( _x + ix );
}

void DecWidget::moveY( int iy ) {
    y( _y + iy );
}

void DecWidget::move( int ix, int iy ) {
    x( _x + ix );
    y( _y + iy );
}

void DecWidget::update() {
    if( _isTimeoutActive ) {
        std::chrono::high_resolution_clock::time_point _currentTimeout = std::chrono::high_resolution_clock::now();
        auto elapsed =
                std::chrono::duration_cast<std::chrono::nanoseconds>(_currentTimeout - _beginTimeout);
        double elapse_time = elapsed.count() * 1e-9;
        //SkDebugf( "TimeOut active %02f, %02f\n", (float)elapse_time, (float)_timeoutPeriod );
        if( elapse_time >= _timeoutPeriod ) {
            _isTimeoutActive = false;
            if( _timeoutCallback ) {
                _timeoutCallback( *this );
            }
        }
    }

    _jobFrameCount++;
    if( (_jobFrameCount % _jobFrameSkip) == 0 ) {
        if( _jobStack.size() > 0 ) {
            DecJob *job = _jobStack[_jobStack.size()-1];
            if( job->_jobProc( *this ) ) {
                _jobStack.erase(std::remove(_jobStack.begin(), _jobStack.end(), job), _jobStack.end());
            }
        }
        else {
            _isJobActive = false;
            if( _jobOnFinishCallback ) {
                _jobOnFinishCallback( *this );
                _jobOnFinishCallback = nullptr;
            }
        }
    }
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->update();
    }     
}

void DecWidget::fireOnResized() {
    onResized( width(), height() );
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->fireOnResized();
    } 
}

void DecWidget::onResized(int w, int h) { 
    //UI()->invalidate();     
}

bool DecWidget::updateUI( int level ) {
    //if( !_isVisible ) {
    //    return false;
    //}
    /*
    if( type() == "Stack" ) {
        SkDebugf( "UpdateUI:Stack\n" );
        if( ID().length() == 0 ) {
            SkDebugf( "noName\n" );
        }
    }
    */

    //if( _isCache ) 
    {
        if( !_isDirtyLayout ) {
            return false;
        }
        //_isDirtyLayout = 0;
    }  
    /*  
    if( type() == "Stack" ) {
        SkDebugf( "UpdateUI:Stack\n" );
        if( ID() == "MP3 Container" ) {
            SkDebugf( "MP3 Container\n" );
        }
    }    
    
    SkDebugf( "UpdateUI: %s %s %i %i\n", type().c_str(), ID().c_str(), _isCache, _isDirtyLayout );
    if( _children.size() == 0 ) {
        SkDebugf( "--\n" );
    }
    */
    _preUpdateUI();
    _updateChildrenUI( level );
    //if (type() == "Pagesx")
    //    _updateChildrenUI( level );
    _postUpdateUI();
    _updateChildrenAlignment();

    if( _isDirtyLayout ) {
        _isDirtyLayout--;
    }    

    if( (__w != _width || __h != _height) ) {
        //onResized( _width, _height );
        __w = _width;
        __h = _height;

        if( _isDirtyLayout == 0 ) {
            invalidate();
            //fixed: infinite dirty 
            //dirty(true, true);
        }


        /*
        //// if (_isCache && _isDirtyLayout < 1) {
        //if (_isCache) {
        //    cache( true );
        //}
        //dirty(true, true);
        //// dirtyChildren( true, true );

        //_isDirty = true;
        _isDirtyLayout = true;
        //_cacheSurface = nullptr;
        for (DecWidget* w : _children) {
            //if (!w->_isVisible)
            //    continue;
            //w->_isDirty = 1;
            w->_isDirtyLayout = true;
            //w->_cacheSurface = nullptr;
        }
        */

        return true;
    }    
    return false;
}

void DecWidget::updateAbsPosn() {
    if( !_isVisible ) {
        return;
    }      
    if( _parent ) {
        _ax = _parent->_ax + _x;
        _ay = _parent->_ay + _y;
    }
    else {
        _ax = _x;
        _ay = _y;
    }
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->updateAbsPosn();
    }    
}

void DecWidget::_setGap( int g ) {
    _gap = g;
}

void DecWidget::_updateChildrenAlignment() {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w->_isVisible == false ) {
            continue;
        }
        w->_updateAlignmentX();
        w->_updateAlignmentY();
    }
}

void DecWidget::_updateAlignmentX() {
    if( _align == ALIGN_ABSOLUTE ) {
        return;
    }
    if( (_align & _ALIGN_LEFT) == _ALIGN_LEFT ) {
        if( _parent ) {
            x( _parent->_padding[3] + _margin[3] );
        }
        else {
            x( _margin[3] );
        }
    }
    else if( (_align & _ALIGN_CENTER) == _ALIGN_CENTER ) {
        x( (_parent->width()-width()) / 2 );
    }
    else if( (_align & _ALIGN_RIGHT) == _ALIGN_RIGHT ) {
        if( _parent ) {
            x( (_parent->width()-width()) - (_parent->_padding[1] + _margin[1]) );
        }
        else {
            x( (_parent->width()-width()) - _margin[1] );
        }
    }
}

void DecWidget::_updateAlignmentY() {
    if( (_align & _ALIGN_TOP) == _ALIGN_TOP ) {
        if( _parent ) {
            y( _parent->_padding[0] + _margin[0] );
        }
        else {
            y( _margin[0]);
        }        
    }
    else if( (_align & _ALIGN_VCENTER) == _ALIGN_VCENTER ) {
        y( (_parent->height()-height()) / 2 );
    }
    else if( (_align & _ALIGN_BOTTOM) == _ALIGN_BOTTOM ) {
        if( _parent ) {
            y( (_parent->height()-height()) - (_parent->_padding[2] + _margin[2]) );
        }
        else {
            y( (_parent->height()-height()) - _margin[2] );
        }
    }
}

DecWidget &DecWidget::max_width( int mw ) {
    _maxWidth = mw;
    return *this;
}

DecWidget &DecWidget::max_height( int mh ) {
    _maxHeight = mh;
    return *this;
}

void DecWidget::_preUpdateUI() {
    x(_ox);
    y(_oy);

    if( _ow == -1 ) {
        if( _parent ) {
            if( _align == ALIGN_ABSOLUTE ) {
                _width = (_parent->width() - _x);
            }
            else {
                _width = (_parent->contentX() + _parent->contentWidth()) - _x - _margin[1];
            }
        }
        else {
            if( _ui ) {
                _width = _ui->width() - _x;
            }
            else {
                _width = 0;
            }
        }
        if( _width > _maxWidth ) {
            _width = _maxWidth;
        }
    } else {
        _width = _ow;
    }

    if( _oh == -1 ) {
        if( _parent ) {
            if( _align == ALIGN_ABSOLUTE ) {
                _height = (_parent->height() - _y);
            }
            else {            
                _height = (_parent->contentY() + _parent->contentHeight()) - _y - _margin[2];
            }
        }
        else {
            if( _ui ) {
                _height = _ui->height() - _y;
            }
            else {
                _height = 0;
            }
        }
        if( _height > _maxHeight ) {
            _height = _maxHeight;
        }
    } else {
        _height = _oh;
    }    
}

void DecWidget::_updateChildrenUI( int level ) {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->updateUI( level );
    }
}



void DecWidget::_postUpdateUI() {
    if( _ow == 0 ) {
        _width = contentWidth() + _padding[1]; // + _padding[3];
        if( _width > _maxWidth ) {
            _width = _maxWidth;
        }        
    }
    if( _oh == 0 ) {
        _height = contentHeight() + _padding[2]; // + _padding[0];
        if( _height > _maxHeight ) {
            _height = _maxHeight;
        }        
    }
}

void DecWidget::pre_draw(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }
    canvas.save();
    canvas.clipRect( SkRect::MakeXYWH( _x, _y, _width, _height ) );
    canvas.translate( _x, _y );
}

void DecWidget::post_draw(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }
    canvas.restore();
}

void DecWidget::render(SkCanvas &canvas) {
    _draw_bg( canvas );
    if( _draw_func ) {
        _draw_func( *this, canvas );
    }
    else {
        _draw_content( canvas );
    }
    canvas.save();
    if (_round ) {
        if( !_captureNextFrame || _captureBorder ) {
            SkRRect rrect;
            rrect.setRectXY(SkRect::MakeWH(_width, _height), _round, _round);
            canvas.clipRRect(rrect, true);
        }
    }
    if( !_captureNextFrame || _captureChildren ) {
        _draw_children( canvas );
    }
    canvas.restore();
    _draw_border( canvas );
}

void DecWidget::draw(SkCanvas& canvas, int level) {
    if (!_isVisible) {
        _isDirty = false;
        return;
    }
    _drawLevel = level + 1;

    const int w = width(), h = height();
    if (w <= 0 || h <= 0) {
        _isDirty = false;
        return;
    }

    //if (type() == "DecSVGDomEditor")
    //    SkDebugf("widget: %s, '%s', size:[%d, %d] _isCache:%d, _isDirty:%d, _cacheSurface:%d.\n", type().c_str(), _text.c_str(), width(), height(), _isCache, _isDirty, !!_cacheSurface );

    if (_isCache) {
        if (_isDirty) {
            // SkDebugf( "Clear dirty\n" );
            if (!_cacheSurface) {
                //const int area = w * h;
                //const bool _isUseRasterSurface = area <= 10000;
                //if (_isUseRasterSurface) {
                //    _cacheSurface = DecMakeRasterSurface(w, h);
                //} else {
                    // create surface associates it with SkCanvas.           
                    if (w > 0 && h > 0) {
                        _cacheSurface = canvas.makeSurface(canvas.imageInfo().makeDimensions({w,h}));
                    }
                //}
        
                // just-test-remove-later
                //if (!_cacheSurface) {
                //    SkDebugf("widget: %s, '%s', size:[%d, %d] make surface fail.\n", type().c_str(), _text.c_str(), width(), height());
                //} else {
                //    SkDebugf("widget: %s, '%s', size:[%d, %d] make surface.\n", type().c_str(), _text.c_str(), width(), height());
                //}
            }
        
            if (_cacheSurface) {
                SkCanvas* pCanvas = _cacheSurface->getCanvas();
                pCanvas->clear(0x00000000);
                render(*pCanvas);
                SkIRect rect = SkIRect::MakeXYWH(0, 0, w, h);
                _cacheImage = _cacheSurface->makeImageSnapshot(rect);
                _isDirty = false;
            }
        }
        
        //if (type() == "DecSVGDomEditor")
        //    SkDebugf("widget: %s, '%s', size:[%d, %d] _isCache:%d, _isDirty:%d, _cacheSurface:%d.\n", type().c_str(), _text.c_str(), width(), height(), _isCache, _isDirty, !!_cacheSurface );
        if (_cacheImage) {
            canvas.drawImage(_cacheImage, 0, 0);
        } else {
            render(canvas);
            _isDirty = false;
        }
        
        /*
        if (_cacheImage) {
            if (_cacheImage->width() != w || _cacheImage->height() != h) {
                _isDirty = true;
            }
        }
        if (_isDirty || !_cacheImage) {
            render(canvas);
            const int _scale = _ui->_scale;
            _cacheImage = canvas.
                getSurface()->
                    makeImageSnapshot(
                    SkIRect::MakeXYWH(
                        _ax * _scale, 
                        _ay * _scale, 
                        _width * _scale, 
                        _height * _scale));
            _isDirty = false;
        } else {
            canvas.drawImage(_cacheImage, 0, 0);
        }
        */
    } else {
        // char chDeep[] = "--------------------------------------------------";
        // chDeep[_drawLevel+1] = 0;
        // SkDebugf( "Draw: %s%s\n",chDeep, type().c_str() );

        render(canvas);
        _isDirty = false;
    }

    if (_captureNextFrame) {
        SkSurface* surface = canvas.getSurface();
        int _scale = _ui->_scale;
        SkIRect rect =
                SkIRect::MakeXYWH(_ax * _scale, _ay * _scale, _width * _scale, _height * _scale);
        sk_sp<SkImage> img = surface->makeImageSnapshot(rect);
        int quality = 100;
        sk_sp<SkData> data = img->encodeToData(SkEncodedImageFormat::kPNG, quality);

        std::ofstream f(_captureFilename.c_str());
        if (f) {
            f.write((const char*)data->bytes(), data->size());
            f.close();
        }

        // sk_sp<SkImage>img = canvas.getSurface()->makeImageSnapshot();
        // int quality = 100;
        // sk_sp<SkData>data = img->encodeToData( SkEncodedImageFormat::kPNG, quality );

        _captureNextFrame = false;
    }
}

void DecWidget::_draw_border(SkCanvas& canvas) {
    if (!_border) {
        return;
    }
    SkPaint paint;

    if (_isFocus) {
        paint.setStrokeWidth(_focus_borderWidth);
        paint.setColor(_focus_border_color);
    } else {
        paint.setStrokeWidth(_borderWidth);
        paint.setColor(_border_color);
    }
    paint.setStyle(SkPaint::Style::kStroke_Style);

    // if( _round ) {
    //    paint.setAntiAlias( true );
    //    float b2 = _borderWidth / 2.0;
    //    canvas.drawRoundRect( SkRect::MakeXYWH( b2, b2, _width-_borderWidth, _height-_borderWidth
    //    ), _round, _round, paint );
    //}
    // else {
    //    canvas.drawRect( SkRect::MakeXYWH( 0, 0, _width, _height ), paint) ;
    //}

    const SkScalar stroke_halfw = paint.getStrokeWidth() / 2;
    SkRect border_rc = SkRect::MakeWH(_width, _height);
    border_rc.inset(stroke_halfw, stroke_halfw);

    if (_round ) {
        if( !_captureNextFrame || _captureBorder ) {
            paint.setAntiAlias(true);
            canvas.drawRoundRect(border_rc, _round, _round, paint);
        }
    } else {
        canvas.drawRect(border_rc, paint);
    }
}

void DecWidget::capture( const char *filename, bool border, bool children ) { 
    _captureFilename = std::string( filename );
    _captureNextFrame = true; 
    _captureBorder = border;
    _captureChildren = children;
}

void DecWidget::_draw_bg(SkCanvas &canvas) {
    if( !_background ) {
        return;
    }
    SkPaint paint;

    paint.setColor( _bg_color );
    paint.setStyle( SkPaint::Style::kFill_Style );

    if( _round ) {
        paint.setAntiAlias( true );
        canvas.drawRoundRect( SkRect::MakeXYWH( 0, 0, _width, _height ), _round, _round, paint );
    }
    else {
        canvas.drawRect( SkRect::MakeXYWH( 0, 0, _width, _height ), paint) ;
    }
}

DecWidget &DecWidget::fontSize( const int s ) {
    _textSize = s;
    return *this;
}

void DecWidget::_draw_content(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }
    if( !_testDraw ) {
        return;
    }

    SkPaint paint;

    paint.setColor( _border_color );
    paint.setStyle( SkPaint::Style::kStroke_Style );
    paint.setAntiAlias( true );

    canvas.drawRect( SkRect::MakeXYWH( 10, 10, width()-20,height()-20 ), paint) ;

    canvas.drawLine( SkPoint::Make(0, 0), SkPoint::Make(_width, _height), paint );
    canvas.drawLine( SkPoint::Make(_width, 0), SkPoint::Make(0, _height), paint );        
/*
    SkPaint paint;

    paint.setColor( _border_color );
    paint.setStyle( SkPaint::Style::kStroke_Style );
    paint.setAntiAlias( true );

    canvas.drawRect( SkRect::MakeXYWH( 10, 10, width()-20,height()-20 ), paint) ;

    canvas.drawLine( SkPoint::Make(0, 0), SkPoint::Make(_width, _height), paint );
    canvas.drawLine( SkPoint::Make(_width, 0), SkPoint::Make(0, _height), paint );  

    canvas.save();
    canvas.translate( 0, 0 );
    const char* text = "Hello world. this  👩🏾‍⚕️ 👨🏾‍⚕️ 👩🏾‍🌾 👨🏾‍🌾  is a paragraph. Hello world. this is a paragraph. Hello world. this is a paragraph. สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก เป็นมนุษย์สุดประเสริฐเลิศคุณค่า กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ";

      std::string fonts = GetResourcePath("fonts/").c_str();
      std::set<std::pair<std::string, std::string>> font_paths = {
          //{"Roboto", "Roboto-Regular.ttf"},
          //{"Roboto", "Roboto-Bold.ttf"},
          {"Kanit","Kanit-Regular.ttf"},
          {"Kanit", "Kanit-Bold.ttf"},
          {"Emoji","NotoColorEmoji.ttf"}};

      sk_sp<skia::textlayout::TypefaceFontProvider> font_provider = sk_make_sp<skia::textlayout::TypefaceFontProvider>();

      for (auto& pair : font_paths) {
        SkString family_name = SkString(pair.first.c_str());
        std::string path = fonts;
        path += pair.second;

        auto data = SkData::MakeFromFileName(path.c_str());
        font_provider->registerTypeface(SkTypeface::MakeFromData(std::move(data)), family_name);
      }

    auto fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setAssetFontManager(std::move(font_provider));
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    skia::textlayout::ParagraphStyle paragraph_style;
    paragraph_style.turnHintingOff();

    skia::textlayout::ParagraphBuilderImpl builder(paragraph_style, fontCollection);
    builder.addText(text);
    auto paragraph = builder.Build();
    canvas.restore();

        
        //SkPictureRecorder rec;
        //SkCanvas* canvas = rec.beginRecording({0,0, 2000,3000});
        

    paragraph->layout(width());
    //auto impl = static_cast<skia::textlayout::ParagraphImpl*>(paragraph.get());
    paragraph->paint(&canvas, 0, 0);
    paragraph->markDirty();
    //impl->resetCache();
*/

  /*
    // SkiaApps/modules/skparagraph/samples/SampleParagraph.cpp
    using namespace skia::textlayout;

    auto drawLine = [=](SkCanvas *canvas, SkScalar w, SkScalar h, const std::string& text, TextAlign align) {
        SkAutoCanvasRestore acr(canvas, true);
        //canvas->save();
        canvas->clipRect(SkRect::MakeWH(w, h));
        canvas->drawColor(SK_ColorWHITE);

        SkScalar margin = 20;

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorBLUE);

        SkPaint gray;
        gray.setColor(SK_ColorLTGRAY);

        TextStyle style;
        style.setBackgroundColor(gray);
        style.setForegroundColor(paint);
        style.setFontFamilies({SkString("Kanit"), SkString("Emoji")});
        style.setFontSize(12);
        ParagraphStyle paraStyle;
        paraStyle.setTextStyle(style);
        paraStyle.setTextAlign(align);


        ParagraphBuilderImpl builder(paraStyle, _ui->fontCollection);
        builder.addText(text.c_str(), text.length());

        auto paragraph = builder.Build();
        paragraph->layout(w - margin * 2);
        //paragraph->layout(w - margin);
        paragraph->paint(canvas, margin, margin);

        //canvas->translate(0,  margin * 5);
        //canvas->restore();
    };
    

    auto drawTest = [](SkCanvas* canvas, SkScalar w, SkScalar h, SkColor fg, SkColor bg) {
        const std::vector<std::tuple<std::string, bool, bool, int, SkColor, SkColor, bool, TextDecorationStyle>> gParagraph = {
            {"monospace", true, false, 20, SK_ColorWHITE, SK_ColorRED, false, TextDecorationStyle::kSolid},
            {"monospace", false, false, 20, SK_ColorWHITE, SK_ColorBLUE, false, TextDecorationStyle::kSolid},
            {"monospace", false, true, 20, SK_ColorWHITE, SK_ColorGREEN, false, TextDecorationStyle::kSolid},
            //{"serif", true, true, 10, SK_ColorWHITE, SK_ColorRED, true, TextDecorationStyle::kDouble},
            //{"Arial", false, true, 16, SK_ColorGRAY, SK_ColorGREEN, true, TextDecorationStyle::kSolid},
            //{"sans-serif", false, false, 8, SK_ColorWHITE, SK_ColorRED, false, TextDecorationStyle::kWavy},
        };
        SkAutoCanvasRestore acr(canvas, true);

        canvas->clipRect(SkRect::MakeWH(w, h));
        canvas->drawColor(SK_ColorWHITE);

        SkScalar margin = 20;

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(fg);

        SkPaint blue;
        blue.setColor(SK_ColorBLUE);

        TextStyle defaultStyle;
        // defaultStyle.setBackgroundColor(blue);
        defaultStyle.setForegroundColor(paint);
        ParagraphStyle paraStyle;

        auto fontCollection = sk_make_sp<FontCollection>();
        fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
        for (auto i = 1; i < 2; ++i) {
            defaultStyle.setFontSize(24 * i);
            paraStyle.setTextStyle(defaultStyle);
            ParagraphBuilderImpl builder(paraStyle, fontCollection);
            //std::string name = "Paragraph: " + std::to_string(24 * i);
            //builder.addText(name.c_str(), name.length());
            std::string name = "";
            for (auto para : gParagraph) {
                TextStyle style;
                style.setFontFamilies({SkString(std::get<0>(para).c_str())});
                SkFontStyle fontStyle(std::get<1>(para) ? SkFontStyle::Weight::kBold_Weight
                                                        : SkFontStyle::Weight::kNormal_Weight,
                                        SkFontStyle::Width::kNormal_Width,
                                        std::get<2>(para) ? SkFontStyle::Slant::kItalic_Slant
                                                        : SkFontStyle::Slant::kUpright_Slant);
                style.setFontStyle(fontStyle);
                style.setFontSize(std::get<3>(para) * i);
                SkPaint background;
                background.setColor(std::get<4>(para));
                style.setBackgroundColor(background);
                SkPaint foreground;
                foreground.setColor(std::get<5>(para));
                foreground.setAntiAlias(true);
                style.setForegroundColor(foreground);
                if (std::get<6>(para)) {
                    style.addShadow(TextShadow(SK_ColorBLACK, SkPoint::Make(5, 5), 2));
                }

                //auto decoration = (i % 4);
                //if (decoration == 3) {
                //    decoration = 4;
                //}
                //
                //bool test = (TextDecoration)decoration != TextDecoration::kNoDecoration;
                //std::string deco = std::to_string((int)decoration);
                //if (test) {
                //    style.setDecoration((TextDecoration)decoration);
                //    style.setDecorationStyle(std::get<7>(para));
                //    style.setDecorationColor(std::get<5>(para));
                //}
                builder.pushStyle(style);
                name = " " + std::get<0>(para) + " " + (std::get<1>(para) ? ", bold" : ", normal") +
                        (std::get<2>(para) ? ", italic" : "") + " " +
                        std::to_string(std::get<3>(para) * i) +
                        //(std::get<4>(para) != bg ? ", background" : "") +
                        //(std::get<5>(para) != fg ? ", foreground" : "") +
                        (std::get<6>(para) ? ", shadow" : "") +
                        //(test ? ", decorations " + deco : "") + ";";
                        ";";
                builder.addText(name.c_str(), name.length());
                builder.pop();
            }

            auto paragraph = builder.Build();
            paragraph->layout(w - margin * 2);
            paragraph->paint(canvas, margin, margin);

            canvas->translate(0, paragraph->getHeight());
        }
    };

    const char* text = "Hello world. this  👩🏾‍⚕️ 👨🏾‍⚕️ 👩🏾‍🌾 👨🏾‍🌾  is a paragraph. Hello world. this is a paragraph. Hello world. this is a paragraph. สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก เป็นมนุษย์สุดประเสริฐเลิศคุณค่า กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ";
    
    //drawLine(&canvas, width(), height(), std::string(text), TextAlign::kLeft);
    drawTest(&canvas, this->width(), this->height(), SK_ColorRED, SK_ColorWHITE);
  */
}

void DecWidget::_draw_children(SkCanvas &canvas) {
    if( !_isVisible ) {
        return;
    }

    SkRect rectClip;
    bool bClip = canvas.getLocalClipBounds( &rectClip );

    if (rectClip.width() == 0) 
        return;
    if (rectClip.height() == 0) 
        return;

    //const SkIRect bounds = SkIRect::MakeXYWH(0, 0, rectClip.width(), rectClip.height());

    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( !w->_isVisible ) {
            continue;
        }

        if (w->width() == 0) 
            continue;
        if (w->height() == 0) 
            continue;
        
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

        //SkIRect cBounds = SkIRect::MakeXYWH(0, 0, w->width(), w->height());
        //if (!cBounds.intersect(bounds)) {
        //    SkDebugf("[%.1f, %.1f] [%d, %d]\n", rectClip.width(), rectClip.height(), w->width(), w->height());
        //    continue;
        //}
        w->pre_draw( canvas );
        w->draw( canvas, _drawLevel );
        w->post_draw( canvas );
    }
}

void DecWidget::dirty( bool b, bool updateLayout ) { 
    //if( type() == "ComboBox" ) {
    //    SkDebugf( "Dirty:ComboBox\n" );
    //}
    //SkDebugf( "Dirty: %s %s %i %i\n", type().c_str(), ID().c_str(), _isCache, _isDirtyLayout );

    _isDirty = b; 
    if( updateLayout ) {
        _isDirtyLayout = 2;
    }
    if( b && _parent ) {
        _parent->dirty( b, updateLayout );
    }
}

void DecWidget::invalidate() {
    _isDirty = true;
    _isDirtyLayout = 1;
    _cacheSurface = nullptr;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->invalidate();
    }
}

/*
void DecWidget::dirtyChildren(bool b, bool updateLayout ) {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->dirtyChildren( b, updateLayout );
    }    
}
*/



DecWidget& DecWidget::active( bool a ) {
    if( _activeWidget ) {
        DecWidget *a = _activeWidget;
        _activeWidget = NULL;
        a->active(false);
    }
    _isActive = a;
    
    if( a ) {
        _activeWidget = this;

    }
    else {
        _activeWidget = NULL;
    }
    return *this;
}

void DecWidget::focus( bool f ) {
    // update: keep the last focus,  just update state on theirs
    /*
    if( _focusWidget ) {
        DecWidget *a = _focusWidget;
        _focusWidget = NULL;
        a->focus(false);
    }
    _isFocus = f;
    _focusWidget = this;
    if( f ) {
        _focusWidget = this;
    }
    else {
        _focusWidget = NULL;
    }
    */


    // onfocus
    if (f) {
        // blur the old one, if it has.
        if (this != _focusWidget) {
            if (_focusWidget) {
                //_focusWidget->focus(false);
                _focusWidget->_isFocus = false;
            }
            // establish here is a focus point to the public
            _focusWidget = this;
        }
        // set state 
        _isFocus = true;
        //SkDebugf("onfocus:%s\n", this->type().c_str());
    } 
    
    // onblur
    else {
        // set state
        _isFocus = false;
        //SkDebugf("onblur:%s\n", this->type().c_str());
    }
}

DecWidget & DecWidget::subWidget( bool f ) {
    _isSubWidget = f;
    return *this;      
}

DecWidget &DecWidget::draggableX( bool d ) {
    _isDraggableX = d;
    return *this;        
}

DecWidget &DecWidget::draggableY( bool d ) {
    _isDraggableY = d;
    return *this;        
}

DecWidget &DecWidget::draggable( bool d ) {
    _isDraggableX = d;
    _isDraggableY = d;
    return *this;        
}

void DecWidget::mouseCapture( bool c ) {
    if( c && _isMouseCapture ) {
        return;
    }
    if( _mouseCaptureWidget && _mouseCaptureWidget != this) {
        DecWidget *a = _mouseCaptureWidget;
        _mouseCaptureWidget = NULL;
        a->mouseCapture(false);
        a->_isPressed = false;
        a->dirty( true, false );
    }
    _isMouseCapture = c;

    if( c ) {
        _mouseCaptureWidget = this;
    }
    else {
        _mouseCaptureWidget = NULL;
    }
}

void DecWidget::mouseMoveWatchHorizontal( bool c ) {
    if( _mouseMoveWatcherHorizontal ) {
        DecWidget *a = _mouseMoveWatcherHorizontal;
        _mouseMoveWatcherHorizontal = NULL;
        a->mouseMoveWatchHorizontal(false);            
    }
    _isMouseMoveWatcherHorizontal = c;
    if( c ) {
        _mouseMoveWatcherHorizontal = this;
    }
    else {
        _mouseMoveWatcherHorizontal = NULL;
    }
}

void DecWidget::mouseMoveWatchVertical( bool c ) {
    if( _mouseMoveWatcherVertical ) {
        DecWidget *a = _mouseMoveWatcherVertical;
        _mouseMoveWatcherVertical = NULL;
        a->mouseMoveWatchVertical(false);            
    }
    _isMouseMoveWatcherVertical = c;
    if( c ) {
        _mouseMoveWatcherVertical = this;
    }
    else {
        _mouseMoveWatcherVertical = NULL;
    }
}

DecWidget& DecWidget::visible( bool v ) {
    _isVisible = v;
    return *this;
}

bool DecWidget::onMouseWheel(int ix, int iy, float deltax, float delta, skui::ModifierKey modifiers) {
    for( int i=_children.size()-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->hitTest( ix, iy ) ) {
            if( w->onMouseWheel( ix - w->x(), iy - w->y(), deltax, delta, modifiers ) ) {
                if (DecWidget::_focusWidget && DecWidget::_focusWidget != w) {
                    DecWidget::_focusWidget->focus(false);
                }
                return true;
            }
            return false;
        }
    }     
    return false;
}

bool DecWidget::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //SkDebugf( "onMouse: %s %s %i %i\n", type().c_str(), ID().c_str(), _isCache, _isDirtyLayout );

        if( _onMousePressed ) {
            if( _onMousePressed( *this ) )
                return true;
        }
        //active( true );
        if( _isDraggableX || _isDraggableY ) {
            mouseCapture( true );
            _isDragging = true;
            _lastMouseX = ix;
            _lastMouseY = iy;
            align( ALIGN_ABSOLUTE );
        }
        if( _isFocusable ) {
            focus( true );
        }        
    }
    else if( state == skui::InputState::kUp ) {
        if( _isDragging ) {
            _isDragging = false;
            mouseCapture( false );
            _endDrag();
        }
        if( _onMouseReleased ) {
            if( _onMouseReleased( *this ) )
                return true;
        }        
    }
    else if( state == skui::InputState::kMove ) {
        if( _isDragging ) {
            int diffX = ix - _lastMouseX;
            int diffY = iy - _lastMouseY;
            //_lastMouseX = ix;
            //_lastMouseY = iy;

            if( _isDraggableX ) {
                moveX( diffX );
                if( _isLimitPosn ) {
                    if( _x < _minX ) {
                        x( _minX );
                    }
                    else if( _x > _maxX ) {
                        x( _maxX );
                    }
                }
            }
            if( _isDraggableY ) {
                moveY( diffY );
                if( _isLimitPosn ) {
                    if( _y < _minY ) {
                        y( _minY );
                    }
                    else if( _y > _maxY ) {
                        y( _maxY );
                    }
                }                
            }
        }
    }
    for( int i=_children.size()-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->hitTest( ix, iy ) ) {
            if( w->onMouse( ix - w->x(), iy - w->y(), state, modifiers ) ) {
                return true;
            }
            this->setCursor("default");
            return false;
        }
    }  

    /*
    if( state == skui::InputState::kDown ) {
        if( _movable ) {
            if( _ui ) {
                _ui->captureMouse( this );
            }
            _moving = true;
            _lastX = ix;
            _lastY = iy;
            return true;
        }
    }
    else if( state == skui::InputState::kMove ) {
        if( _moving ) {
            x( _x + (ix - _lastX) );
            y( _y + (iy - _lastY) );
            //_lastX = x;
            //_lastY = y;
            return true;
        }
    }
    else if( state == skui::InputState::kUp ) {
        if( _moving ) {
            _moving = false;
            if( _ui ) {
                _ui->releaseMouse();
            }
            return true;
        }
    }
    */
    return false;
}

bool DecWidget::hitTest( int x, int y ) {
    //SkDebugf( "hittest: %s %s %s\n", type().c_str(), ID().c_str(), _text.c_str() );

    if( !_isVisible ) {
        return false;
    }

    if( (x > _x) && (x <= (_x + _width)) &&
        (y > _y) && (y <= (_y + _height)) ) {
            return true;
    }
    return false;
}

bool DecWidget::hitTestAbs( int x, int y ) {
    if( !_isVisible ) {
        return false;
    }

    if( (x > _ax) && (x <= (_ax + _width)) &&
        (y > _ay) && (y <= (_ay + _height)) ) {
            return true;
    }
    return false;
}

bool DecWidget::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool DecWidget::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false; 
}

bool DecWidget::onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    for (int i = _children.size() - 1; i >= 0; i--) {
        DecWidget* w = _children[i];
        if (w->hitTest(ix, iy)) {
            if (w->onDropFile(ix - w->x(), iy - w->y(), dropped_file, iptr, length)) {
                return true;
            }
            return false;
        }
    } 
    return false; 
}

void DecWidget::pushJob( DecJob *job, std::function<void(DecWidget& widget)>onFinishCallback ) {
    _isJobActive = true;
    if( _jobStack.empty() ) {
        _jobOnFinishCallback = onFinishCallback;
    }
    _jobStack.push_back( job ); 
}


void DecWidget::setCursor(std::string cursorStyle) {
#ifdef __EMSCRIPTEN__
    std::string strcanvasid = "canvas";
    // emscripten
    EM_ASM(
            {
                var id = '#' + UTF8ToString($0);
                var cursorStyle = UTF8ToString($1);
                var canvasElement = document.querySelector(id);
                if (!canvasElement) {
                    canvasElement = document.querySelector('canvas');
                }
                if (canvasElement) {
                    var cursor = canvasElement.style.cursor;
                    var cursordata = canvasElement.getAttribute('cursor-data');
                    if (!cursorStyle) {
                        cursor = "";
                        canvasElement.style.cursor = cursor;
                        cursorStyle = cursordata || 'default';
                    }
                    if (cursordata != cursorStyle) {
                        canvasElement.setAttribute('cursor-data', cursorStyle);
                    }
                    if (cursor != cursorStyle) {
                        canvasElement.style.cursor = cursorStyle;
                    }
                }
            },
            strcanvasid.c_str(),
            cursorStyle.c_str());
#else
    // native

#endif



}