#include "DeclarativeUI.h"
#include "DecComboBox.h"
#include <include/core/SkTime.h>

class DecComboBoxScroller : public DecScroller {
public:
    DecComboBoxScroller(int scroll_type) : DecScroller(scroll_type) {
        _isFocusable = true;
        _border = true;
        _borderWidth = 1;
        _border_color = 0xFF808080;
        _focus_borderWidth = 2;
        _focus_border_color = SK_ColorGREEN;
    }
    virtual ~DecComboBoxScroller() override{}

    virtual std::string type() override { return "ComboBoxScroller"; }

    double fNextTime = -DBL_MAX;

    virtual void focus(bool f) override { 
        if (!f) {
            if (_isVisible) {
                const double now = SkTime::GetNSecs();
                constexpr double dHalfSec = 0.5 * 1e9;
                fNextTime = now + dHalfSec;

                DecWidget::focus(f);
                _isVisible = false;
            }
            return;
        }

        if (canAction()) {
            //const double now = SkTime::GetNSecs();
            //constexpr double dHalfSec = 0.5 * 1e9;
            //fNextTime = now + dHalfSec;

            DecWidget::focus(f);
            _isVisible = true;
        }
    }

    bool canAction() {
        if (fNextTime == -DBL_MAX) 
            return true;
        const double now = SkTime::GetNSecs();
        if (now >= fNextTime) {
            fNextTime = -DBL_MAX;
            return true;
        }
        return false;
    }

    bool onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers) override {
        static skui::InputState savestate = state;
        skui::InputState laststate = savestate;
        savestate = state;

        if (state == skui::InputState::kDown) {
            if (_scrollType & SCROLL_HORIZONTAL) {
                mouseMoveWatchHorizontal(true);
                _isPressed = true;
            }
            if (_scrollType & SCROLL_VERTICAL) {
                mouseMoveWatchVertical(true);
                _isPressed = true;
            }
            _firstMove = true;
            _lastMouseX = ix;
            _lastMouseY = iy;
        } else if (state == skui::InputState::kMove) {
            if (_isPressed) {
                int dx = 0;
                int dy = 0;
                if (_scrollType & SCROLL_HORIZONTAL) {
                    dx = ix - _lastMouseX;
                }
                if (_scrollType & SCROLL_VERTICAL) {
                    dy = iy - _lastMouseY;
                }
                if (_firstMove) {
                    if (dx < -3 || dx > 3) {
                        _lastMouseX = ix;
                        _firstMove = false;
                    }
                    if (dy < -3 || dy > 3) {
                        _lastMouseY = iy;
                        _firstMove = false;
                    }
                    if (_firstMove == false) {
                        if (moveXY(dx, dy)) {
                            mouseCapture(false);
                        }
                    }
                } else {
                    if (dx || dy) {
                        if (dx) {
                            _lastMouseX = ix;
                        }
                        if (dy) {
                            _lastMouseY = iy;
                        }
                        if (moveXY(dx, dy)) {
                            mouseCapture(false);
                        }
                        return true;
                    }
                }
            }
        } else if (state == skui::InputState::kUp) {
            if (_isPressed) {
                _isPressed = false;
                if (_scrollType & SCROLL_HORIZONTAL) {
                    mouseMoveWatchHorizontal(false);
                }
                if (_scrollType & SCROLL_VERTICAL) {
                    mouseMoveWatchVertical(false);
                }
            }
        }

        if ((state == skui::InputState::kUp) && 
            (laststate == skui::InputState::kDown) &&
            _firstMove) {
            for (int i = _children.size() - 1; i >= 0; i--) {
                DecWidget* w = _children[i];
                if (w->hitTest(ix, iy)) {
                    //SkDebugf("state:%d, hit: return true\n", state);
                    w->onMouse(ix - w->x(), iy - w->y(), skui::InputState::kDown, modifiers);
                    w->onMouse(ix - w->x(), iy - w->y(), skui::InputState::kUp, modifiers);
                    return true;
                }
            }
        }

        //SkDebugf("state:%d, !hit: return false\n", state);
        return false;
    }

};

DecComboBoxScroller& ComboBoxScroller(int scroll_type = SCROLL_BOTH) {
    DecComboBoxScroller* w = new DecComboBoxScroller(scroll_type);
    return *w;
}

