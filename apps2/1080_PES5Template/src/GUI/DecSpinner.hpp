//
//  DecSpinner.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 6/10/2565 BE.
//

#ifndef DecSpinner_hpp
#define DecSpinner_hpp

#include "DecWidget.h"
#include "DecRowContainer.h"
#include "DecButton.h"
#include "DecText.h"

namespace sk_ui{

class DecSpinner : public DecRowContainer{
public:
    DecSpinner(float value, float min, float max, float step);
    virtual ~DecSpinner() override;
    virtual std::string type() override { return "Spinner"; }
    virtual DecWidget & value(float v) override;
    virtual float value() override;
    
protected:
//    float _value;
    float _min;
    float _max;
    float _step;
    DecText* pText;
};

DecSpinner &Spinner(float value, float min, float max, float step);

}

#endif /* DecSpinner_hpp */
