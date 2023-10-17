#ifndef __MAGICPATTERN_APP_LAYER_H__
#define __MAGICPATTERN_APP_LAYER_H__

#include "DecUILayer.h"
#include "DecMagicPattern.h"

namespace sk_ui {


class MagicPatternLayer : public DecUILayer {
public:
    MagicPatternLayer( std::string option );

    DecMagicPattern *pMagicPattern;
};

}

#endif //__MAGICPATTERN_APP_LAYER_H__
