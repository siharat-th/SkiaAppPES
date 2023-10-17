//
//  sk_ui.cpp
//  sk_ui
//
//  Created by Jimmy on 13/5/2564 BE.
//

#include <iostream>
#include "sk_ui.hpp"
#include "sk_uiPriv.hpp"

void sk_ui::HelloWorld(const char * s)
{
    sk_uiPriv *theObj = new sk_uiPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void sk_uiPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

