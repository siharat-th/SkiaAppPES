//
//  PES5Toolbox.cpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#include "PES5Toolbox.hpp"
#include "tools/Resources.h"

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include "PES5Command.hpp"
#include "PES5Template.hpp"

namespace sk_ui {

DecScroller & CreatePesToolbox(){
    DecScroller* scroller = new DecScroller(SCROLL_VERTICAL);
    scroller->hideOnFlag(PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_TOOL_BOX);
    scroller->size(0, -1);
    scroller->border(true);

    DecColumnContainer *container = new DecColumnContainer();
    container->size( 0, 0);
    container->border( false );
    container->background( false );
    container->gap( 5 );
    container->padding(5, 5, 5, 5);
    
    container->_(Button("", 0xf031, [=](DecButton &btn){
        PES5_AddPPEFText();
    }).size( 35, 35)); // A : PPEF Font
    container->_(Button("", 0xf893, [=](DecButton &btn){
        PES5_AddTTFText();
    }).size( 35, 35)); // T : TTF Font
    container->_(Button("", 0xf1dd, [=](DecButton &btn){ //0xf25c
        PES5_AddPPEFText_V2();
    }).size( 35, 35)); // P : Monogram Font(PPEF2)
    container->_(Button("", 0xf61f, [=](DecButton &btn){
        PES5_OpenShapeDialog();
    }).size( 35, 35)); // Basic Shape(SVG)
    container->_(Button("", 0xf279, [=](DecButton &btn){
        SkDebugf("Satin Column byButton\n");
        if(PES5_IsSatinColumnInputStarted()==false){
            PES5_StartSatinColumnInput();
        }
        else{
            PES5_StopSatinColumnInput();
        }
    }).size( 35, 35)); // Satin Column
    container->_(Button("", 0xf040, [=](DecButton &btn){
        if(PES5_IsDrawingInputStarted()==false){
            PES5_StartDrawingInput();
        }
        else{
            PES5_StopDrawingInput();
        }
    }).size( 35, 35)); // Drawing
    container->_(Button("", 0xf068, [=](DecButton &btn){
        if(PES5_IsLinesToolInputStarted()==false){
            PES5_StartLinesToolInput();
        }
        else{
            PES5_StopLinesToolInput();
        }
    }).size( 35, 35)); // Line
    container->_(Button("", 0xf02e, [=](DecButton &btn){
        if(PES5_IsPolygonToolInputStarted()==false){
            PES5_StartPolygonToolInput();
        }
        else{
            PES5_StopPolygonToolInput();
        }
    }).size( 35, 35)); // Polygon
    container->_(Button("", 0xf25a, [=](DecButton &btn){
        if(PES5_IsPathEditInputStarted()==false){
            PES5_StartPathEditInput();
        }
        else{
            PES5_StopPathEditInput();
        }
    }).size( 35, 35)); // Path Edit
    container->_(Button("", 0xf25a, [=](DecButton &btn){
        if(PES5_IsStitchEditInputStarted()==false){
            PES5_StartStitchEditInput();
        }
        else{
            PES5_StopStitchEditInput();
        }
    }).size( 35, 35)); // Stitch Edit
    container->_(Button("", 0xf02e, [=](DecButton &btn){
//        PES5_AddObjectOutlineShape();
        PES5_OpenOutlineShapeDialog();
    }).size( 35, 35)); // Outline Shape
    container->_(Button("Sim", 0, [=](DecButton& btn) {
        if(PES5_IsStitchSimulatorMode()==false){
            PES5_StartStitchSimulatorMode();
        }
        else{
            PES5_StopStitchSimulatorMode();
        }
    }).size( 35, 35)); // Stitch Simulator Mode
    
    //return *container;
    scroller->_(*container);
    return *scroller;
}

}
