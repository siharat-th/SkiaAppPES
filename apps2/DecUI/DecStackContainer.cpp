#include "DecStackContainer.h"

DecStackContainer::DecStackContainer() 
{
    //SkDebugf( "DecStackContainer\n" );    
}

DecStackContainer::~DecStackContainer() {

}

DecStackContainer &Stack() {
    DecStackContainer *w = new DecStackContainer();
    return *w;    
}

bool DecStackContainer::onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) {
    if( _children.size() == 0 ) {
        return false;
    }
    else if( k == skui::Key::kUp && state == skui::InputState::kDown ) {
        SkDebugf( "Previous\n" );
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];   
            if( w->_isFocus ) {
                i--;
                if( i < 0 ) {
                    i = _children.size()-1;
                }
                w = _children[i]; 
                w->focus( true );
                SkDebugf( "Focus %i\n", i );
                break;
            }
        }     
    }
    else if( k == skui::Key::kDown && state == skui::InputState::kDown ) {
        SkDebugf( "Next\n" );
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];   
            if( w->_isFocus ) {
                i++;
                if( i >= _children.size() ) {
                    i = 0;
                }
                w = _children[i]; 
                w->focus( true );
                SkDebugf( "Focus %i\n", i );

                break;
            }
        }         
    }    
    return false;
}