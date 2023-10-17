#ifndef __DECBWS_LOADER_H__
#define __DECBWS_LOADER_H__

#include "DeclarativeUI.h"
#include "DecBWSUI.h"
#include "DecBWSJoyStick.h"
#include "DecBWSR2.h"

class DecBWS : public DecContainer {
public:    
    DecBWS();
    virtual ~DecBWS() override;
    virtual void init() override;
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;
    virtual void update() override;

    DecBWSJoyStick *_pJoyStick;
    DecBWSR2 *_pR2;
    DecBWSUI *_pUI;

    float _jX;
    float _jY;

    float _jTargetX;
    float _jTargetY;

    float _jLeft;
    float _jRight;
    float _jUp;
    float _jDown;
};

DecBWS &BWS();

#endif //__DECBWS_LOADER_H__