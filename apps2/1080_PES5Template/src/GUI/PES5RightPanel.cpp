//
//  PES5RightPanel.cpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#include "PES5RightPanel.hpp"
#include "tools/Resources.h"

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include "pesEngine.h"
#include "PES5Command.hpp"
#include "PES5DocumentView.hpp"
#include "PES5Pattern.h"
#include "DecSpinner.hpp"

#include "../PES5Template.hpp"

extern long PES5_GLOBAL_FLAGS;

namespace sk_ui {

DecPagesPes5& PagesPes5() {
    DecPagesPes5* w = new DecPagesPes5();
    return *w;
}

DecPagesPes5* pModeWidget = NULL;
DecPagesPes5* pBodyWidget = NULL;

DecList *pLayerList = NULL;
DecList *pColorList = NULL;

const int PPEF_PAGE = 0;
const int TTF_PAGE  = 1;
const int MONOGRAM_PAGE = 2;
const int SATIN_COLUMN_PAGE = 3;
const int SVG_PAGE = 4;
const int BLANK_PAGE = 5;

DecPagesPes5* pPropertiesWidget = NULL;
DecTextEdit *pPPEF_TextEdit = NULL;
DecTextEdit *pTTF_TextEdit = NULL;
DecTextEdit *pMonogram_TextEdit = NULL;
DecComboBox *pPPEFFont = NULL;
DecComboBox *pTTFFont = NULL;
DecComboBox *pMonogramFont = NULL;
DecComboBox *pFillComboBox = NULL;
DecComboBox *pStrokeComboBox = NULL;
DecComboBox *pSVGFillComboBox = NULL;
DecComboBox *pSVGStrokeComboBox = NULL;

DecSpinner *pPPEF_Density = NULL;
DecSpinner *pPPEF_PullCompensate = NULL;
DecSpinner *pMonogram_Density = NULL;
DecSpinner *pMonogram_PullCompensate = NULL;
DecSpinner *pSatinColumn_Density = NULL;
DecSpinner *pSatinColumn_PullCompensate = NULL;

DecComboBox *pPPEF_Effect = NULL;
DecCheckBox *pPPEF_Border = NULL;
DecCheckBox *pPPEF_Italic = NULL;

DecSpinner *pEffect_Angle = NULL;
DecSpinner *pEffect_Radius = NULL;
DecSpinner *pEffect_BorderGapX = NULL;
DecSpinner *pEffect_BorderGapY = NULL;
DecText *pEffect_Angle_Text = NULL;
DecText *pEffect_Radius_Text = NULL;
DecText *pEffect_BorderGapX_Text = NULL;
DecText *pEffect_BorderGapY_Text = NULL;

DecCheckBox *pTTF_FillFading = NULL;
DecCheckBox *pSVG_FillFading = NULL;

DecColumnContainer& CreatePesRightPanel() {
    DecColumnContainer *container = new DecColumnContainer();
    if( PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_LAYER_BOX ) {
        container->visible( false );
    }

    const int cW = 160;

    //container->size(260, -1);
    container->size(cW, -1);
    container->border(true);
    container->background( true );
    container->bg_color(0xFFF0F0F0);
    container->gap( 5 );
    container->padding(5, 5, 5, 5);
    
    container->_(
        Row()
        .size( cW-10, 32 )
        .gap( 5 )
        .border( false )
        .background( false )
        ._( Button( "", 0xf060, 
            [=](DecButton &btn) {
                pModeWidget->previous();
                pBodyWidget->previous();
                UpdateGUIRightPanel();
            })
            .border_color(0xFF808080)
            .gap(0) 
        )
        ._( PagesPes5().size( -1, -1 )
            .border( false )
            .background( true )
            .bg_color( 0xFFE0E0E0 )
            .ref( (DecWidget **)&pModeWidget )
            ._( Text( "Layer" ).align( ALIGN_CENTER ) )
            ._( Text( "Property" ).align( ALIGN_CENTER ) )
            ._( Text( "Color" ).align( ALIGN_CENTER ) )
        )
        ._( Button( "", 0xf061, 
            [=](DecButton &btn) {
                pModeWidget->next();
                pBodyWidget->next();
                UpdateGUIRightPanel();
            })
            .border_color(0xFF808080)
            .gap(0) 
        )
    );
    
    container->_(
        PagesPes5()
        .size( cW-10, -1 )
        .border(false)
        .ref((DecWidget **)&pBodyWidget)

        // page Layer
        ._( Column()
            .border(false)
            .gap(5)
            ._( List().size(cW-10, -1).ref((DecWidget **)&pLayerList))
            ._( Row()
                .size(cW-10, 0)
                .background( true )
                .bg_color(0xFFE0E0E0)
                .border( false )
                .gap( 5 )
                //._(Button("", 0xf357, [=](DecButton &btn){
                //     PES5_LayerMoveUp();
                // }).size(-1, 0))
                //._(Button("", 0xf354, [=](DecButton &btn){
                //     PES5_LayerMoveDown();
                // }).size(-1, 0))
                ._( Button("", 0xf247, 
                        [=](DecButton &btn){
                            PES5_LayerMerge();
                        }
                    )
                    .size(-1, 0)
                )
                ._( Button("", 0xf248, 
                        [=](DecButton &btn){
                            PES5_LayerBreak();
                        }
                    )
                    .size(-1, 0)
                )
                ._( Button("", 0xf0c5, 
                        [=](DecButton &btn){
                            PES5_LayerDuplicate();
                        }
                    )
                    .size(-1, 0)
                )
                ._( Button("", 0xf2ed, 
                        [=](DecButton &btn){
                            PES5_LayerDelete();
                        }
                    )
                    .size(-1, 0)
                )
            )
            ._( Row()
                .size(cW-10, 0)
                .background( true )
                .bg_color(0xFFE0E0E0)
                .border( false )
                .gap( 5 )
                ._( Button("", 0xf357, [=](DecButton &btn){
                        PES5_LayerMoveUp();
                }).size(-1, 0))
                ._( Button("", 0xf354, [=](DecButton &btn){
                     PES5_LayerMoveDown();
                }).size(-1, 0))
                //._(Button("", 0xf247, [=](DecButton &btn){
                //     PES5_LayerMerge();
                // }).size(-1, 0))
                //._(Button("", 0xf248, [=](DecButton &btn){
                //     PES5_LayerBreak();
                // }).size(-1, 0))
                //._(Button("", 0xf0c5, [=](DecButton &btn){
                //     PES5_LayerDuplicate();
                // }).size(-1, 0))
                //._(Button("", 0xf2ed, [=](DecButton &btn){
                //     PES5_LayerDelete();
                // }).size(-1, 0))
            )
        )

        // page Properties
        //._( Scroller(SCROLL_VERTICAL)
        ._( Column()
            //.border(false)
            //.background( false )
            .size(cW-10, -1)
            .border(false)
            .gap(0)
            ._( PagesPes5().border(false).ref((DecWidget **)&pPropertiesWidget)
                .size( cW-10, -1 )
            
                // page 0 - ppef
                ._( Column().padding(5,5,5,5).gap( 10 )
                    //.size( -1, 0 )
                    .size(cW-10, -1)
                    ._( TextEdit("ภิญญ์จักรปัก").size(-1, 0).ref((DecWidget**)&pPPEF_TextEdit))
                    ._( Scroller(SCROLL_VERTICAL)
                        .border(false)
                        .background( false )
                        .size(cW-20, -1)
                        ._( Column().gap( 10 )
                            .border(false)
                            .size( cW-20, 0 )

                            ._( ComboBox()
                                .ref( (DecWidget **)&pPPEFFont )
                                ._( ComboBoxItem( "Thai001" ) )
                                ._( ComboBoxItem( "Thai002" ) )
                                ._( ComboBoxItem( "Thai003" ) )
                                ._( ComboBoxItem( "Thai004" ) )
                                ._( ComboBoxItem( "Thai005" ) )
                                ._( ComboBoxItem( "Thai006" ) )
                                ._( ComboBoxItem( "Thai007" ) )
                                ._( ComboBoxItem( "Thai008" ) )
                                ._( ComboBoxItem( "Thai009" ) )
                                ._( ComboBoxItem( "Thai010" ) )
                                ._( ComboBoxItem( "Thai011" ) )
                                ._( ComboBoxItem( "Thai012" ) )
                                ._( ComboBoxItem( "Thai013" ) )
                                ._( ComboBoxItem( "Thai014" ) )
                                ._( ComboBoxItem( "Thai015" ) )
                                ._( ComboBoxItem( "Thai016" ) )
                                ._( ComboBoxItem( "Thai017" ) )
                                ._( ComboBoxItem( "Thai018" ) )
                                ._( ComboBoxItem( "Thai019" ) )
                                ._( ComboBoxItem( "Thai020" ) )
                                ._( ComboBoxItem( "Thai021" ) )
                                ._( ComboBoxItem( "Thai022" ) )
                                ._( ComboBoxItem( "Thai023" ) )
                                ._( ComboBoxItem( "Thai024" ) )
                                ._( ComboBoxItem( "Thai025" ) )
                                ._( ComboBoxItem( "Thai026" ) )
                                ._( ComboBoxItem( "Thai027" ) )
                                ._( ComboBoxItem( "Thai028" ) )
                                ._( ComboBoxItem( "Thai999" ) )
                                .onValueChanged( [=](DecWidget &widget) {
                                    //SkDebugf( "PPEF Font Changed %i\n", (int)widget.value() );
                                    //SkDebugf( "PPEF Font Changed %i\n", (int)pPPEFFont->value() );
                                })
                            )
                            ._( Text("Density(1/mm.)") )
                            ._( Spinner(2.5, 1.0, 5.0, 0.25).ref((DecWidget**)&pPPEF_Density) )
                            ._( Text("Pull Compensate(mm.)") )
                            ._( Spinner(0.0, -0.5, 2.0, 0.1).ref((DecWidget**)&pPPEF_PullCompensate) )
                            ._( Text("Effect :") )
                            ._( ComboBox()
                                .ref((DecWidget **)&pPPEF_Effect)
                                ._( ComboBoxItem("Plain Text") )
                                ._( ComboBoxItem("Arch Up") )
                                ._( ComboBoxItem("Arch Down") )
                                ._( ComboBoxItem("Circle") )
                                ._( ComboBoxItem("Wave") )
                                ._( ComboBoxItem("Chevron Up") )
                                ._( ComboBoxItem("Chevron Down") )
                                ._( ComboBoxItem("Slant Up") )
                                ._( ComboBoxItem("Slant Down") )
                                ._( ComboBoxItem("Triangle Up") )
                                ._( ComboBoxItem("Triangle Down") )
                                ._( ComboBoxItem("Fade Right") )
                                ._( ComboBoxItem("Fade Left") )
                                ._( ComboBoxItem("Fade Up") )
                                ._( ComboBoxItem("Fade Down") )
                                ._( ComboBoxItem("Inflate") )
                                .onValueChanged( [=](DecWidget &widget) {
                                    //SkDebugf( "Effect Changed %i\n", (int)widget.value() );
                                    int selectedIndex = (int)widget.value();
                                    if(selectedIndex<=6){
                                        if(pPPEF_Border) pPPEF_Border->show();
                                        if(pPPEF_Italic) pPPEF_Italic->show();
                                        // border gap
                                        if(pPPEF_Border && pPPEF_Border->isChecked()){
                                            if(pEffect_BorderGapX) pEffect_BorderGapX->show();
                                            if(pEffect_BorderGapY) pEffect_BorderGapY->show();
                                            if(pEffect_BorderGapX_Text) pEffect_BorderGapX_Text->show();
                                            if(pEffect_BorderGapY_Text) pEffect_BorderGapY_Text->show();
                                        }
                                    }
                                    else{
                                        if(pPPEF_Border) pPPEF_Border->hide();
                                        if(pPPEF_Italic) pPPEF_Italic->hide();
                                        // border gap
                                        if(pEffect_BorderGapX) pEffect_BorderGapX->hide();
                                        if(pEffect_BorderGapY) pEffect_BorderGapY->hide();
                                        if(pEffect_BorderGapX_Text) pEffect_BorderGapX_Text->hide();
                                        if(pEffect_BorderGapY_Text) pEffect_BorderGapY_Text->hide();
                                    }
                                })
                            )
                            ._( Text("Angle:").ref((DecWidget**)&pEffect_Angle_Text) )
                            ._( Spinner(60, 15, 90, 1).ref((DecWidget**)&pEffect_Angle) )
                            ._( Text("Radius:").ref((DecWidget**)&pEffect_Radius_Text)  )
                            ._( Spinner(100, 100, 5000, 10).ref((DecWidget**)&pEffect_Radius) )
                            ._( CheckBox("Border").ref((DecWidget **)&pPPEF_Border) )
                            ._( CheckBox("Italic").ref((DecWidget **)&pPPEF_Italic) )
                            // border gap
                            ._( Text("Border gap X:").ref((DecWidget**)&pEffect_BorderGapX_Text) )
                            ._( Spinner(0, 0, 1000, 10).ref((DecWidget**)&pEffect_BorderGapX) )
                            ._( Text("Border gap Y:").ref((DecWidget**)&pEffect_BorderGapY_Text) )
                            ._( Spinner(0, 0, 1000, 10).ref((DecWidget**)&pEffect_BorderGapY) )
                            ._( CreateBrotherColorWidget([=](int colorIndex){
                                    int index = PES5_GetObjectSelectedIndex();
                                    pesData * pes = pesGetDocument()->getDataObject(index).get();
                                    pes->parameter.fillColorIndex = colorIndex;
                                    pes->fillBlocks[0].setColorFromIndex(colorIndex);
                                    PES5_UpdatePesImage(index);
                                }))
                            //._( TextEdit("ภิญญ์จักรปัก").size(-1, 0).ref((DecWidget**)&pPPEF_TextEdit))
                        )
                    )
                    ._( Button("อัพเดทข้อความ", 0, [=](DecButton & button){
                            int index = PES5_GetObjectSelectedIndex();
                            auto pes = pesGetDocument()->getDataObject(index);
                            pes->saveParameter();
                            pes->savePaths();
                            pes->parameter.text = pPPEF_TextEdit->text();
                            int selectedIndex = (int)pPPEFFont->value();
                            pes->parameter.fontName = pPPEFFont->getChildren()[selectedIndex]->text();
                            pes->parameter.fill.density = (float)pPPEF_Density->value();
                            pes->parameter.pullCompensate = (float)pPPEF_PullCompensate->value();
                            pes->parameter.shapeIndex = (int)pPPEF_Effect->value();
                            pes->parameter.border = pPPEF_Border->isChecked();
                            pes->parameter.italic = pPPEF_Italic->isChecked();
                            
                            int effectIndex = pes->parameter.shapeIndex;
                            if(effectIndex==0){
                                pes->parameter.angleValue = -1;
                                pes->parameter.radiusValue = -1;
                            }
                            else if(effectIndex==1 || effectIndex==2){
                                if(pes->parameter.angleValue!=-1)
                                    pes->parameter.angleValue = pEffect_Angle->value();
                                if(pes->parameter.radiusValue!=-1)
                                    pes->parameter.radiusValue = pEffect_Radius->value();
                            }
                        
                            // border gap
                            if(effectIndex<=6 && pes->parameter.border){
                                pes->parameter.borderGap = pEffect_BorderGapX->value();
                                pes->parameter.borderGapY = pEffect_BorderGapY->value();
                            }
                            
                            PES5_ReplacePPEFText(index);
//                            PES5_UpdateGUIRightPanel();
                            PES5_ApplyParameter(index);
                        })
                    .size(-1, 0))
                    //._( Spacer().size(-1, 160) )
                )
                
                // page 1 - ttf
                ._( Column().padding(5,5,5,5).gap(10)
                    //.size( -1, 0 )
                    .size(cW-10, -1)
                    ._( TextEdit("ภิญญ์จักรปัก").size(-1, 0).ref((DecWidget**)&pTTF_TextEdit))
                    ._( Scroller(SCROLL_VERTICAL)
                        .border(false)
                        .background( false )
                        .size(cW-20, -1)
                        ._( Column().gap( 10 )
                            .border(false)
                            .size( cW-20, 0 )


                            ._( ComboBox()
                                .ref( (DecWidget **)&pTTFFont )
                                // Starter
                                ._( ComboBoxItem( "JS-Boaboon" ).font(Font().Regular("TTF/JS-Boaboon.ttf")) )
                                ._( ComboBoxItem( "JS-75-Pumpuang" ).font(Font().Regular("TTF/JS-75-Pumpuang.ttf")) )
                                ._( ComboBoxItem( "JS-Amphan-Italic" ).font(Font().Regular("TTF/JS-Amphan-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Angsumalin" ).font(Font().Regular("TTF/JS-Angsumalin.ttf")) )
                                ._( ComboBoxItem( "JS-Chaimongkol" ).font(Font().Regular("TTF/JS-Chaimongkol.ttf")) )
                                ._( ComboBoxItem( "JS-Chalit" ).font(Font().Regular("TTF/JS-Chalit.ttf")) )
                                ._( ComboBoxItem( "JS-Chanok-Normal" ).font(Font().Regular("TTF/JS-Chanok-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Charnchai-Normal" ).font(Font().Regular("TTF/JS-Charnchai-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Chawlewhieng" ).font(Font().Regular("TTF/JS-Chawlewhieng.ttf")) )
                                ._( ComboBoxItem( "JS-Chodok-Bold" ).font(Font().Regular("TTF/JS-Chodok-Bold.ttf")) )
                                // Pro
                                ._( ComboBoxItem( "JS-Chulee" ).font(Font().Regular("TTF/JS-Chulee.ttf")) )
                                ._( ComboBoxItem( "JS-Chusri-Normal" ).font(Font().Regular("TTF/JS-Chusri-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Duangta" ).font(Font().Regular("TTF/JS-Duangta.ttf")) )
                                ._( ComboBoxItem( "JS-Giat-Bold" ).font(Font().Regular("TTF/JS-Giat-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Hariphan" ).font(Font().Regular("TTF/JS-Hariphan.ttf")) )
                                ._( ComboBoxItem( "JS-Jetarin-Italic" ).font(Font().Regular("TTF/JS-Jetarin-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Jindara-Bold" ).font(Font().Regular("TTF/JS-Jindara-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Jindara-Bold-Italic" ).font(Font().Regular("TTF/JS-Jindara-Bold-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Jindara-Italic" ).font(Font().Regular("TTF/JS-Jindara-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Jindara-Normal" ).font(Font().Regular("TTF/JS-Jindara-Normal.ttf")) )

                                ._( ComboBoxItem( "JS-Jukaphan" ).font(Font().Regular("TTF/JS-Jukaphan.ttf")) )
                                ._( ComboBoxItem( "JS-Jukaphan-Italic" ).font(Font().Regular("TTF/JS-Jukaphan-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Junkaew" ).font(Font().Regular("TTF/JS-Junkaew.ttf")) )
                                ._( ComboBoxItem( "JS-Junkaew-Italic" ).font(Font().Regular("TTF/JS-Junkaew-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Karabow" ).font(Font().Regular("TTF/JS-Karabow.ttf")) )
                                ._( ComboBoxItem( "JS-Khunwai" ).font(Font().Regular("TTF/JS-Khunwai.ttf")) )
                                ._( ComboBoxItem( "JS-Kobori-Allcaps-Bold" ).font(Font().Regular("TTF/JS-Kobori-Allcaps-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Korakhot-Normal" ).font(Font().Regular("TTF/JS-Korakhot-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Laongdao-Bold" ).font(Font().Regular("TTF/JS-Laongdao-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Likhit-Normal" ).font(Font().Regular("TTF/JS-Likhit-Normal.ttf")) )

                                ._( ComboBoxItem( "JS-Macha-Normal" ).font(Font().Regular("TTF/JS-Macha-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Mookravee-Normal" ).font(Font().Regular("TTF/JS-Mookravee-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Neeno-Normal" ).font(Font().Regular("TTF/JS-Neeno-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Ninja-Allcaps-Bold" ).font(Font().Regular("TTF/JS-Ninja-Allcaps-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Noklae-Normal" ).font(Font().Regular("TTF/JS-Noklae-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Obsaward-Italic" ).font(Font().Regular("TTF/JS-Obsaward-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Obsaward-Normal" ).font(Font().Regular("TTF/JS-Obsaward-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Oobboon-Normal" ).font(Font().Regular("TTF/JS-Oobboon-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Padachamai-Normal" ).font(Font().Regular("TTF/JS-Padachamai-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Pisit-Italic" ).font(Font().Regular("TTF/JS-Pisit-Italic.ttf")) )

                                ._( ComboBoxItem( "JS-Pisit-Normal" ).font(Font().Regular("TTF/JS-Pisit-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Pitsamai-Italic" ).font(Font().Regular("TTF/JS-Pitsamai-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Pitsanu" ).font(Font().Regular("TTF/JS-Pitsanu.ttf")) )
                                ._( ComboBoxItem( "JS-Prajuk-Italic" ).font(Font().Regular("TTF/JS-Prajuk-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Pranee-Italic" ).font(Font().Regular("TTF/JS-Pranee-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Prapakorn-Italic" ).font(Font().Regular("TTF/JS-Prapakorn-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Prapakorn-Normal" ).font(Font().Regular("TTF/JS-Prapakorn-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Prasoplarp-Italic" ).font(Font().Regular("TTF/JS-Prasoplarp-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Prasoplarp-Normal" ).font(Font().Regular("TTF/JS-Prasoplarp-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Puchong-Normal" ).font(Font().Regular("TTF/JS-Puchong-Normal.ttf")) )

                                ._( ComboBoxItem( "JS-Pudgrong-Normal" ).font(Font().Regular("TTF/JS-Pudgrong-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Pumpuang-Normal" ).font(Font().Regular("TTF/JS-Pumpuang-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Puriphop-Normal" ).font(Font().Regular("TTF/JS-Puriphop-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Rapee-Bold" ).font(Font().Regular("TTF/JS-Rapee-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Rapee-Bold-Italic" ).font(Font().Regular("TTF/JS-Rapee-Bold-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Rapee-Italic" ).font(Font().Regular("TTF/JS-Rapee-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Rapee-Normal" ).font(Font().Regular("TTF/JS-Rapee-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Sadayu-Allcaps" ).font(Font().Regular("TTF/JS-Sadayu-Allcaps.ttf")) )
                                ._( ComboBoxItem( "JS-Sadayu-Bold" ).font(Font().Regular("TTF/JS-Sadayu-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Sadayu-Bold-Italic" ).font(Font().Regular("TTF/JS-Sadayu-Bold-Italic.ttf")) )

                                ._( ComboBoxItem( "JS-Saksit-Bold" ).font(Font().Regular("TTF/JS-Saksit-Bold.ttf")) )
                                ._( ComboBoxItem( "JS-Saksit-Bold-Italic" ).font(Font().Regular("TTF/JS-Saksit-Bold-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Saksit-Italic" ).font(Font().Regular("TTF/JS-Saksit-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Saksit-Normal" ).font(Font().Regular("TTF/JS-Saksit-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Samurai-Normal" ).font(Font().Regular("TTF/JS-Samurai-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Sangravee-Normal" ).font(Font().Regular("TTF/JS-Sangravee-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Saowapark-Italic" ).font(Font().Regular("TTF/JS-Saowapark-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Saowapark-Normal" ).font(Font().Regular("TTF/JS-Saowapark-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Sarunya-Normal" ).font(Font().Regular("TTF/JS-Sarunya-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Setha-Normal" ).font(Font().Regular("TTF/JS-Setha-Normal.ttf")) )

                                ._( ComboBoxItem( "JS-Sirium-Normal" ).font(Font().Regular("TTF/JS-Sirium-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Sunsanee-Italic" ).font(Font().Regular("TTF/JS-Sunsanee-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Sunsanee-Normal" ).font(Font().Regular("TTF/JS-Sunsanee-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Thanaporn-Normal" ).font(Font().Regular("TTF/JS-Thanaporn-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Tina-Normal" ).font(Font().Regular("TTF/JS-Tina-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Toomtam-Normal" ).font(Font().Regular("TTF/JS-Toomtam-Normal.ttf")) )
                                ._( ComboBoxItem( "JS-Wanida-Italic" ).font(Font().Regular("TTF/JS-Wanida-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Wannaree-Italic" ).font(Font().Regular("TTF/JS-Wannaree-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Wansika-Italic" ).font(Font().Regular("TTF/JS-Wansika-Italic.ttf")) )
                                ._( ComboBoxItem( "JS-Yodthida" ).font(Font().Regular("TTF/JS-Yodthida.ttf")) )

                                ._( ComboBoxItem( "JS-Yodthida-Italic" ).font(Font().Regular("TTF/JS-Yodthida-Italic.ttf")) )

                                .onValueChanged( [=](DecWidget &widget) {
                                    //SkDebugf( "TTF Font Changed %i\n", (int)widget.value() );
                                })
                            )
                            ._( Text("Fill :") )
                            ._( ComboBox().ref((DecWidget**)&pFillComboBox)
                                ._( ComboBoxItem("None").value(pesData::FILL_TYPE_NONE) )
                                ._( ComboBoxItem("Stitch").value(pesData::FILL_TYPE_NORMAL) )
                                ._( ComboBoxItem("Pattern...").value(pesData::FILL_TYPE_PATTERN) )
                                ._( ComboBoxItem("Motif...").value(pesData::FILL_TYPE_MOTIF) )
                                ._( ComboBoxItem("Cross Stitch").value(pesData::FILL_TYPE_CROSS_STITCH) )
                                ._( ComboBoxItem("Dynamic").value(pesData::FILL_TYPE_DYNAMIC) )
                                .onValueChanged( [=](DecWidget &widget) {
                                    //SkDebugf( "Fill Changed %i\n", (int)widget.value() );
                                    int selectedIndex = (int)widget.value();
                                    if(selectedIndex==2){
                                        PES5_OpenPatternFillDialog();
                                    }
                                    else if(selectedIndex==3){
                                        PES5_OpenMotifFillDialog();
                                    }
                                    
                                    if(selectedIndex==1 || selectedIndex==2){
                                        pTTF_FillFading->show();
                                    }
                                    else{
                                        pTTF_FillFading->hide();
                                    }
                                })
                            )
                            // pom - fading
                            ._( CheckBox("Fading").check(false, false).ref((DecWidget**)&pTTF_FillFading)
                               )
                           
                            ._( CreateBrotherColorWidget( [=](int colorIndex){
                                    int index = PES5_GetObjectSelectedIndex();
                                    pesData * pes = pesGetDocument()->getDataObject(index).get();
                                    pes->parameter.fillColorIndex = colorIndex;
                                    for(auto & p : pes->paths){
                                        p.setFillColor(pesGetBrotherColor(colorIndex));
                                    }
                                    for(auto & block : pes->fillBlocks){
                                        block.setColorFromIndex(colorIndex);
                                    }
                                    PES5_UpdatePesImage(index);
                                })
                            )
                            ._( Text("Stroke :") )
                            ._( ComboBox().ref((DecWidget**)&pStrokeComboBox)
                                ._( ComboBoxItem("None") )
                                ._( ComboBoxItem("Running") )
                                ._( ComboBoxItem("Double Running") )
                                ._( ComboBoxItem("Triple Running") )
                                ._( ComboBoxItem("Bean Running") )
                                ._( ComboBoxItem("Satin") )
                                ._( ComboBoxItem("Motif...") )
                                .onValueChanged( [=](DecWidget &widget) {
                                    int selectedIndex = (int)widget.value();
                                    if(selectedIndex==6){
                                        PES5_OpenMotifStrokeDialog();
                                    }
                                })
                            )
                            ._( CreateBrotherColorWidget( [=](int colorIndex){
                                    int index = PES5_GetObjectSelectedIndex();
                                    pesData * pes = pesGetDocument()->getDataObject(index).get();
                                    pes->parameter.colorIndex = colorIndex;
                                    for(auto & p : pes->paths){
                                        p.setStrokeColor(pesGetBrotherColor(colorIndex));
                                    }
                                    for(auto & block : pes->strokeBlocks){
                                        block.setColorFromIndex(colorIndex);
                                    }
                                    PES5_UpdatePesImage(index);
                                })
                            )
                        )
                    )                    
                    ._( Button("อัพเดทข้อความ", 0, [=](DecButton & button){
                            int index = PES5_GetObjectSelectedIndex();
                            auto pes = pesGetDocument()->getDataObject(index);
                            pes->saveParameter();
                            pes->savePaths();
                            pes->parameter.text = pTTF_TextEdit->text();
                            int selectedIndex = (int)pTTFFont->value();
                            pes->parameter.fontName = pTTFFont->getChildren()[selectedIndex]->text();
                            // pom - fading
                            pes->parameter.fillFading = pTTF_FillFading->isChecked();
                            
                            //pes->parameter.fillTypeIndex = (int)pFillComboBox->value();
                            //pes->parameter.fillType = (pesData::FillType)(pesData::FILL_TYPE_NONE + pes->parameter.fillTypeIndex);
                            pes->parameter.fillTypeIndex = (int)pFillComboBox->getSelectedValue();
                            pes->parameter.fillType = (pesData::FillType) pFillComboBox->getSelectedValue();
                            pes->parameter.strokeTypeIndex = (int)pStrokeComboBox->value();
                            if(pes->parameter.strokeTypeIndex==6){ // 6=STROKE_TYPE_MOTIF
                                pes->parameter.strokeType = pesData::STROKE_TYPE_MOTIF;
                            }
                            else{
                                pes->parameter.strokeType = (pesData::StrokeType)(pesData::STROKE_TYPE_NONE + pes->parameter.strokeTypeIndex);
                            }
                            
                            PES5_ReplaceTTFText(pTTFFont->getChildren()[selectedIndex]->font()->_typeface[0], index);
                            PES5_ApplyParameter(index);
                        })
                        .size(-1, 0)
                    )
                )
                
                // page 2 - Monogram
                ._( Column().padding(5,5,5,5).gap( 10 )
                    .size(cW-10, -1)
                    ._( TextEdit("ภิญญ์").size(-1, 0).ref((DecWidget**)&pMonogram_TextEdit))
                    ._( Scroller(SCROLL_VERTICAL)
                        .border(false)
                        .background( false )
                        .size(cW-20, -1)
                        ._( Column().gap( 10 )
                            .border(false)
                            .size( cW-20, 0 )

                            ._( ComboBox()
                                .ref( (DecWidget **)&pMonogramFont )
                                ._( ComboBoxItem( "M001" ) )
                                ._( ComboBoxItem( "M002" ) )
                                ._( ComboBoxItem( "M003" ) )
                                ._( ComboBoxItem( "M004" ) )
                                ._( ComboBoxItem( "M005" ) )
                                .onValueChanged( [=](DecWidget &widget) {
                                    //SkDebugf( "PPEF Font Changed %i\n", (int)widget.value() );
                                })
                            )
                            ._( Text("Density(1/mm.)") )
                            ._( Spinner(2.5, 1.0, 5.0, 0.25).ref((DecWidget**)&pMonogram_Density) )
                            ._( Text("Pull Compensate(mm.)") )
                            ._( Spinner(0.0, -0.5, 2.0, 0.1).ref((DecWidget**)&pMonogram_PullCompensate) )
                        )
                    )
                    ._( Button("อัพเดทข้อความ", 0, 
                        [=](DecButton & button){
                            int index = PES5_GetObjectSelectedIndex();
                            auto pes = pesGetDocument()->getDataObject(index);
                            pes->saveParameter();
                            pes->savePaths();
                            pes->parameter.text = pMonogram_TextEdit->text();
                            int selectedIndex = (int)pMonogramFont->value();
                            pes->parameter.fontName = pMonogramFont->getChildren()[selectedIndex]->text();
                            pes->parameter.fill.density = (float)pMonogram_Density->value();
                            pes->parameter.pullCompensate = (float)pMonogram_PullCompensate->value();
                            PES5_ReplacePPEFText_V2(index);
                            PES5_ApplyParameter(index);
                        })
                        .size(-1, 0)
                    )
                )
                
                // page 3 - Satin Column
                ._( Column().padding(5,5,5,5).gap( 10 )
                    .size(cW-10, -1)
                    ._( Scroller(SCROLL_VERTICAL)
                        .border(false)
                        .background( false )
                        .size(-1, -1)
                        ._( Column().gap( 10 )
                            .border(false)
                            .size( cW-20, 0 )

                            ._( Text("Density(1/mm.)") )
                            ._( Spinner(2.5, 1.0, 5.0, 0.25).ref((DecWidget**)&pSatinColumn_Density) )
                            ._( Text("Pull Compensate(mm.)") )
                            ._( Spinner(0.0, -0.5, 2.0, 0.1).ref((DecWidget**)&pSatinColumn_PullCompensate) )
                            ._( CreateBrotherColorWidget(
                                [=](int colorIndex){
                                    int index = PES5_GetObjectSelectedIndex();
                                    pesData * pes = pesGetDocument()->getDataObject(index).get();
                                    pes->parameter.fillColorIndex = colorIndex;
                                    pes->fillBlocks[0].setColorFromIndex(colorIndex);
                                    PES5_UpdatePesImage(index);
                                })
                            )
                        )
                    )
                    ._( Button("Update", 0, 
                        [=](DecButton & button){
                            int index = PES5_GetObjectSelectedIndex();
                            auto pes = pesGetDocument()->getDataObject(index);
                            pes->saveParameter();
                            pes->savePaths();
                            pes->parameter.fill.density = (float)pSatinColumn_Density->value();
                            pes->parameter.pullCompensate = (float)pSatinColumn_PullCompensate->value();
                            pes->applyFill();
//                            PES5_UpdatePesImage(index);
                            PES5_ApplyParameter(index);
                        })
                        .size(-1, 0)
                    )
                )
                
                // page 4 - SVG
                ._( Column().padding(5,5,5,5).gap( 10 )
                    .size(cW-10, -1)
                    ._( Scroller(SCROLL_VERTICAL)
                        .border(false)
                        .background( false )
                        .size(-1, -1)

                        ._( Column().gap( 10 )
                            .border(false)
                            .size( cW-20, 0 )

                            ._( Text("Fill :") )
                            ._( ComboBox().ref((DecWidget**)&pSVGFillComboBox)
                                ._( ComboBoxItem("None").value(pesData::FILL_TYPE_NONE) )
                                ._( ComboBoxItem("Stitch").value(pesData::FILL_TYPE_NORMAL) )
                                ._( ComboBoxItem("Pattern...").value(pesData::FILL_TYPE_PATTERN) )
                                ._( ComboBoxItem("Motif...").value(pesData::FILL_TYPE_MOTIF) )
                                ._( ComboBoxItem("Cross Stitch").value(pesData::FILL_TYPE_CROSS_STITCH) )
                                ._( ComboBoxItem("Dynamic").value(pesData::FILL_TYPE_DYNAMIC) )
                                .onValueChanged( 
                                    [=](DecWidget &widget) {
                                        int selectedValue = (int)pSVGFillComboBox->getSelectedValue();
                                        if(selectedValue==pesData::FILL_TYPE_PATTERN){
                                            PES5_OpenPatternFillDialog();
                                        }
                                        else if(selectedValue==pesData::FILL_TYPE_MOTIF){
                                            PES5_OpenMotifFillDialog();
                                        }
                                        
                                        if(selectedValue==pesData::FILL_TYPE_NORMAL || selectedValue==pesData::FILL_TYPE_PATTERN){
                                            pSVG_FillFading->show();
                                        }
                                        else{
                                            pSVG_FillFading->hide();
                                        }
                                    }
                                )
                            )
                            // pom - fading
                            ._( CheckBox("Fading").check(false, false).ref((DecWidget**)&pSVG_FillFading)
                              )
                           
                            ._( CreateBrotherColorWidget(
                                [=](int colorIndex){
                                    int index = PES5_GetObjectSelectedIndex();
                                    pesData * pes = pesGetDocument()->getDataObject(index).get();
                                    pes->parameter.fillColorIndex = colorIndex;
                                    for(auto & p : pes->paths){
                                        p.setFillColor(pesGetBrotherColor(colorIndex));
                                    }
                                    for(auto & block : pes->fillBlocks){
                                        block.setColorFromIndex(colorIndex);
                                    }
                                    PES5_UpdatePesImage(index);
                                })
                            )
                            ._( Text("Stroke :") )
                            ._( ComboBox().ref((DecWidget**)&pSVGStrokeComboBox)
                                ._( ComboBoxItem("None").value(pesData::STROKE_TYPE_NONE) )
                                ._( ComboBoxItem("Running").value(pesData::STROKE_TYPE_RUNNING) )
                                ._( ComboBoxItem("Double Running").value(pesData::STROKE_TYPE_RUNNING_DOUBLE) )
                                ._( ComboBoxItem("Triple Running").value(pesData::STROKE_TYPE_RUNNING_TRIPLE) )
                                ._( ComboBoxItem("Bean Running").value(pesData::STROKE_TYPE_RUNNING_BEAN) )
                                ._( ComboBoxItem("Satin").value(pesData::STROKE_TYPE_SATIN) )
                                ._( ComboBoxItem("Motif...").value(pesData::STROKE_TYPE_MOTIF) )
                                ._( ComboBoxItem("Dynamic").value(pesData::STROKE_TYPE_DYNAMIC) )
                                .onValueChanged( [=](DecWidget &widget) {
                                     int selectedValue = (int)pSVGStrokeComboBox->getSelectedValue();
                                     if(selectedValue==pesData::STROKE_TYPE_MOTIF){
                                         PES5_OpenMotifStrokeDialog();
                                     }
                                })
                            )
                            ._( CreateBrotherColorWidget([=](int colorIndex){
                                     int index = PES5_GetObjectSelectedIndex();
                                     pesData * pes = pesGetDocument()->getDataObject(index).get();
                                     pes->parameter.colorIndex = colorIndex;
                                     for(auto & p : pes->paths){
                                         p.setStrokeColor(pesGetBrotherColor(colorIndex));
                                     }
                                     for(auto & block : pes->strokeBlocks){
                                         block.setColorFromIndex(colorIndex);
                                     }
                                     PES5_UpdatePesImage(index);
                                })
                            )
                        )
                    )
                    ._( Button("Update", 0, [=](DecButton & button){
                                int index = PES5_GetObjectSelectedIndex();
                                auto pes = pesGetDocument()->getDataObject(index);
                                pes->saveParameter();
                                pes->savePaths();
                                pes->parameter.fillTypeIndex = (int)pSVGFillComboBox->getSelectedValue();
                                pes->parameter.strokeTypeIndex = (int)pSVGStrokeComboBox->getSelectedValue();
                                pes->parameter.fillType = (pesData::FillType)pSVGFillComboBox->getSelectedValue();
                                pes->parameter.strokeType = (pesData::StrokeType)pSVGStrokeComboBox->getSelectedValue();
                                // pom - fading
                                pes->parameter.fillFading = pSVG_FillFading->isChecked();
                        
                                pes->applyFill();
                                pes->applyStroke();
//                                PES5_UpdatePesImage(index);
                                PES5_ApplyParameter(index);
                        }).size(-1, 0)
                    )
                )
                
                // page 5 - blank
                ._( Column().padding(5,5,5,5).gap(10)
                    .size(cW-10, -1) 
                )
            )
        )

        // /* 
        // comment not use
        // page Color
        ._( Column()
            .border(false)
            .gap(5)
            ._( List().size(-1, -1).ref((DecWidget **)&pColorList))
            ._( CreateBrotherColorWidget([=](int index){
                    if(PES5_GetObjectSelectedIndex()!=-1){
                        int colorListIndex = (int)pColorList->value();
                        if(colorListIndex!=-1){
                            int pesIndex = PES5_GetObjectSelectedIndex();
                            pesGetDocument()->getDataObject(pesIndex)
                            ->fillBlocks[colorListIndex].setColorFromIndex(index);
                            PES5_UpdatePesImage(PES5_GetObjectSelectedIndex());
                        }
                    }
                })
            )
            ._( Row()
                .size(-1, 0)
                .background( true )
                .bg_color(0xFFE0E0E0)
                .border( false )
                .gap( 5 )
                ._( Button("", 0xf357, [=](DecButton &btn){
                        int index = PES5_GetObjectSelectedIndex();
                        if(index!=-1){
                            int cIndex = (int)pColorList->value();
                            if(PES5_ColorBlockSwap(pesGetDocument()->getDataObject(index).get(),
                                                   cIndex, cIndex-1)){
                                PES5_UpdatePesImage(index);
                                pColorList->select_listItem(cIndex-1);
                            }
                        }
                    })
                    .size(-1, 0)
                )
                ._( Button("", 0xf354, [=](DecButton &btn){
                        int index = PES5_GetObjectSelectedIndex();
                        if(index!=-1){
                            int cIndex = (int)pColorList->value();
                            if(PES5_ColorBlockSwap(pesGetDocument()->getDataObject(index).get(),
                                                   cIndex, cIndex+1)){
                                PES5_UpdatePesImage(index);
                                pColorList->select_listItem(cIndex+1);
                            }
                        }
                    })
                    .size(-1, 0)
                )
                //._(Button("", 0xf53f).size(-1, 0))
            )
        )
        // */
    );
    
    pLayerList->select_listItem(-1);
    
//    pPropertiesWidget->activate(PPEF_PAGE);
//    pPropertiesWidget->activate(TTF_PAGE);
    pPropertiesWidget->activate(BLANK_PAGE);
    pPPEF_TextEdit->visible(false);
    pTTF_TextEdit->visible(false);
    pMonogram_TextEdit->visible(false);
    pPPEFFont->select_comboboxItem(0);
    pTTFFont->select_comboboxItem(0);
    pMonogramFont->select_comboboxItem(0);
    pFillComboBox->select_comboboxItem(0);
    pStrokeComboBox->select_comboboxItem(0);
    
    pColorList->select_listItem(-1);
    
    pLayerList->_onValueChanged = [=](DecWidget &w){
        int selectedIndex = w.value();
        //GetPesDocumentView()->setSelectedIndex(selectedIndex);
        bool isShiftKeyOnly = false;
        if (selectedIndex > -1) {
            PES5_SetObjectSelectedIndex(selectedIndex, isShiftKeyOnly);
        }
    };
    
    return *container;
}

DecList * GetLayerList(){
    return pLayerList;
}

DecScroller & CreateBrotherColorWidget(std::function<void(int index)> onSelected){
    DecScroller *container =  new DecScroller( SCROLL_HORIZONTAL );
    container->size(-1, 0);
    DecRowContainer *r;
    container->_(Row()
                 .padding( 0, 10, 10, 10 )
                 .size( 0, 0 ).gap( 2 ).border(false).background(false).ref((DecWidget **)&r));
    
    for(int i=1; i<=64; i++){
        r->_(Button("", 0, [=](DecButton &btn){
            onSelected(i);
        }).gap(0).bg_color( 0xFF000000 | pesGetBrotherColor(i).getHex() ).size( 30, 30 ).round(15));
    }
    
    return *container;
}

const sk_sp<SkTypeface> & GetDefaultTypeface(){
    return pTTFFont->getChildren()[0]->font()->_typeface[0];
}

const char* GetDefaultTypefaceName() { 
    return pTTFFont->getChildren()[0]->text(); 
}

int GetTypefaceIndex(int nObjIndex) { 
    int typefaceIndex = -1;
    if (nObjIndex == -1) {
        nObjIndex = PES5_GetObjectSelectedIndex();
    }

    if (nObjIndex >= 0 && nObjIndex < PES5_GetObjectCount()) {
        pesData* pes = pesGetDocument()->getDataObject(nObjIndex).get();
        int i = 0;
        for (auto c : pTTFFont->getChildren()) {
            if (c->text() == pes->parameter.fontName) {
                typefaceIndex = i;
                break;
            }
            i++;
        }
    }
    return typefaceIndex;
}


//////////////////////
// DecLayerListItem //
//////////////////////
DecLayerListItem::DecLayerListItem() {
    border( true );
    size( -1, 80 );
    cache( false );
}

DecLayerListItem::~DecLayerListItem() {}

DecLayerListItem &LayerListItem() {
    DecLayerListItem *w = new DecLayerListItem();
    return *w;
}

DecWidget &DecLayerListItem::select( bool i ) {
    DecWidget::select( i );
    if( i ) {
        background( true );
        bg_color( 0xFF3274EE );
    }
    else {
        background( true );
        bg_color( 0xFFFFFFFF );
    }
    return *this;
}

bool DecLayerListItem::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifier ) {
    if( state == skui::InputState::kDown ) {
        _isSelected = !_isSelected;
        if( _isSelected ) {
//            bg_color(0xFFFF0000);
            bubbleAction( ACTION_LISTITEM_SELECTED, this );
        }
        else {
//            bg_color(0xFFFFFFFF);
            bubbleAction( ACTION_LISTITEM_SELECTED, NULL );
        }
    }
    return false;
}

int DecLayerListItem::findMyIndex(){
    if(_parent){
        int cnt = _parent->_children.size();
        for(int i=0; i<cnt; i++){
            if(_parent->_children[i]==this)
                return i;
        }
    }
    
    return -1;
}

SkRect _makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void DecLayerListItem::_draw_content(SkCanvas &canvas){
    SkPaint p;
    p.setAntiAlias(true);
    p.setStroke(false);
    p.setColor(0xFFFFFFFF);
    canvas.drawRect(SkRect::MakeXYWH(5, 5, 70, 70), p);
    p.setStroke(true);
    p.setColor(0xFFD0D0D0);
    canvas.drawRect(SkRect::MakeXYWH(5, 5, 70, 70), p);
    
    int i = findMyIndex();
    const auto img = PES5_GetPesImageThumbnail(i);
    if (img == nullptr) {
        //SkDebugf("img[%d] == nullptr\n", i);
        return;
    }
    float s = img->width()>img->height() ? 70.0/img->width() : 70.0/img->height();
    const SkRect dst = _makeRectFromCenter(40, 40, img->width()*s, img->height()*s);
    canvas.drawImageRect(img, dst, SkSamplingOptions(SkFilterMode::kLinear));
    
    p.setStroke(false);
    p.setColor(0xFF000000);
    SkFont font;
    font.setTypeface(_font->typeface(1)); // 1 = FONT_BOLD
    font.setSubpixel(true);
    font.setSize(_textSize);
    auto type = pesGetDocument()->getDataParameter(i).type;
    canvas.drawString(PES5_ObjectTypeToString(type).c_str(), 80, 45, font, p);
}

//////////////////////
// DecColorListItem //
//////////////////////
DecColorListItem::DecColorListItem() {
    border( true );
    size( -1, 50 );
    cache( false );
}

DecColorListItem::~DecColorListItem() {}

DecColorListItem &ColorListItem() {
    DecColorListItem *w = new DecColorListItem();
    return *w;
}

DecWidget &DecColorListItem::select( bool i ) {
    DecWidget::select( i );
    if( i ) {
        background( true );
        bg_color( 0xFF3274EE );
    }
    else {
        background( true );
        bg_color( 0xFFFFFFFF );
    }
    
    return *this;
}

bool DecColorListItem::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifier ) {
    if( state == skui::InputState::kDown ) {
        _isSelected = !_isSelected;
        if( _isSelected ) {
//            bg_color(0xFFFF0000);
            bubbleAction( ACTION_LISTITEM_SELECTED, this );
        }
        else {
//            bg_color(0xFFFFFFFF);
            bubbleAction( ACTION_LISTITEM_SELECTED, NULL );
        }
    }
    return false;
}

int DecColorListItem::findMyIndex(){
    if(_parent){
        int cnt = _parent->_children.size();
        for(int i=0; i<cnt; i++){
            if(_parent->_children[i]==this)
                return i;
        }
    }
    
    return -1;
}

void DecColorListItem::_draw_content(SkCanvas &canvas){
    int pesIndex = PES5_GetObjectSelectedIndex();
    int blockIndex = findMyIndex();
    int colorIndex = pesGetDocument()->getDataObject(pesIndex)->fillBlocks[blockIndex].colorIndex;
    pesColor c = pesGetBrotherColor(colorIndex);
    const char* color_name = pesGetBrotherColorNameWithCatalogNumber(colorIndex);
    SkColor color = SkColorSetARGB(255, c.r, c.g, c.b);
    
    SkPaint p;
    p.setAntiAlias(true);
    p.setStrokeWidth(2);
    p.setStroke(false);
    p.setColor(color);
    canvas.drawRect(SkRect::MakeXYWH(5, 5, 40, 40), p);
    p.setStroke(true);
    p.setColor(0xFF000000);
    canvas.drawRect(SkRect::MakeXYWH(5, 5, 40, 40), p);

    p.setStroke(false);
    p.setColor(0xFF000000);
    SkFont font;
    font.setTypeface(_font->typeface(0)); // 0 = FONT_REGULAR
    font.setSubpixel(true);
    font.setSize(_textSize);
    canvas.drawString(color_name, 60, 30, font, p);
}

void UpdateGUIRightPanel(){
    // update layer list
    if(pLayerList){
        int list_cnt = pLayerList->_pColumn->_children.size();
        int pesObject_cnt = PES5_GetObjectCount();
        if(list_cnt!=pesObject_cnt){
            pLayerList->clear();
            for(int i=0; i<pesObject_cnt; i++){
                pLayerList->_(LayerListItem());
            }
            // Jimmy* - notify JS here to update layer list
            #ifdef __EMSCRIPTEN__
            EM_ASM({ 
                if (window["pes5updateLayerList"]) { window["pes5updateLayerList"]($0); }
            }, -1);
            #endif
        }
    }
    
    // update properties
    if(pPropertiesWidget){
//        if ((pBodyWidget->getActivateIndex() == 1) && pPropertiesWidget->visible()) {
          if ((pBodyWidget->getActivateIndex() == 1)) {
            int selectedIndex = PES5_GetObjectSelectedIndex();
              if (selectedIndex >= 0 && selectedIndex < PES5_GetObjectCount()) {
                // update data to dialog
                pesData *pes = pesGetDocument()->getDataObject(selectedIndex).get();
                if(pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT){
                    pPropertiesWidget->activate(PPEF_PAGE);
                    pPPEF_TextEdit->setText(pes->parameter.text);
                    pPPEF_TextEdit->visible(true);
                    pTTF_TextEdit->visible(false);
                    pMonogram_TextEdit->visible(false);
                    int cbIndex = -1;
                    int i = 0;
                    for(auto c : pPPEFFont->getChildren()){
                        if(c->text()==pes->parameter.fontName){
                            cbIndex = i;
                            break;
                        }
                        i++;
                    }
                    if(cbIndex!=-1)
                        pPPEFFont->select_comboboxItem(cbIndex);
                    pPPEF_Density->value(pes->parameter.fill.density);
                    pPPEF_PullCompensate->value(pes->parameter.pullCompensate);
                    pPPEF_Effect->select_comboboxItem(pes->parameter.shapeIndex);
                    pPPEF_Border->check(pes->parameter.border);
                    pPPEF_Italic->check(pes->parameter.italic);
                    
                    int effectIndex = pes->parameter.shapeIndex;
                    if(effectIndex==1 || effectIndex==2){ // 1=Arch up, 2=Arch down
                        pEffect_Angle->value(pes->parameter.angleValue);
                        pEffect_Radius->value(pes->parameter.radiusValue);
                        pEffect_Angle->show();
                        pEffect_Radius->show();
                        pEffect_Angle_Text->show();
                        pEffect_Radius_Text->show();
                    }
                    else{
                        pEffect_Angle->hide();
                        pEffect_Radius->hide();
                        pEffect_Angle_Text->hide();
                        pEffect_Radius_Text->hide();
                    }
                    
                    // border gap
                    if(effectIndex<=6 && pes->parameter.border){
                        pEffect_BorderGapX->value(pes->parameter.borderGap);
                        pEffect_BorderGapY->value(pes->parameter.borderGapY);
                        if(pEffect_BorderGapX) pEffect_BorderGapX->show();
                        if(pEffect_BorderGapY) pEffect_BorderGapY->show();
                        if(pEffect_BorderGapX_Text) pEffect_BorderGapX_Text->show();
                        if(pEffect_BorderGapY_Text) pEffect_BorderGapY_Text->show();
                    }
                    else{
                        if(pEffect_BorderGapX) pEffect_BorderGapX->hide();
                        if(pEffect_BorderGapY) pEffect_BorderGapY->hide();
                        if(pEffect_BorderGapX_Text) pEffect_BorderGapX_Text->hide();
                        if(pEffect_BorderGapY_Text) pEffect_BorderGapY_Text->hide();
                    }
                }
                else if(pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_TTF_TEXT){
                    pPropertiesWidget->activate(TTF_PAGE);
                    pTTF_TextEdit->setText(pes->parameter.text);
                    pPPEF_TextEdit->visible(false);
                    pTTF_TextEdit->visible(true);
                    pMonogram_TextEdit->visible(false);
                    int cbIndex = -1;
                    int i = 0;
                    for(auto c : pTTFFont->getChildren()){
                        if(c->text()==pes->parameter.fontName){
                            cbIndex = i;
                            break;
                        }
                        i++;
                    }
                    if(cbIndex!=-1)
                        pTTFFont->select_comboboxItem(cbIndex);
                
                    if(pFillComboBox){
//                        pFillComboBox->select_comboboxItem(pes->parameter.fillTypeIndex, false);
                        pFillComboBox->selectByChildValue(pes->parameter.fillTypeIndex, false);
                    }
                    if(pStrokeComboBox)
                        pStrokeComboBox->select_comboboxItem(pes->parameter.strokeTypeIndex, false);
                    // pom - fading
                    if(pTTF_FillFading)
                        pTTF_FillFading->check(pes->parameter.fillFading, false);
                }
                else if(pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2){
                    pPropertiesWidget->activate(MONOGRAM_PAGE);
                    pMonogram_TextEdit->setText(pes->parameter.text);
                    pPPEF_TextEdit->visible(false);
                    pTTF_TextEdit->visible(false);
                    pMonogram_TextEdit->visible(true);
                    int cbIndex = -1;
                    int i = 0;
                    for(auto c : pMonogramFont->getChildren()){
                        if(c->text()==pes->parameter.fontName){
                            cbIndex = i;
                            break;
                        }
                        i++;
                    }
                    if(cbIndex!=-1)
                        pMonogramFont->select_comboboxItem(cbIndex);
                    pMonogram_Density->value(pes->parameter.fill.density);
                    pMonogram_PullCompensate->value(pes->parameter.pullCompensate);
                }
                else if(pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN){
                    pPropertiesWidget->activate(SATIN_COLUMN_PAGE);
                    pPPEF_TextEdit->visible(false);
                    pTTF_TextEdit->visible(false);
                    pMonogram_TextEdit->visible(false);
                    pSatinColumn_Density->value(pes->parameter.fill.density);
                    pSatinColumn_PullCompensate->value(pes->parameter.pullCompensate);
                }
                else if(pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_SVG_FILE){
                    pPropertiesWidget->activate(SVG_PAGE);
                    pPPEF_TextEdit->visible(false);
                    pTTF_TextEdit->visible(false);
                    pMonogram_TextEdit->visible(false);
                    if(pSVGFillComboBox){
//                        if(pes->parameter.fillTypeIndex!=99)
//                            pSVGFillComboBox->select(pes->parameter.fillTypeIndex, false);
//                        pSVGFillComboBox->selectByChildValue(pes->parameter.fillType, false);
                        pSVGFillComboBox->selectByChildValue(pes->parameter.fillType, false);
                    }
                    if(pSVGStrokeComboBox){
//                        if(pes->parameter.strokeTypeIndex!=99)
//                            pSVGStrokeComboBox->select(pes->parameter.strokeTypeIndex, false);
                        pSVGStrokeComboBox->selectByChildValue(pes->parameter.strokeType, false);
                    }
                    // pom - fading
                    if(pSVG_FillFading)
                        pSVG_FillFading->check(pes->parameter.fillFading, false);
                }
                else {
                    pPropertiesWidget->activate(BLANK_PAGE);
                    pPPEF_TextEdit->visible(false);
                    pTTF_TextEdit->visible(false);
                    pMonogram_TextEdit->visible(false);
                }
            } else {
                pPropertiesWidget->activate(BLANK_PAGE);
                pPPEF_TextEdit->visible(false);
                pTTF_TextEdit->visible(false);
                pMonogram_TextEdit->visible(false);
            }
        } else {
            pPropertiesWidget->activate(BLANK_PAGE);
            pPPEF_TextEdit->visible(false);
            pTTF_TextEdit->visible(false);
            pMonogram_TextEdit->visible(false);
        }
    }
    
    // update color list
    if(pColorList){
        pColorList->clear();
        int selectedIndex = PES5_GetObjectSelectedIndex();
        if(selectedIndex>=0 && selectedIndex<PES5_GetObjectCount()){
            pesData *pes = pesGetDocument()->getDataObject(selectedIndex).get();
            if(pes->parameter.type == pesData::OBJECT_TYPE_PES){
                int color_cnt = pes->fillBlocks.size();
                for(int i=0; i<color_cnt; i++){
                    pColorList->_(ColorListItem());
                }
            }
        }
        pColorList->select_listItem(-1);
    }
}



}  // namespace sk_ui

void PES5_UpdateTTFFont( int objIndex, int ttfIndex ) {
    PES5_ReplaceTTFText(sk_ui::pTTFFont->getChildren()[ttfIndex]->font()->_typeface[0], objIndex);
}
