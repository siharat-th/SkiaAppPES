#ifndef __DEC_SLIDER_H__
#define __DEC_SLIDER_H__

#include "DecWidget.h"
#include "DecLabel.h"

class DecSliderKnob : public DecWidget {
public:
    DecSliderKnob();
    virtual ~DecSliderKnob() override;
};

class DecSlider : public DecContainer {
public:    
    DecSlider( int min, int max, int step, bool bVertical = false, void (*callback)(DecSlider &button)=NULL );
    virtual ~DecSlider() override;

    virtual std::string type() override { return "RadioButton"; }

    virtual bool updateUI( int level ) override;
protected:
    void (*_callback)(DecSlider &checkbox);
    //float _value;
    float _min;
    float _max;
    float _step;

    bool _isVertical;

    DecSliderKnob *_knob;
};

DecSlider &Slider( int min, int max, int step, bool bVertical = false, void (*callback)(DecSlider &button)=NULL );

#endif //__DEC_SLIDER_H__