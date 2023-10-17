#ifndef __DEC_TESTWIDGET_H__
#define __DEC_TESTWIDGET_H__

#include "DecWidget.h"

class DecTestWidget : public DecWidget {
public:    
    DecTestWidget();
    virtual ~DecTestWidget() override;

    virtual std::string type() override { return "TestWidget"; }

    virtual void update() override;
    virtual bool updateUI( int level ) override;
    virtual void onResized(int w, int h) override;
    virtual void fireOnResized() override;
    virtual void dirty(bool b = true, bool updateLayout=false ) override;
protected:
};

DecTestWidget &TestWidget();

#endif //__DEC_TESTWIDGET_H__