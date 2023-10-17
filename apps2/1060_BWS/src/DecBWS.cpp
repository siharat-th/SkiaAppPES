#include "DeclarativeUI.h"
#include "DecBWS.h"
#include "DecBWSUI.h"
#include "DecBWSR2.h"
#include "DecBWSJoyStick.h"

DecBWS::DecBWS() {
    _isFocusable = true;
    focus( true );

    _jX = 0;
    _jY = 0;

    _jTargetX = 0;
    _jTargetY = 0;

    _jLeft = 0;
    _jRight = 0;
    _jUp = 0;
    _jDown = 0;

    size( -1, -1 );
    border( false );
    background( true );
    _( Column()
        .size( -1, -1 )
        ._( Row().hide()
            .size( -1, 44 )
            .border( false )
            .bg_color( 0xFFE0E0E0 )
            ._( Spacer().size( -1, 0 ) )
            ._( Text( "BWS" ).align( ALIGN_CENTER ) )
            ._( Spacer().size( -1, 0 ) )
        )
        ._( Row()
            .size( -1, -1 )
            .border( false )
            ._( Spacer().size( -1, 0 ) )
            ._( Column()
                .size( 0, -1 )
                .border( false )
                ._( Spacer().size( 0, -1 ) )
                ._( Row()
                    .size( 0, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( BWSJoyStick()//.hide()
                        .ref( (DecWidget **)&_pJoyStick )
                    )
                    ._( Column()
                        .size( 0, 0 )
                        .gap( 10 )
                        .border( false )
                        ._( Row() //.hide()
                            .size( -1, 0 )
                            .border( false )
                            ._( Spacer().size( -1, 0 ) )
                            ._( BWSR2()
                                .ref( (DecWidget **)&_pR2 )
                            )
                        )
                        ._( BWSUI()
                            .ref( (DecWidget **)&_pUI )
                            .align( ALIGN_CENTER )
                        )
                    )
                )
                ._( Spacer().size( 0, -1 ) )
            )
            ._( Spacer().size( -1, 0 ) )
        )
    );
}

DecBWS::~DecBWS(){

}

void DecBWS::init() {
    SkDebugf( "DecBWS::init\n" );
    DecWidget::init();
}

DecBWS &BWS() {
    DecBWS *w = new DecBWS();
    return *w;
}

void DecBWS::update() {
    DecWidget::update();
    if (_pJoyStick->bGamepadConnected) {
        _pUI->bA = _pJoyStick->bA;
        _pUI->bB = _pJoyStick->bB;

        if (_pUI->bR2 != _pJoyStick->bR2) {
            _pUI->bR2 = _pJoyStick->bR2;
            _pR2->bR2 = _pJoyStick->bR2;
            _pR2->dirty(true, false);
        }

        _pUI->bDUp = _pJoyStick->bDUp;
        _pUI->bDDown = _pJoyStick->bDDown;
        _pUI->bDLeft = _pJoyStick->bDLeft;
        _pUI->bDRight = _pJoyStick->bDRight;

        _pUI->jX = _pJoyStick->jX;
        _pUI->jY = _pJoyStick->jY;
    } else {
        _jTargetX = _jLeft + _jRight;
        _jTargetY = _jUp + _jDown;

        if( _jTargetX < _jX ) {
            _jX -= 0.1;
            if( _jTargetX > _jX )
                _jX = _jTargetX;
        }
        if( _jTargetX > _jX ) {
            _jX += 0.1;
            if( _jTargetX < _jX )
                _jX = _jTargetX;
        }

        if( _jTargetY < _jY ) {
            _jY -= 0.1;
            if( _jTargetY > _jY )
                _jY = _jTargetY;
        }
        if( _jTargetY > _jY ) {
            _jY += 0.1;
            if( _jTargetY < _jY )
                _jY = _jTargetY;
        }

        float l = sqrt((_jX * _jX) + (_jY * _jY));
        if( l > 1 ) {
            _pJoyStick->jX = _jX / l;
            _pJoyStick->jY = _jY / l;
            _pUI->jX = _jX / l;
            _pUI->jY = _jY / l;
        }
        else {
            _pJoyStick->jX = _jX;
            _pJoyStick->jY = _jY;
            _pUI->jX = _jX;
            _pUI->jY = _jY;
        }
    }
}

bool DecBWS::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) {
    if( state == skui::InputState::kDown ) {
        SkDebugf( "%i\n", (int)k );
        if( k == skui::Key::kR ) {
            _pJoyStick->bDUp = true;
            _pUI->bDUp = true;
        }
        if( k == skui::Key::kF ) {
            _pJoyStick->bDDown = true;
            _pUI->bDDown = true;
        }
        if( k == skui::Key::kC ) {
            _pJoyStick->bDLeft = true;
            _pUI->bDLeft = true;
        }
        if( k == skui::Key::kV ) {
            _pJoyStick->bDRight = true;
            _pUI->bDRight = true;
        }

        if( k == skui::Key::kUp ) {
            _pJoyStick->bDUp = true;
            _pUI->bDUp = true;
        }
        if( k == skui::Key::kDown ) {
            _pJoyStick->bDDown = true;
            _pUI->bDDown = true;
        }
        if( k == skui::Key::kLeft ) {
            _pJoyStick->bDLeft = true;
            _pUI->bDLeft = true;
        }
        if( k == skui::Key::kRight ) {
            _pJoyStick->bDRight = true;
            _pUI->bDRight = true;
        }

        if( k == skui::Key::kZ ) {
            _pJoyStick->bA = true;
            _pUI->bA = true;
        }
        if( k == skui::Key::kX ) {
            _pJoyStick->bB = true;
            _pUI->bB = true;
        }

        if( k == skui::Key::kShift ) {
            SkDebugf( "Shift down\n" );
            _pR2->bR2 = true;
            _pR2->dirty( true, false );
            _pUI->bR2 = true;
        }

        if( k == skui::Key::kA )
            _jLeft = -1.0;
        if( k == skui::Key::kD )
            _jRight = 1.0;
        if( k == skui::Key::kW )
            _jUp = -1.0;
        if( k == skui::Key::kS )
            _jDown = 1.0;

    }
    else if( state == skui::InputState::kUp ) {
        if( k == skui::Key::kR ) {
            _pJoyStick->bDUp = false;
            _pUI->bDUp = false;
        }
        if( k == skui::Key::kF ) {
            _pJoyStick->bDDown = false;
            _pUI->bDDown = false;
        }
        if( k == skui::Key::kC ) {
            _pJoyStick->bDLeft = false;
            _pUI->bDLeft = false;
        }
        if( k == skui::Key::kV ) {
            _pJoyStick->bDRight = false;
            _pUI->bDRight = false;
        }

        if( k == skui::Key::kUp ) {
            _pJoyStick->bDUp = false;
            _pUI->bDUp = false;
        }
        if( k == skui::Key::kDown ) {
            _pJoyStick->bDDown = false;
            _pUI->bDDown = false;
        }
        if( k == skui::Key::kLeft ) {
            _pJoyStick->bDLeft = false;
            _pUI->bDLeft = false;
        }
        if( k == skui::Key::kRight ) {
            _pJoyStick->bDRight = false;
            _pUI->bDRight = false;
        }

        if( k == skui::Key::kZ ) {
            _pJoyStick->bA = false;
            _pUI->bA = false;
        }
        if( k == skui::Key::kX ) {
            _pJoyStick->bB = false;
            _pUI->bB = false;
        }

        if( k == skui::Key::kShift ) {
            SkDebugf( "Shift up\n" );

            _pR2->bR2 = false;
            _pR2->dirty( true, false );
            _pUI->bR2 = false;
        }

        if( k == skui::Key::kA )
            _jLeft = 0.0;
        if( k == skui::Key::kD )
            _jRight = 0.0;
        if( k == skui::Key::kW )
            _jUp = 0.0;
        if( k == skui::Key::kS )
            _jDown = 0.0;

    }
    return false;
}
