#include "DecScroller.h"

DecScroller::DecScroller( int scroll_type ) {
    _scrollType = scroll_type;
    _isPressed = false;
    _firstMove = false;

    _lastMoveX = 0;
    _lastMoveY = 0;

    pScroll2Widget = NULL;

    border( false );
    background( false );
}

DecScroller::~DecScroller() {

}

DecScroller &Scroller( int scroll_type ) {
    DecScroller *w = new DecScroller( scroll_type );
    return *w; 
}

void DecScroller::update() {
    DecWidget::update();
    if( !_isPressed ) {
        if( _lastMoveX || _lastMoveY ) {
            moveXY( _lastMoveX, _lastMoveY );
        }
        else {
            if( _children.size() ) {
                for( int i=0; i<_children.size(); i++ ) {
                    DecWidget *w = _children[i];  
                    if( w->width() < width() && w->x() != 0 ) {
                        w->x(0);
                    }   
                    if( w->height() < height() && w->y() != 0 ) {
                        w->y(0);
                    }                                
                }
            }
            else {
                _lastMoveX = 0;
                _lastMoveY = 0;
            }
        }
    }

    if (pScroll2Widget) {
        const float ay0 = _ay;
        const float ay1 = ay0 + height();

        const float w_ay0 = pScroll2Widget->_ay;
        const float w_ay1 = w_ay0 + pScroll2Widget->height();

        if (height() < pScroll2Widget->height()) {
            //SkDebugf("scroll 2 success:0\n");
            pScroll2Widget = NULL;
            return;
        }

        if (w_ay0 < ay0) {
            float dy = (ay0 - w_ay0) / 20 + 1;
            moveXY(0, dy);
            //SkDebugf("scroll 2: %.03f success:1\n", dy);
            return;
        } 
        else if (ay1 < w_ay1) {
            float dy = -((w_ay1 - ay1) / 20 + 1);
            moveXY(0, dy);
            //SkDebugf("scroll 2: %.03f success:2\n", dy);
            return;
        }
        
        //SkDebugf("scroll 2 success:3\n");
        pScroll2Widget = NULL;
    }
}

bool DecScroller::moveXY( int dx, int dy ) {
    dirty( true );
    //dirtyChildren( true, true );
    bool moved = false;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];  
        w->align( ALIGN_ABSOLUTE );
        if( dx ) {
            if( w->width() < width() ) {
                continue;
            }   
            moved = true;         
            int _fxx = w->x() + dx;
            if( _fxx > 0 ) {
                _fxx = 0;
            }
            if( _fxx < (0-(w->width()-width())) ) {
                _fxx = (0-(w->width()-width()));
            }
            float _innX = (_fxx - w->x()) * 0.95;
            if( _innX > -1 && _innX < 1 ) {
                _innX = 0;
            } 
            w->x( _fxx );
            mouseCapture( false ); 
            _lastMoveX = _innX;
        } 
        if( dy ) {
            if( w->height() < height() ) {
                continue;
            }    
            moved = true;        
            int _fyy = w->y() + dy;
            if( _fyy > 0 ) {
                _fyy = 0;
            }
            if( _fyy < (0-(w->height()-height())) ) {
                _fyy = (0-(w->height()-height()));
            }
            float _innY = (_fyy - w->y()) * 0.95;
            if( _innY > -1 && _innY < 1 ) {
                _innY = 0;
            }             
            w->y( _fyy );
            mouseCapture( false );  
            _lastMoveY = _innY;
        }

    }
    return moved;
}

void DecScroller::reset() {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];  
        w->x( _padding[3] );
        w->y( _padding[0] );
    }
}

bool DecScroller::onMouseWheel(int ix, int iy, float deltax, float delta, skui::ModifierKey modifiers) {
    for( int i=_children.size()-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->hitTest( ix, iy ) ) {
            if( w->onMouseWheel( ix - w->x(), iy - w->y(), deltax, delta, modifiers ) ) {
                return true;
            }
        }
    }    
    int dx = 0;
    int dy = 0;
    if( _scrollType & SCROLL_HORIZONTAL ) {
        dx = deltax;
    }
    if( _scrollType & SCROLL_VERTICAL ) {
        dy = delta;
    }
    if( dx || dy ) {
        moveXY( dx, dy );
        return true;
    }    
    return false;
}

bool DecScroller::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    /*
    // Jimmy's note - I fixed the scroller (moveXY) instead. 
    // It's kill capture even it did not moved. So, button failed to detect click.
    if (state == skui::InputState::kMove && 
        _children.size() && _children[0] && 
        _children[0]->width() <= width() &&
        _children[0]->height() <= height() )
        return true;
    */    
    if( state == skui::InputState::kDown ) {
        if( _scrollType & SCROLL_HORIZONTAL ) {
            mouseMoveWatchHorizontal( true );
            _isPressed = true;
        }
        if( _scrollType & SCROLL_VERTICAL ) {
            mouseMoveWatchVertical( true );
            _isPressed = true;
        }
        _firstMove = true;
        _lastMouseX = ix;
        _lastMouseY = iy;        
    }
    else if( state == skui::InputState::kMove ) {
        if( _isPressed ) {
            int dx = 0;
            int dy = 0;
            if( _scrollType & SCROLL_HORIZONTAL ) {
                dx = ix - _lastMouseX;
            }
            if( _scrollType & SCROLL_VERTICAL ) {
                dy = iy - _lastMouseY;
            }
            if( _firstMove ) {
                if( dx < -3 || dx > 3 ) {
                     _lastMouseX = ix;
                     _firstMove = false;
                }              
                if( dy < -3 || dy > 3 ) {
                    _lastMouseY = iy;
                    _firstMove = false;
                }
                if( _firstMove == false ) {
                    if( moveXY( dx, dy ) ) {
                        mouseCapture( false );
                    }                
                }
            }
            else {
                if( dx || dy ) {
                    if( dx ) {
                        _lastMouseX = ix;
                    }
                    if( dy ) {
                        _lastMouseY = iy;
                    }
                    if( moveXY( dx, dy ) ) {
                        mouseCapture( false );
                    }
                    return true;
                }
            }
        }
    }
    else if( state == skui::InputState::kUp ) {
        if( _isPressed ) {
            _isPressed = false;
            if( _scrollType & SCROLL_HORIZONTAL ) {
                mouseMoveWatchHorizontal( false );       
            }
            if( _scrollType & SCROLL_VERTICAL ) {
                mouseMoveWatchVertical( false );
            }  
        }
    }
    
    for( int i=_children.size()-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->hitTest( ix, iy ) ) {
            
            if( w->onMouse( ix - w->x(), iy - w->y(), state, modifiers ) ) {
                return true;
            }
            return false;
        }
    }     
    

    return false;
}
