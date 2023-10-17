#include "DecSpacer.h"

DecSpacer::DecSpacer() {
    background( false );
    border( false );
    _testDraw = false;
    //cache( true );
}

DecSpacer::~DecSpacer() {

};

DecSpacer &Spacer() {
    DecSpacer *w = new DecSpacer();
    return *w;    
}