#include "DecPageContainer.h"
#include "DeclarativeUI.h"

DecPageContainer::DecPageContainer() 
{
    //SkDebugf( "DecPageContainer\n" );  
    _isFocusable = true;  
    _activate_index = -1;
}

DecPageContainer::~DecPageContainer() {

}

DecPageContainer &Pages() {
    DecPageContainer *w = new DecPageContainer();
    return *w;    
}

DecWidget &DecPageContainer::child( DecWidget &c, bool front ) {
    c.setParent( this );
    if( front ) {
        _children.insert(_children.begin(), 1, &c);
    }
    else {
        _children.push_back( &c );
    }    
    if( _children.size() == 1 ) {
        _activate_child( 0 );
    }
    else {
        if( !c._isSubWidget ) {
            c.visible( false );
        }
    }
    return *this;
}

/*
void DecPageContainer::_draw_children(SkCanvas &canvas) {
    if( _activate_index >= 0 && _activate_index < _children.size() ) {
        DecWidget *w = _children[_activate_index];
        w->pre_draw( canvas );
        w->draw( canvas );
        w->post_draw( canvas );
    }
}
*/

void DecPageContainer::_activate_child( int a ) {
    _activate_index = a;

    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( i == a ) {
            w->visible( true );
        }
        else {
            if( w->_isSubWidget == false ) {
                w->visible( false );
            }
        }
    }  
    dirty( true, true );
}

void DecPageContainer::next() {
    for( int i=_activate_index+1; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w->_isSubWidget == false ) {
             _activate_child( i );
             return; 
        }
    }
    for( int i=0; i<_activate_index; i++ ) {
        DecWidget *w = _children[i];
        if( w->_isSubWidget == false ) {
             _activate_child( i );
             return; 
        }        
    }
}

void DecPageContainer::previous() {
    for( int i=_activate_index-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->_isSubWidget == false ) {
             _activate_child( i );
             return; 
        }
    }
    for( int i=_children.size()-1; i>_activate_index; i-- ) {
        DecWidget *w = _children[i];
        if( w->_isSubWidget == false ) {
             _activate_child( i );
             return; 
        }        
    } 
}

bool DecPageContainer::onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) {
    if( _children.size() == 0 ) {
        return false;
    }
    if( k == skui::Key::kLeft && state == skui::InputState::kDown ) {
        //SkDebugf( "Left\n" );
        previous();
    }
    else if( k == skui::Key::kRight && state == skui::InputState::kDown ) {
        //SkDebugf( "Right\n" );
        next();     
    }
    else if( k == skui::Key::kUp && state == skui::InputState::kDown ) {
        if( !_isFocus ) {
            focus( true );
            return true;
        }
        if( _parent ) {
            _parent->onKey( k, state, modifiers );
        }
    }
    else if( k == skui::Key::kDown && state == skui::InputState::kDown ) {
        if( _activate_index >= 0 && _activate_index < _children.size() ) {
            DecWidget *w = _children[_activate_index];     
            if( w->type()== "Pages")  {
                w->focus( true );
                SkDebugf( "Down to pages %i\n", (int)w->_isVisible );
                return true;
            }
        }
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];     
            if( w->type()== "Pages")  {
                w->focus( true );
                SkDebugf( "Down to pages %i\n", (int)w->_isVisible );
                return true;
            }
            if( w->_isSubWidget ) {
                w->focus( true );
                return true;
            }
        }  
        if( _parent ) {
            _parent->onKey( k, state, modifiers );
        }
    }
    return false;
}