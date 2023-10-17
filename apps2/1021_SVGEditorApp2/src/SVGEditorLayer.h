#ifndef __SVGEDITOR_APP_LAYER_H__
#define __SVGEDITOR_APP_LAYER_H__

#include "DecUILayer.h"

class DecSVGEditor;

namespace sk_ui {

class SVGEditorLayer : public DecUILayer {
public:
    SVGEditorLayer();

//virtual bool onChar(SkUnichar c, skui::ModifierKey mod) override;

protected:
    DecSVGEditor *pSVGEditor;
};

}

#endif //__SVGEDITOR_APP_LAYER_H__