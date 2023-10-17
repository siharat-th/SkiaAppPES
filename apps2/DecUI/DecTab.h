#ifndef __DEC_TAB_H__
#define __DEC_TAB_H__

#include "DeclarativeUI.h"

class DecTabBar : public DecRowContainer {
public:    
    DecTabBar(std::function<void(DecTabBar &tabBar, int activeTab)> callback=nullptr);
    virtual ~DecTabBar() override;
    virtual DecWidget &_( DecWidget &c, bool front=false ) override;

    virtual std::string type() override { return "Tab"; }
    std::vector<DecTabButton *>tabButtons;

    void setActiveTab( int n );
    int nActiveTab;

    std::function<void(DecTabBar &tabBar, int activeTab)> _callback;
};

DecTabBar &TabBar(std::function<void(DecTabBar &tabBar, int activeTab)> callback=nullptr);

class DecTabContent : public DecStackContainer {
public:    
    DecTabContent();
    virtual ~DecTabContent() override;

    virtual std::string type() override { return "DecTabContent"; }
    
protected:
    virtual void _draw_border(SkCanvas& canvas) override;    
};

DecTabContent &TabContent();

#endif // __DEC_TAB_H__