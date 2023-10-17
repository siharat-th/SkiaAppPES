//
//  PES5Layer.hpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#ifndef PES5Layer_hpp
#define PES5Layer_hpp

#include "DeclarativeUI.h"
#include "DecUILayer.h"
//#include "PES5Command.hpp"

namespace sk_ui {

class Pes5Layer : public DecUILayer {
public:
    Pes5Layer(std::string AppTitle);
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;

};

}

#endif /* PES5Layer_hpp */
