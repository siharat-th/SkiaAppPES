//
//  PES5ModalDialog.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 5/10/2565 BE.
//

#ifndef PES5ModalDialog_hpp
#define PES5ModalDialog_hpp

#include <stdio.h>
#include "DeclarativeUI.h"
#include "DecSpinner.hpp"
#include "pesEngine.h"
#include "Utils/PesTracingUtils.hpp"

namespace sk_ui {

class Pes5TracingDialogPreview;
class Pes5ShapeDialogPreview;
class Pes5PatternPreview;
class Pes5EmbroideryPreview;

// Tracing Dialog
class Pes5TracingDialog : public DecModal {
public:
    Pes5TracingDialog();
    virtual ~Pes5TracingDialog() override;

    virtual std::string type() override { return "MessageBox"; }
    void onOK();
    void onApply();
    
    void setBitmap(const SkBitmap & bitmap);
    void resetParameter();
    
protected:
    DecComboBox *pTraceTypeComboBox;
    DecSpinner *pNumColorSpinner, *pBrigthnessSpinner;
    DecCheckBox *pSmoothCheckBox, *pAutoEmbCheckBox;
    Pes5TracingDialogPreview *pTracingPreview;
    SkBitmap _bitmap;
    std::vector<TracingEngineResult> _results;
};
Pes5TracingDialog &TracingDialog();

// Shape Dialog
class Pes5ShapeDialog : public DecModal {
public:
    Pes5ShapeDialog();
    virtual ~Pes5ShapeDialog() override;

    virtual std::string type() override { return "Pes5ShapeDialog"; }
    void onCancel();
    void onOK();
    
protected:
    Pes5ShapeDialogPreview *pShapePreview;
    DecList *pList;
};
Pes5ShapeDialog &ShapeDialog();

// PatternFill Dialog
class Pes5PatternFillDialog : public DecModal {
public:
    Pes5PatternFillDialog();
    virtual ~Pes5PatternFillDialog() override;

    virtual std::string type() override { return "Pes5PatternFillDialog"; }
    void onCancel();
    void onOK();
    
protected:
    Pes5PatternPreview *pPatternPreview;
    DecList *pList;
};
Pes5PatternFillDialog &PatternFillDialog();

// MotifFill Dialog
class Pes5MotifFillDialog : public DecModal {
public:
    Pes5MotifFillDialog();
    virtual ~Pes5MotifFillDialog() override;

    virtual std::string type() override { return "Pes5MotifFillDialog"; }
    void onCancel();
    void onOK();
    
protected:
    Pes5PatternPreview *pPatternPreview;
    DecList *pList;
};
Pes5MotifFillDialog &MotifFillDialog();

// MotifStroke Dialog
class Pes5MotifStrokeDialog : public DecModal {
public:
    Pes5MotifStrokeDialog();
    virtual ~Pes5MotifStrokeDialog() override;

    virtual std::string type() override { return "Pes5MotifStrokeDialog"; }
    void onCancel();
    void onOK();
    
protected:
    Pes5PatternPreview *pPatternPreview;
    DecList *pList;
};
Pes5MotifStrokeDialog &MotifStrokeDialog();

// Setting Dialog
class Pes5SettingDialog : public DecModal {
public:
    Pes5SettingDialog();
    virtual ~Pes5SettingDialog() override;

    virtual std::string type() override { return "Pes5SettingDialog"; }
    void onOK();
    void initUi();
    
protected:
    DecCheckBox *pShowJump;
    DecComboBox *pHoop;
    DecComboBox *pDrawingMode;
};
Pes5SettingDialog &SettingDialog();

// Outline Shape Dialog
class Pes5OutlineShapeDialog : public DecModal {
public:
    Pes5OutlineShapeDialog();
    virtual ~Pes5OutlineShapeDialog() override;

    virtual std::string type() override { return "Pes5OutlineShapeDialog"; }
    void onCancel();
    void onOK();
    
protected:
    DecSpinner *pInsetSpinner;
    DecCheckBox *pRemoveHoleCheckBox;
};
Pes5OutlineShapeDialog &OutlineShapeDialog();

}

#endif /* PES5ModalDialog_hpp */