DecComboBox::DecComboBox() {
    _selectedComboBoxItem = nullptr;
    _isFocusable = true;
    _border = true;
    _borderWidth = 1;
    _border_color = 0xFF808080;
    _focus_borderWidth = 2;
    _focus_border_color = SK_ColorGREEN;
    size( -1, 32 );
    padding( 0, 10, 0, 10 );
    //cache( false );
    child( Text( "" )
        .ref( (DecWidget **)&_pText )
        .size( -1, 0 )
        .align( ALIGN_CENTER )
        .cache(false)
    );
    child( Icon( 0xf0d7 )
        .size( 0, 0 )
        .align( ALIGN_CENTER )
        .cache(false)
    );
    onMousePressed( [=](DecWidget &widget) -> bool {
       dirty(true, true);

        if (((DecComboBoxScroller*)_pScroller)->canAction()) {
            if (_pScroller->visible()) {
                _pScroller->hide();
            } else {
                _pScroller->width(width() - 10);
                _pScroller->x(_ax + width() - width() + 10);
                _pScroller->y(_ay + height() - 1);
                _pScroller->show();
                _pScroller->dirty( true, true);
                // should be appended only once.
                if (!_pScroller->_parent) _ui->rootWidget->_(*_pScroller);
            }
        }
        return false;
    });

    _pScroller = new DecComboBoxScroller(SCROLL_VERTICAL);
    _pScroller->align( ALIGN_ABSOLUTE );
    _pScroller->size( 200, 0 );
    _pScroller->max_height( 160 );
    _pScroller->border( true );
    _pScroller->borderWidth( 1 );
    _pScroller->hide();

    _pScroller->_( Column().ref( (DecWidget **)&_pColumn ).size( -1, 0).border(false) );


}

DecComboBox::~DecComboBox() {

}

void DecComboBox::focus(bool f) {
    DecWidget::focus(f);
    if (_pScroller->visible()) {
        _pScroller->focus(true);
    } 

    if (_isFocus) {
        _pScroller->hide();
    }

    dirty( true );
}

DecComboBox &ComboBox() {
    DecComboBox *w = new DecComboBox();
    return *w;   
}

DecWidget &DecComboBox::_( DecWidget &c, bool front ) {
    if( _pColumn ) {
        c._index = _pColumn->_children.size();
        c._logicalParent = this;
        c.dirty();
        _pColumn->_(c, front);
        _pColumn->dirty();
    }
    return *this;
}

DecWidget &DecComboBox::select_comboboxItem( int index, bool update ) {
    _value = index;
    if( index == -1 ) {
        if( _parent ) {
            _parent->bubbleAction( ACTION_COMBOBOX_SELECT_CHANGED, NULL );
        }        
        text( "" );
    }
    else {
        if( _pColumn && (_pColumn->_children.size() > 0)) {
            if( _selectedComboBoxItem ) {
                _selectedComboBoxItem->select( false );
            }
            _selectedComboBoxItem = (DecComboBoxItem *)_pColumn->_children[index];
            _selectedComboBoxItem->select( true );   
            _pText->text( _selectedComboBoxItem->_pText->text() );
            text( _selectedComboBoxItem->_pText->text() );
            if( _parent ) {
                _parent->bubbleAction( ACTION_COMBOBOX_SELECT_CHANGED, this );
            }     
        }
    }
    if( _onValueChanged && update) {
        _onValueChanged( *this );
    }
    return *this;
}

DecWidget & DecComboBox::selectByChildValue( int value, bool update ){
    int n = (int)_pColumn->_children.size();
    int index = -1;
    for(int i=0; i<n; i++){
        int v = (int)_pColumn->_children[i]->value();
        if(v==value){
            index = i;
            break;
        }
    }
    return select_comboboxItem(index, update);
}

DecWidget & DecComboBox::selectByChildValue( float value, bool update ){
    int n = (int)_pColumn->_children.size();
    int index = -1;
    for(int i=0; i<n; i++){
        float v = _pColumn->_children[i]->value();
        if(v==value){
            index = i;
            break;
        }
    }
    return select_comboboxItem(index, update);
}

int DecComboBox::getSelectedIndex(){
    return (int)value();
}

float DecComboBox::getSelectedValue(){
    int selectedIndex = getSelectedIndex();
    if(selectedIndex==-1)
        return -1;
    else return _pColumn->_children[selectedIndex]->value();
}

void DecComboBox::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_COMBOBOXITEM_SELECTED ) {
        if( _pScroller ) {
            _pScroller->hide();
        }
        if( sender ) {
            _value = sender->_index;
            if( sender->type() == "DecComboBoxItem" ) {
                //dirty(true);
                if( _selectedComboBoxItem ) {
                    _selectedComboBoxItem->select( false );
                }
                sender->select( true );
                _selectedComboBoxItem = (DecComboBoxItem *)sender;
                _pText->text( _selectedComboBoxItem->_pText->text() );
                text( _selectedComboBoxItem->_pText->text() );
                if( _parent ) {
                    _parent->bubbleAction( ACTION_COMBOBOX_SELECT_CHANGED, this );
                }
            }
        }
        else {
            _value = -1;
            if( _selectedComboBoxItem ) {
                _selectedComboBoxItem->select( false );
            }
            _selectedComboBoxItem = NULL;
            _pText->text( "" );
            if( _parent ) {
                _parent->bubbleAction( ACTION_COMBOBOX_SELECT_CHANGED, NULL );
            }
        }       
        if( _onValueChanged ) {
            _onValueChanged( *this );
        }
    }
    else {
        DecWidget::bubbleAction( actionID, sender );
    }
}
