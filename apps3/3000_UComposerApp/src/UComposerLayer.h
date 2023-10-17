#ifndef __UCOMPOSER_APP_LAYER_H__
#define __UCOMPOSER_APP_LAYER_H__

#include "DecUILayer.h"

namespace sk_ui {


class UComposerLayer : public DecUILayer {
public:
    UComposerLayer();
    virtual void onInit() override;
protected:
    DecRowContainer *_workspace;
};

}

#endif //__UCOMPOSER_APP_LAYER_H__