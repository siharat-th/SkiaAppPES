#ifndef __DECUTESTPANEL_LOADER_H__
#define __DECUTESTPANEL_LOADER_H__

#include "DeclarativeUI.h"

class DecUTestPanel : public DecColumnContainer {
public:    
    DecUTestPanel();
    virtual ~DecUTestPanel() override;

    void AddTestWidget();
    void AddTestSVG();

    void NewDocument();

    void LoadSVG();
    void OpenSVGFile();

    void SaveSVGFile();
    void ExportSVGFile();

    void ExportPNGFile();
};

DecUTestPanel &UTestPanel();

#endif //__DECUTESTPANEL_LOADER_H__