#ifndef __SVGEDITOR_APP_LAYER_H__
#define __SVGEDITOR_APP_LAYER_H__

#include "DecUILayer.h"

class DecSVGEditor;

namespace sk_ui {

class SVGEditorLayer : public DecUILayer {
public:
    SVGEditorLayer();

protected:
    DecSVGEditor *pSVGEditor;
};

}

#endif //__SVGEDITOR_APP_LAYER_H__