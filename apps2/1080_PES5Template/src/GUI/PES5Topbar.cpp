//
//  PES5Topbar.cpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#include "PES5Topbar.hpp"
#include "tools/Resources.h"

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include "PES5Command.hpp"
#include "Utils/PesStitchSimulator.hpp"
#include "PES5Template.hpp"

namespace sk_ui {

DecScroller & CreatePesTopbar() {
    DecScroller* scroller = new DecScroller(SCROLL_HORIZONTAL);
    scroller->hideOnFlag(PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_TOOL_BAR);
    scroller->size(-1, 0);
    scroller->border(true);

    DecRowContainer *container = new DecRowContainer();
    container->size(0, 0);
    container->border( false );
    container->background( false );
    container->gap( 5 );
    container->padding(5, 5, 5, 5);
    
    container->_(Button("", 0xf15b, [=](DecButton &btn){ // New
        PES5_NewDocument();
#ifdef __EMSCRIPTEN__
        EM_ASM({
            window['pes_data'] && window['pes_data']['file'] && window['pes_data']['file']['updateHash'] && (async() => await window['pes_data']['file']['updateHash']())();
        });
#endif
    }).size( 35, 35));
    container->_(Button("", 0xf07c, [=](DecButton &btn){ // Open
        PES5_OpenPPESFile();
    }).size( 35, 35));
    container->_(Button("", 0xf0c7, [=](DecButton &btn){ // Save
        PES5_SavePPESFile();
    }).size( 35, 35));
    
    container->_(Spacer().size( 10, 0 ));
    
    container->_(Button("", 0xf1c5, [=](DecButton &btn){ // Tracing
        PES5_TracingPNGFile();
    }).size( 35, 35));
    
    container->_(Button("PES", 0xf090, [=](DecButton &btn){ // Import
        PES5_ImportPESFile();
    }).size( 60, 35));
    container->_(Button("SVG", 0xf090, [=](DecButton &btn){ // Import
        PES5_ImportSVGFile();
    }).size( 60, 35));

    container->_(Spacer().size(10, 0));

    container->_(Button("PES", 0xf08b, [=](DecButton& btn) {  // Export PES
        PES5_ExportPESFile();
    }).size( 60, 35));
    container->_(Button("DST", 0xf08b, [=](DecButton &btn){ // Export DST
        PES5_ExportDSTFile();
    }).size( 60, 35));
    container->_(Button("EXP", 0xf08b, [=](DecButton &btn){ // Export EXP
        PES5_ExportEXPFile();
    }).size( 60, 35));
    container->_(Button("JEF", 0xf08b, [=](DecButton &btn){ // Export JEF
        PES5_ExportJEFFile();
    }).size( 60, 35));
    container->_(Button("PNG", 0xf08b, [=](DecButton &btn){ // Export PNG
        PES5_ExportPNGFile();
    }).size( 60, 35));
    container->_(Button("SVG", 0xf08b, [=](DecButton &btn){ // Export SVG
        PES5_ExportSVGFile();
    }).size( 60, 35));
    
    
    container->_(Spacer().size( 10, 0 ));
    
    container->_(Button("", 0xf0e2, [=](DecButton &btn){ // Undo
        PES5_Undo();
    }).size( 35, 35));
    container->_(Button("", 0xf01e, [=](DecButton &btn){ // Redo
        PES5_Redo();
    }).size( 35, 35));
    
    container->_(Spacer().size( 10, 0 ));
    
    
    container->_(Button("", 0xf00e, [=](DecButton &btn){ // Zoom in
        PES5_ZoomIn();
    }).size( 35, 35));
    container->_(Button("", 0xf010, [=](DecButton &btn){ // Zoom out
        PES5_ZoomOut();
    }).size( 35, 35));
    
    container->_(Spacer().size( 10, 0 ));
    
    // backward f04a
    // play     f04b
    // pause    f04c
    // stop     f04d
    // forward  f04e
    auto sim = PesStitchSimulator::getInstance();
    container->_(Button("", 0xf04a, [=](DecButton &btn){
        sim->slowdown();
    }).size( 35, 35));
    container->_(Button("", 0xf04b, [=](DecButton &btn){
        if(sim->isPause())
            sim->resume();
        else{
            sim->stop();
            PES5_PrepareStitchSimulatorData();
            sim->start();
        }
    }).size( 35, 35));
    container->_(Button("", 0xf04c, [=](DecButton &btn){
        sim->pause();
    }).size( 35, 35));
    container->_(Button("", 0xf04d, [=](DecButton &btn){
        sim->stop();
    }).size( 35, 35));
    container->_(Button("", 0xf04e, [=](DecButton &btn){
        sim->speedup();
    }).size( 35, 35));
    
    container->_(Spacer().size( 10, 0 ));

    container->_(Button("", 0xf013, [=](DecButton &btn){
         PES5_OpenSettingDialog();
    }).size( 35, 35));
    
    //return *container;
    scroller->_(*container);
    return *scroller;
}

}
