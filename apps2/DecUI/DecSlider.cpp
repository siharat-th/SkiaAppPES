#include "DecSlider.h"
#include "DecUI.h"

DecSliderKnob::DecSliderKnob() {
    _testDraw = false;
}

DecSliderKnob::~DecSliderKnob() {

}

DecSlider::DecSlider( int min, int max, int step, bool bVertical, void (*callback)(DecSlider &button) ) {
    _min = min;
    _max = max;
    _step = step;

    _isVertical = bVertical;

    if( !_isVertical ) {
        size( -1, 20 );
    }
    round( 10 );
    bg_color( 0xFFF0F0F0 );

    _knob = new DecSliderKnob();
    _knob->size( 20, 20 );
    _knob->round( 10 );
    _knob->border( true );
    _knob->borderWidth( 2 );
    _knob->draggableX( true );
    _knob->align( ALIGN_ABSOLUTE );
    this->child( *_knob );
}

DecSlider::~DecSlider() {

}

bool DecSlider::updateUI( int level ) {
    bool ret = DecWidget::updateUI( level );

    _knob->_isLimitPosn = true;
    _knob->_minX = 0;
    _knob->_maxX = _width - _knob->width();

    return ret;
}

DecSlider &Slider( int min, int max, int step, bool bVertical, void (*callback)(DecSlider &button) ) {
    DecSlider *w = new DecSlider( min, max, step, bVertical, callback );
    return *w;  
}