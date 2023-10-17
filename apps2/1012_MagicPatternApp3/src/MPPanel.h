#ifndef __MP_PANEL_H__
#define __MP_PANEL_H__

#include "DeclarativeUI.h"

class DecMagicPattern;
class DecMPButton;

class DecMPPanel : public DecStackContainer {
public:
    DecMPPanel( DecMagicPattern *pMP, int h );
    ~DecMPPanel();

    int nMode;
    void setMode( int n );

    void ShowNeck7( bool show );
    void ShowCollar4( bool show );

    int currentPocketLevel;
    int currentPocketSide;

    void setAcitvePocket( int level, int side, bool active );

    DecRowContainer *pPanelType;
    DecScroller *pPanelLength;
    DecScroller *pPanelNeck;
    DecScroller *pPanelCollar;
    DecScroller *pPanelCollar2;
    DecScroller *pPanelSleeve;
    DecScroller *pPanelHem;
    DecRowContainer *pPanelPocket;
    DecImage *pPocketInactive[4];
    DecImage *pPocketActive[4];

    DecStackContainer *pPocketContainer;
    DecStackContainer *pPocketCatContainer;
    DecColumnContainer *pPocketTypeContainer;
    DecStackContainer *pPocketInfo;

    DecMPButton *pButtonNeck7;
    DecMPButton *pButtonCollar4;

    DecMagicPattern *pMagicPattern;
};

DecMPPanel &MPPanel( DecMagicPattern *pMP, int h );

#endif //__MP_PANEL_H__