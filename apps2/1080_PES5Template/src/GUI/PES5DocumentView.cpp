//
//  PES5DocumentView.cpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#include "PES5DocumentView.hpp"
#include "tools/Resources.h"

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include <fstream>
#include "pesEngine.h"

#include "PES5Command.hpp"
#include "PES5RightPanel.hpp"

#include "PES5ModalDialog.hpp"
#include "Utils/PesTracingUtils.hpp"
#include "Utils/PesStitchSimulator.hpp"
#include "../PES5Template.hpp"
#include "pesPathUtility.hpp"

//extern float __pes_current_zoom;

namespace sk_ui {

static Pes5DocumentView *_instance = nullptr;
Pes5TracingDialog* _pTracingDialog = nullptr;
Pes5ShapeDialog* _pShapeDialog = nullptr;
Pes5PatternFillDialog* _pPatternFillDialog = nullptr;
Pes5MotifFillDialog* _pMotifFillDialog = nullptr;
Pes5MotifStrokeDialog* _pMotifStrokeDialog = nullptr;
Pes5SettingDialog* _pSettingDialog = nullptr;
Pes5OutlineShapeDialog* _pOutlineShapeDialog = nullptr;


// Smart Stroke Parameters
bool _displayDOT = true;
bool _displayTINY = false;
float _strokeSize = 12;
float _cornerEntryAngleThreshol = 10;
int _corner_threshold = 3; // Consider it's corner
float _cornerSignificantAngleThreshold = 20;

float _angleEntryAngleThreshol = 20;
float _dotInterval = 2.0;

bool _angle_id_display = false;
bool _angle_start_id_display = false;
bool _angle_end_id_display = false;
bool _corner_id_display = true;
bool _lines_id_display = true;


bool _display_line0 = true;
bool _display_line1 = true;
bool _display_line2 = true;
bool _display_line3 = true;
bool _display_line8 = true;
bool _display_line16 = false;

bool _process_line0 = false;
bool _process_line1 = false;
bool _process_line2 = false;
bool _process_line3 = false;
bool _process_line8 = false;

Pes5DocumentView & CreatePesDocumentView(){
    Pes5DocumentView *view = new Pes5DocumentView();
    view->border(false);
    view->bg_color(0xffffffff);
    _instance = view;
    return *view;
}

static void AllLayerMerge() {
    auto pesDoc = pesGetDocument();
    pesData mergedData = pesMergeAllData( *pesDoc );
    mergedData.orderByColor();

    pesDoc->newDocument();
    pesDoc->addObject(mergedData);

    PES5_UpdatePesImages();
    PES5_UpdateGUIRightPanel();
}

void Pes5DocumentView::js_callback( std::string message ) {
    bool isValid = true;
    if( message == "NewDocument" ) {
        PES5_NewDocument();
    }
    else if( message == "OpenDocument" ) {
        PES5_OpenPPESFile();        
    }
    else if( message == "SaveDocument" ) {
        std::string strFileName = PES5_GetDocumentName();
        // strFileName += ".ppes";
        strFileName += ".ppes5";
        PES5_SavePPESFile(strFileName.c_str());
    }
    else if( message == "ImportPES" ) {
        PES5_ImportPESFile();
    }
    else if( message == "ImportSVG" ) {
        PES5_ImportSVGFile();
    }
    else if( message == "ImportBackground" ) {
        PES5_ImportBackgroundFile();
    }
    else if( message == "ExportPES" ) {
        SkDebugf( "ExportPES command\n" );
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".pes";
        PES5_ExportPESFile(strFileName.c_str(),true);  // center
    }
    else if( message == "ExportDST" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".dst";
        PES5_ExportDSTFile(strFileName.c_str());
    }
    else if( message == "ExportEXP" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".exp";
        PES5_ExportEXPFile(strFileName.c_str());
    }
    else if( message == "ExportJEF" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".jef";
        PES5_ExportJEFFile(strFileName.c_str());
    }    
    else if( message == "ExportXXX" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".xxx";
        PES5_ExportXXXFile(strFileName.c_str());
    }    
    else if( message == "ExportPNG" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".png";
        PES5_ExportPNGFile(strFileName.c_str());
    }
    else if( message == "ExportSVG" ) {
        std::string strFileName = PES5_GetDocumentName();
        strFileName += ".svg";
        PES5_ExportSVGFile(strFileName.c_str());
    }
    else if( message == "TraceBitmap" ) {
        PES5_TracingPNGFile();
    }    
    else if( message == "SimSlow" ) {
        auto sim = PesStitchSimulator::getInstance();
        sim->slowdown();
    }  
    else if( message == "SimStart" ) {
        auto sim = PesStitchSimulator::getInstance();
        if(sim->isPause())
            sim->resume();
        else{
            sim->stop();
            PES5_PrepareStitchSimulatorData();
            sim->start();
        }
    }  
    else if( message == "ZoomIn" ) {
        PES5_ZoomIn();        
    }
    else if( message == "ZoomOut" ) {
        PES5_ZoomOut();        
    }
    else if( message == "ResetZoom" ) {
        PES5_ResetZoomScale();
    }
    else if( message == "SimPause" ) {
        auto sim = PesStitchSimulator::getInstance();
        sim->pause();
    }
    else if( message == "SimStop" ) {
        auto sim = PesStitchSimulator::getInstance();
        sim->stop();
    }
    else if( message == "SimFast" ) {
        auto sim = PesStitchSimulator::getInstance();
        sim->speedup();
    }
    else if( message == "Settings" ) {
        PES5_OpenSettingDialog();
    }
    else if( message == "AddPPEFText" ) {
        PES5_AddPPEFText();
    }
    else if( message == "AddTTFText" ) {
        PES5_AddTTFText();
    }
    else if( message == "AddPPEFTextV2" ) {
        PES5_AddPPEFText_V2();
    }
    else if( message == "AddShape" ) {
        PES5_OpenShapeDialog();
    }
    else if( message == "AddSatinColumn" ) {
        if(PES5_IsSatinColumnInputStarted()==false){
            //SkDebugf( "Start Satin Column from MSG\n" );
            PES5_StartSatinColumnInput();
        }
        else{
            PES5_StopSatinColumnInput();
        }
    }
    else if( message == "AddDraw" ) {
        if(PES5_IsDrawingInputStarted()==false){
            PES5_StartDrawingInput();
        }
        else{
            PES5_StopDrawingInput();
        }        
    }
    else if( message == "AddLines" ) {
        if(PES5_IsLinesToolInputStarted()==false){
            //SkDebugf( "Start Lines Tool from MSG\n" );
            PES5_StartLinesToolInput();
        }
        else{
            PES5_StopLinesToolInput();
        }
    }
    else if( message == "AddPolygon" ) {
        if(PES5_IsPolygonToolInputStarted()==false){
            //SkDebugf( "Start Polygon Tool from MSG\n" );
            PES5_StartPolygonToolInput();
        }
        else{
            PES5_StopPolygonToolInput();
        }
    }
    else if( message == "AddOutlineShape" ){
//        PES5_AddObjectOutlineShape();
        PES5_OpenOutlineShapeDialog();
    }
    else if( message == "PathEdit" ) {
        if(PES5_IsPathEditInputStarted()==false){
            //SkDebugf( "Start PathEdit from MSG\n" );
            PES5_StartPathEditInput();
        }
        else{
            PES5_StopPathEditInput();
        }
    }
    else if( message == "StitchEdit" ) {
        if(PES5_IsStitchEditInputStarted()==false){
            //SkDebugf( "Start StitchEdit from MSG\n" );
            PES5_StartStitchEditInput();
        }
        else{
            PES5_StopStitchEditInput();
        }
    }
    else if( message == "StitchSimulator" ) {
        if(PES5_IsStitchSimulatorMode()==false){
            //SkDebugf( "Start StitchSimulator from MSG\n" );
            PES5_StartStitchSimulatorMode();
        }
        else{
            PES5_StopStitchSimulatorMode();
        }
    }
    else if( message == "LayerUp" ) {
        PES5_LayerMoveUp();      
    }
    else if( message == "LayerDown" ) {
        PES5_LayerMoveDown();
    }    
    else if( message == "LayerMerge" ) {
        PES5_LayerMerge();
    }    
    else if( message == "LayerBreak" ) {
        PES5_LayerBreak();
    }    
    else if( message == "LayerDuplicate" ) {
        PES5_LayerDuplicate();
    }    
    else if( message == "LayerDelete" ) {
        PES5_LayerDelete();
    }    
    //else if( message == "ExportSVG" ) {
    //    PES5_ExportSVGFile();
    //}  
    else if( message == "MergeAllLayers" ) {
        AllLayerMerge();
    }  
    else if( message == "DotDisplay" ) {
        _displayDOT = !_displayDOT;
    }    
    else if( message == "TinyDisplay" ) {
        _displayTINY = !_displayTINY;
    }    
    else if( message == "LineDisplay0" ) {
        _display_line0 = !_display_line0;
    }    
    else if( message == "LineDisplay1" ) {
        _display_line1 = !_display_line1;
    }    
    else if( message == "LineDisplay2" ) {
        _display_line2 = !_display_line2;
    }    
    else if( message == "LineDisplay3" ) {
        _display_line3 = !_display_line3;
    }    
    else if( message == "LineDisplay8" ) {
        _display_line8 = !_display_line8;
    }    
    else if( message == "LineDisplay16" ) {
        _display_line16 = !_display_line16;
    }    
    else if( message == "LineProcess0" ) {
        _process_line0 = true;
    }    
    else if( message == "LineProcess1" ) {
        _process_line1 = true;
    }    
    else if( message == "LineProcess2" ) {
        _process_line2 = true;
    }    
    else if( message == "LineProcess3" ) {
        _process_line3 = true;
    }    
    else if( message == "LineProcess8" ) {
        _process_line8 = true;
    }    
    else {
        SkDebugf( "Pes5DocumentView Callback: %s\n", message.c_str() );
        isValid = false;
    }

    if (isValid) {
        dirty(true, false);
    }
}


bool Pes5DocumentView::onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey mod) {
    //SkDebugf( "Wheel: %0.2f %0.2f\n", deltax, delta );
    bool b = DecWidget::onMouseWheel( x, y, deltax, delta, mod);
    //__pes_current_zoom += (deltax / 5.0);
    //SkDebugf( "Zoom: %0.4f\n", __pes_current_zoom );
    return b;
}


Pes5DocumentView * GetPesDocumentView(){
    return _instance;
}

Pes5TracingDialog * GetPesTracingDialog(){
    return _pTracingDialog;
}

Pes5ShapeDialog * GetPesShapeDialog(){
    return _pShapeDialog;
}

Pes5PatternFillDialog * GetPesPatternFillDialog(){
    return _pPatternFillDialog;
}

Pes5MotifFillDialog * GetPesMotifFillDialog(){
    return _pMotifFillDialog;
}

Pes5MotifStrokeDialog * GetPesMotifStrokeDialog(){
    return _pMotifStrokeDialog;
}

Pes5SettingDialog * GetPesSettingDialog(){
    return _pSettingDialog;
}

Pes5OutlineShapeDialog * GetPesOutlineShapeDialog(){
    return _pOutlineShapeDialog;
}

#ifdef __EMSCRIPTEN__
void em_updatehash() {
    EM_ASM({
        window['pes_data'] && window['pes_data']['file'] && 
        window['pes_data']['file']['updateHash'] && 
        (async() => await window['pes_data']['file']['updateHash']())()
    });
}
#endif
    
Pes5DocumentView::Pes5DocumentView(){
    //_isCache = true;
    selectedIndex = -1;
    pressed = false;
    selectedCorner = "undefined";
    bDrawGrid = true;
    
    size(-1, -1);
    
    _( FileDialogBox() );
    _( TracingDialog().ref((DecWidget**)&_pTracingDialog) );
    _( ShapeDialog().ref((DecWidget**)&_pShapeDialog) );
    _( PatternFillDialog().ref((DecWidget**)&_pPatternFillDialog) );
    _( MotifFillDialog().ref((DecWidget**)&_pMotifFillDialog) );
    _( MotifStrokeDialog().ref((DecWidget**)&_pMotifStrokeDialog) );
    _( SettingDialog().ref((DecWidget**)&_pSettingDialog) );
    _( OutlineShapeDialog().ref((DecWidget**)&_pOutlineShapeDialog) );
    
    PES5_NewDocument();
    #ifdef __EMSCRIPTEN__    
    if( (PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_JUMP_STITCH) ) {
        pesGetDocument()->setShowJumpStitch( false );
    }
    else {
        pesGetDocument()->setShowJumpStitch( true );
    }
    if( (PES5_GLOBAL_FLAGS & PES5_FLAG_RENDER_TEXTURE) ) {
        pesGetDocument()->setDrawStitchTexture( true );
    }
    else {
        pesGetDocument()->setDrawStitchTexture( false );
    }
    if( !(PES5_GLOBAL_FLAGS & PES5_FLAG_NO_WELCOME) ) {
    #endif

    PES5_LoadOrDownloadAsset("welcome.ppes", [](sk_sp<SkData> data){
        pesBuffer buff((const char*)data->data(), data->size());
        if(pesGetDocument()->loadPPESFromBuffer(buff)){
            PES5_UpdatePesImages();
            PES5_UpdateGUIRightPanel();
        }
#ifdef __EMSCRIPTEN__    
        em_updatehash();
#endif    
    });
#ifdef __EMSCRIPTEN__    
    } else {
        em_updatehash();
    }
#endif    
//    // Test potrace
//    PES5_LoadOrDownloadAsset("pes/burgerking.png", [](sk_sp<SkData> data){
//        PesTracingUtils tracer;
//        tracer.setBrightnessThreshold(0.75);
//        SkBitmap bitmap;
//        if(DecodeDataToBitmap(data, &bitmap)){
//            auto results = tracer.trace(bitmap);
//            int n = results.size();
//            pesData pes;
//            for(int i=0; i<n; i++){
//                pesPath p = results[i].getPathData();
//                p.setFilled(true);
//                pes.paths.push_back(p);
//            }
//            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
//            pesVec2f center = pes.getBoundingBox().getCenter();
//            pes.translate(-center.x, -center.y);
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
//        }
//    });
//    PES5_LoadOrDownloadAsset("pes/burgerking.png", [](sk_sp<SkData> data){
//        PesTracingUtils tracer;
//        tracer.setTraceType(TRACE_QUANT_COLOR);
//        tracer.setBrightnessThreshold(0.75);
//        tracer.setMultiScanNrColors(4);
//        SkBitmap bitmap;
//        if(DecodeDataToBitmap(data, &bitmap)){
//            auto results = tracer.trace(bitmap);
//            int n = results.size();
//            pesData pes;
//            for(int i=0; i<n; i++){
//                pesPath p = results[i].getPathData();
//                p.setFilled(true);
//                pes.paths.push_back(p);
//            }
//            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
//            pesVec2f center = pes.getBoundingBox().getCenter();
//            pes.translate(-center.x, -center.y);
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
//        }
//    });
    
    
    // Test
//    for(int i=1; i<=17; i++){
//        char path[64];
//        sprintf(path, "Pattern/pat%02d.pas", i);
//        sk_sp<SkData> data = GetResourceAsData(path);
//        const char* str = (const char*)data->data();
////        printf("%s\n,", str);
//    }
    
//    // Test DST
//    PES5_NewDocument();
//    PES5_LoadOrDownloadAsset("test2.dst", [](sk_sp<SkData> data){
//        pesBuffer buff((const char*)data->data(), data->size());
//        pesData dst;
//        if(dst.loadDSTFromBuffer(buff)){
//            pesGetDocument()->addObject(dst);
//            PES5_UpdatePesImages();
//            PES5_UpdateGUIRightPanel();
//        }
//    });
    
// #ifndef DEMO_EXPIRE_END_OF_JULY
//     #define DEMO_EXPIRE_END_OF_JULY
// #endif
    
// #ifdef DEMO_EXPIRE_END_OF_JULY
//     const time_t second_per_minute = 60;
//     const time_t second_per_hour   = second_per_minute * 60;
//     const time_t second_per_day    = second_per_hour * 24;
    
// //    time_t expire_time = 1685476820; // (31 May 2023 03:00 UTC+7)
// //    time_t expire_time = 1686601800; // (13 Jun 2023 03:30 UTC+7)
// //    time_t expire_time = 1688218400; // (1 Jul 20:30 UTC+7)
//     time_t expire_time = 1691026305; // (3August 08:30 UTC+7)
//     time_t now = time(0);
// //    time_t next_30_days = now + (second_per_day * 30);
// //    SkDebugf("now=%ld, next30day=%ld\n", now, next_30_days);
//     SkDebugf("now=%d, expire=%d\n", now, expire_time);
//     if(now > expire_time){
//         SkDebugf("Demo period expired!\n");
//         exit(1);
//     }
//     else{
//         time_t remain_time = expire_time - now;
//         time_t remain_day = remain_time / second_per_day;
//         time_t remain_hour = (remain_time - remain_day * second_per_day) / second_per_hour;
//         time_t remain_minute = (remain_time - remain_day * second_per_day - remain_hour * second_per_hour) / second_per_minute;
//         SkDebugf("Demo period remain: %d Day - %d Hour - %d Minute\n", remain_day, remain_hour, remain_minute);
//     }
// #endif
//    // Test Multi Selection
//    PES5_MSSelectAll();
//    PES5_MSRotate(45);
//    PES5_MSRotate(45);
//    PES5_MSRotate(45);
}

Pes5DocumentView::~Pes5DocumentView(){}

void Pes5DocumentView::drawGrid(SkCanvas& canvas, bool draw_grid ){
    const float doc_scale = PES5_GetZoomScale();
    const float scalelinethick = doc_scale < 1 ? 1 : doc_scale;
    const float gridlinethick = 0.5f / scalelinethick;
    const float origlinethick = 1.5f / scalelinethick;
    const float hooplinethick = 2.0f / scalelinethick;

    const int GRID_SIZE_MM = 10;
    const int UNIT_PER_MM = 10;
    pesVec2f hoop = pesGetDocument()->getHoopSizeInMM();
    float HW = hoop.x / 2; // half width
    float HH = hoop.y / 2; // half height
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(gridlinethick);
    paint.setAntiAlias(true);
    paint.setColor(SkColorSetARGB(255, 128, 128, 128));
    paint.setBlendMode(SkBlendMode::kDifference);

    if( draw_grid ) {
        // vertical line
        float Y = GRID_SIZE_MM;
        while(Y<HH){
            canvas.drawLine(-HW*UNIT_PER_MM,  Y*UNIT_PER_MM, HW*UNIT_PER_MM,  Y*UNIT_PER_MM, paint);
            canvas.drawLine(-HW*UNIT_PER_MM, -Y*UNIT_PER_MM, HW*UNIT_PER_MM, -Y*UNIT_PER_MM, paint);
            Y += GRID_SIZE_MM;
        }
    
        // horizontal line
        float X = GRID_SIZE_MM;
        while(X<HW){
            canvas.drawLine( X*UNIT_PER_MM, -HH*UNIT_PER_MM,  X*UNIT_PER_MM, HH*UNIT_PER_MM, paint);
            canvas.drawLine(-X*UNIT_PER_MM, -HH*UNIT_PER_MM, -X*UNIT_PER_MM, HH*UNIT_PER_MM, paint);
            X += GRID_SIZE_MM;
        }
    }

    paint.setStrokeWidth(origlinethick);
    if( draw_grid ) {
        canvas.drawLine(-HW*UNIT_PER_MM, 0, HW*UNIT_PER_MM, 0, paint);
        canvas.drawLine(0, -HH*UNIT_PER_MM, 0, HH*UNIT_PER_MM, paint);
    }

    // hoop border
    paint.setStrokeWidth(hooplinethick);
    SkRect rect = SkRect::MakeLTRB(-HW*UNIT_PER_MM, -HH*UNIT_PER_MM, HW*UNIT_PER_MM, HH*UNIT_PER_MM);
    canvas.drawRoundRect(rect, 50, 50, paint);
}

int Pes5DocumentView::getNearestCorner( pesVec2f pt, float max_distance ) {
    float nearestDist = 9999999;
    float nearestIndex = -1;
    int i=0;
    for( i=0; i<corners.size(); i++ ) {
        pesVec2f v( corners[i] );
        float d = v.distance(pt);
        if( d > max_distance )
            continue;
        if( d < nearestDist ) {
            nearestDist = d;
            nearestIndex = i;
        }
    }
    if( nearestIndex != -1 ) {
        if( nearestDist >= 1 ) {
            //SkDebugf( "Nearest: %0.2f\n", nearestDist );
        }
    }
    return nearestIndex;
}

int Pes5DocumentView::getNearestAngle( pesVec2f pt, float max_distance, int except_id ) {
    float nearestDist = 9999999;
    float nearestIndex = -1;
    int i=0;
    for( i=0; i<angles_end.size(); i++ ) {
        if( i == except_id ) 
            continue;
        pesVec2f v( angles_end[i] );
        float d = v.distance(pt);
        if( d > max_distance )
            continue;
        if( d < nearestDist ) {
            nearestDist = d;
            nearestIndex = i;
        }
    }
    for( i=0; i<angles_start.size(); i++ ) {
        if( i == except_id ) 
            continue;
        pesVec2f v( angles_start[i] );
        float d = v.distance(pt);
        if( d > max_distance )
            continue;
        if( d < nearestDist ) {
            nearestDist = d;
            nearestIndex = i;
        }
    }    
    if( nearestIndex != -1 ) {
        if( nearestDist >= 1 ) {
            //SkDebugf( "Nearest: %0.2f\n", nearestDist );
        }
    }
    return nearestIndex;
}

int Pes5DocumentView::getLineCut( pesVec2f pt1, pesVec2f pt2, float max_distance, float max_degree, int except_id, int *ptIndex ) {
    pesVec2f v = pt2-pt1;
    int minLine = -1;
    int minPt = -1;
    float minAngle = 999.0;
    for( int j=0; j<lines.size(); j++ ) {
        if( j == except_id ) {
            continue;
        }
        if( lines[j].size() < 2 ) {
            continue;
        }
        for( int i=0; i<lines[j].size()-1; i++ ) {
            if( pt2.distance(lines[j][i] ) <= max_distance ) 
            {
                pesVec2f v2 = lines[j][i]-pt1;
                pesVec2f v3 = lines[j][i+1]-lines[j][i];
                float ang = abs(v.angle( v2 ));
                float ang2 = abs( v3.angle( v ));
                if( ang <= max_degree && ang2 > 45 && ang2 < 135 ) 
                {
                    if( pt1.distance(lines[j][i]) > pt2.distance(lines[j][i]) )  
                    {
                        if( ang < minAngle ) {
                            minAngle = ang;
                            minLine = j;
                            minPt = i;
                        }
                    }
                }
            }
        }
    }
    if( ptIndex ) {
        *ptIndex = minPt;
    }
    return minLine;
}

int Pes5DocumentView::getTargetCorner( pesVec2f pt1, pesVec2f pt2, float max_distance, float max_degree ) {
    float nearestDist = 9999999;
    float nearestIndex = -1;
    pesVec2f dir1 = pt2-pt1;
    int i=0;
    for( i=0; i<corners.size(); i++ ) {
        pesVec2f v( corners[i] );
        float d = v.distance(pt2);
        if( d > max_distance )
            continue;        

        pesVec2f dir2 = v - pt1;
        pesVec2f dir3 = v - pt2;

        if( (dir3.length() < dir2.length()) && dir1.angle(dir2) < max_degree ) {
            if( d < nearestDist ) {
                nearestDist = d;
                nearestIndex = i;
            }
        }
    }
    return nearestIndex;
}

const float grulerw = 25;
void Pes5DocumentView::drawRuler(SkCanvas& canvas, bool isDarkMode) {
    SkFont font;
    font.setTypeface(_font->typeface(0));  // 0 = FONT_REGULAR

    const float zs = PES5_GetZoomScale();
    const int UNIT_PER_MM = 10;
    const float ONE_MM = UNIT_PER_MM * zs;
    const float ONE_CM = ONE_MM * 10;

    const float centerX = _width  / 2 + PES5_GetPan().x;
    const float centerY = _height / 2 + PES5_GetPan().y;
    const SkRect rect_h = SkRect::MakeXYWH(0, 0, _width, grulerw);
    const SkRect rect_v = SkRect::MakeXYWH(0, 0, grulerw, _height);

    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(1);
    paint.setAntiAlias(true);

    // save states layer, allocates a SkSurface for subsequent drawing. 
    // and blend-diff when restore
    SkPaint blendDiff;
    blendDiff.setBlendMode(SkBlendMode::kDifference);
    canvas.saveLayer(nullptr, &blendDiff);

    // save states layer, allocates a SkSurface for subsequent drawing. 
    // and blend with alpha when restore
    SkPaint alpha;
    alpha.setAlphaf(0.2f);
    canvas.saveLayer(nullptr, &alpha);
    paint.setARGB(0xff, 0x40, 0x40, 0x40);
    canvas.drawRect(rect_h, paint);
    canvas.drawRect(rect_v, paint);
    // restore and fill with 20% transparent
    canvas.restore();

    paint.setARGB(0xc0, 0x70, 0x70, 0x70);
    canvas.drawLine(grulerw, grulerw, rect_h.width(), grulerw, paint);
    canvas.drawLine(grulerw, grulerw, grulerw, rect_v.height(), paint);    
    
    paint.setARGB(0xff, 0x80, 0x80, 0x80);
    float y = 0;
    float x = 0;
    float start = 0;
    float p = 2;
    int number = 0;

    start = centerX;
    number = 0;
    while(start<_width){ // X+
        for(int i=0; i<10; i++){ // loop every 1 mm
            int h = 12;
            if(i==5) h = 8;
            else if(i%10!=0) h = 4;
            x = start + (i*ONE_MM);
            canvas.drawLine(x, 0, x, h, paint);
        }
        if((zs<0.3 && number%2==0) || (zs>0.29))
            canvas.drawString(pesToString(number).c_str(), start, grulerw-p, font, paint);
        number++;
        start+=ONE_CM; // loop every 1 cm
    }
    
    start = centerX;
    number = 0;
    while(start>0){ // X-
        for(int i=0; i<10; i++){ // loop every 1 mm
            if (number == 0 && i == 0) continue;
            int h = 12;
            if(i==5) h = 8;
            else if(i%10!=0) h = 4;
            x = start - (i*ONE_MM);
            canvas.drawLine(x, 0, x, h, paint);
        }
        if(number != 0 && ((zs<0.3 && number%2==0) || (zs>0.29)))
            canvas.drawString(pesToString(number).c_str(), start, grulerw-p, font, paint);
        number--;
        start-=ONE_CM; // loop every 1 cm
    }
    
    start = centerY;
    number = 0;
    while(start<_height){ // Y+
        for(int i=0; i<10; i++){ // loop every 1 mm
            int h = 12;
            if(i==5) h = 8;
            else if(i%10!=0) h = 4;
            y = start + (i*ONE_MM);
            canvas.drawLine(0, y, h, y, paint);
        }
        canvas.save();
        {
            canvas.rotate(-90);
            if((zs<0.3 && number%2==0) || (zs>0.29))
                canvas.drawString(pesToString(number).c_str(), -start, grulerw-p, font, paint);
        }
        canvas.restore();
        number++;
        start+=ONE_CM; // loop every 1 cm
    }
    
    start = centerY;
    number = 0;
    while(start>0){ // Y-
        for(int i=0; i<10; i++){ // loop every 1 mm
            if (number == 0 && i == 0) continue;
            int h = 12;
            if(i==5) h = 8;
            else if(i%10!=0) h = 4;
            y = start - (i*ONE_MM);
            canvas.drawLine(0, y, h, y, paint);
        }
        canvas.save();
        {
            canvas.rotate(-90);
            if(number != 0 && ((zs<0.3 && number%2==0) || (zs>0.29)))
                canvas.drawString(pesToString(number).c_str(), -start, grulerw-p, font, paint);
        }
        canvas.restore();
        number--;
        start-=ONE_CM; // loop every 1 cm
    }

    // restore and blend-diff 
    canvas.restore();
}

SkRect makeRectFromCenter(float x, float y, float w, float h){
    return SkRect::MakeXYWH(x-w*0.5, y-h*0.5, w, h);
}

void Pes5DocumentView::makeEvenlyPolyline( std::vector<pesVec2f> &source, std::vector<pesVec2f> &target, float dotInterval ) {
    pesVec2f vt0 = source[0];
    target.push_back( vt0 );

    float cur_dist = 0;

    for( int v=1; v<source.size(); v++ ) {
        pesVec2f vt = source[v];
        float dist = vt.distance( vt0 );

        if( (cur_dist + dist) < dotInterval ) {
            cur_dist += dist;
            vt0 = vt;
            continue;
        }
        float ldist = dotInterval - cur_dist;
        pesVec2f dir = vt - vt0;
        dir.normalize();
        while( ldist <= dist ) {
            pesVec2f vtd = vt0 + (dir * ldist);
            target.push_back( vtd );
            ldist += dotInterval;
        }
        vt0 = vt;
        cur_dist = dotInterval-(ldist - dist);
    }

    if( target[0].distance(target[target.size()-1]) < 0.5 ) {
        target.pop_back();
    }
}

int Pes5DocumentView::getLineByPoint( pesVec2f pt, int start_index, float max_distance, int except_id ) {
    for( int l=start_index; l<lines.size(); l++ ) {
        if( l == except_id ) {
            continue;
        }
        if( lines_flag[l] != 8 ) {
            continue;
        }
        for( int v=0; v<lines[l].size(); v++ ) {
            pesVec2f vx = lines[l][v];
            if( vx.distance(pt) < max_distance ) {
                return l;
            }
        }
    }
    return -1;
}

void Pes5DocumentView::makeNomals( std::vector<pesVec2f> &source, std::vector<pesVec2f> &target ) {
    // calculate normal
    for( int v=0; v<source.size(); v++ ) {
        pesVec2f vtp;
        pesVec2f vtn;
        if( v == 0 ) {
            vtp = source[source.size()-1];
        }
        else {
            vtp = source[v-1];
        }
        if( v == source.size()-1) {
            vtn = source[0];
        }
        else {
            vtn = source[v+1];
        }
        pesVec2f tangent = vtn - vtp;
        pesVec2f norm = tangent * -1;
        norm.perpendicular();
        pesVec2f nm = norm;  
        target.push_back( nm );
    }    
}

SkRect __modeRect;
void Pes5DocumentView::_draw_content(SkCanvas& canvas) {
    if (!_isVisible) {
        return;
    }

    // fixed: error on js call PES5_NewDocument()
    int imgcnt = PES5_GetPesImageCount();
    int objcnt = PES5_GetObjectCount();
    if (imgcnt > objcnt) {
        PES5_UpdatePesImages();
        imgcnt = PES5_GetPesImageCount();
        objcnt = PES5_GetObjectCount();
        if (imgcnt > objcnt) {
            imgcnt = objcnt;
        }
    }
    int cnt = imgcnt;

    float doc_scale = PES5_GetZoomScale();
    const float scalelinethick = doc_scale < 1 ? 1 : doc_scale;

    canvas.save();
    {
        pesVec2f pan = PES5_GetPan();
        canvas.translate((_width / 2) + pan.x, (_height / 2) + pan.y);
        canvas.scale(doc_scale, doc_scale);

        auto drawObjects = [=](SkCanvas& canvas) -> void {
            bool isSingleSelect = selectedIndex > -1;
            bool isMultiSelect = PES5_MSSelectCount() > 1;
            pesRectangle groupbound;
            if (isMultiSelect) {
                groupbound = PES5_MSGetBoundingBox();
            }

            // draw object
            for (int i = 0; i < cnt; i++) {
                const auto pes = pesGetDocument()->getDataObject(i);
                pesRectangle bound(pes->getBoundingBox());
                pesData::ObjectType type = pes->parameter.type;

                const float bw = bound.width; 
                const float bh = bound.height; 
                const float hbw = SkScalarHalf(bw); 
                const float hbh = SkScalarHalf(bh); 

                // draw image
                canvas.save();
                {
                    const auto img = PES5_GetPesImage(i);
                    SkRect dst = SkRect::MakeLTRB(-hbw, -hbh, hbw, hbh);
                    
                    if (type != pesData::OBJECT_TYPE_BACKGROUND) {
                        const int margin = 3;
                        const int double_margin = margin * 2;
                        int dstw = (int)floor(bw + double_margin);
                        int dsth = (int)floor(bh + double_margin);
                        dst = SkRect::MakeXYWH(-hbw - margin, -hbh - margin, dstw, dsth);
                    }

                    if (i == selectedIndex && actionDesc.action == "drag") {
                        if (type == pesData::OBJECT_TYPE_BACKGROUND) {
                            canvas.translate(bound.getCenter().x, bound.getCenter().y);
                        } else {
                            canvas.translate(bound.getCenter().x + actionDesc.tx, bound.getCenter().y + actionDesc.ty);
                        }
                        const float rotateDegree = pesGetDocument()->getDataObject(i)->parameter.rotateDegree;
                        if (type != pesData::OBJECT_TYPE_BACKGROUND) 
                            canvas.rotate(rotateDegree);

                        SkPaint paint;
                        paint.setAlpha(0x80);
                        canvas.drawImageRect(img, dst, SkSamplingOptions(), &paint);
                    } else if (i == selectedIndex && actionDesc.action == "rotate") {
                        canvas.translate(bound.getCenter().x, bound.getCenter().y);
                        if (type != pesData::OBJECT_TYPE_BACKGROUND)
                            canvas.rotate(actionDesc.degree);

                        SkPaint paint;
                        paint.setAlpha(0x80);
                        canvas.drawImageRect(img, dst, SkSamplingOptions(), &paint);
                    } else {
                        canvas.translate(bound.getCenter().x, bound.getCenter().y);
                        const float rotateDegree = pesGetDocument()->getDataObject(i)->parameter.rotateDegree;
                        if (type != pesData::OBJECT_TYPE_BACKGROUND) 
                            canvas.rotate(rotateDegree);

                        if (type == pesData::OBJECT_TYPE_BACKGROUND) {
                            canvas.drawImageRect(img, dst, SkSamplingOptions());
                        } else {
                            if (pesGetDocument()->getDataObject(i)->getTotalStitches()) {
                                if (doc_scale > 2) {
                                    pesGetDocument()->drawPesObject(&canvas, i, doc_scale);
                                } else {
                                    canvas.drawImageRect(img, dst, SkSamplingOptions());
                                }
                            } else {
                                if (doc_scale > 1) {
                                    pesGetDocument()->drawPesObject(&canvas, i, doc_scale);
                                } else {
                                    canvas.drawImageRect(img, dst, SkSamplingOptions());
                                }
                            }
                        }

                        if (type == pesData::OBJECT_TYPE_SCALABLE_SVG_FILE) {
                            SkPaint paint;
                            paint.setStyle(SkPaint::Style::kStroke_Style);
                            paint.setStrokeWidth(4);
                            paint.setColor(0x80FF0000);

                            int iPath = pesGetDocument()->getDataObject(i)->showPathBoundIndex;
                            if (iPath != -1) {
                                canvas.save();
                                pesRectangle rect = pesGetDocument()
                                                            ->getDataObject(i)
                                                            ->paths[iPath]
                                                            .getBoundingBox();

                                // scales, not only x-axis
                                auto parameter = pesGetDocument()->getDataObject(i)->parameter;
                                float pes_scale = std::min(abs(parameter.ppefScaleX), abs(parameter.ppefScaleY));

                                canvas.translate(-bound.getCenter().x, -bound.getCenter().y);
                                SkRect r = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
                                canvas.drawRect(r, paint);

                                if (pesGetDocument()->getDataObject(i)->paths[iPath].flags & 1) {
                                    paint.setStrokeWidth(1);

                                    // num path
                                    int num_pline = pesGetDocument()
                                                            ->getDataObject(i)
                                                            ->paths[iPath]
                                                            .getOutline()
                                                            .size();

                                    // buffer for rearrange vertiexes and normals
                                    std::vector<std::vector<pesVec2f>> nvertices;
                                    std::vector<std::vector<pesVec2f>> nnormals;

                                    // ---------------------------
                                    // Adjust distance of vertices
                                    // ---------------------------
                                    for (int pl = 0; pl < num_pline; pl++) {
                                        pesPolyline pline = pesGetDocument()
                                                                    ->getDataObject(i)
                                                                    ->paths[iPath]
                                                                    .getOutline()[pl];

                                        std::vector<pesVec2f> vertices = pline.getVertices();
                                        if (vertices.size() == 0) {
                                            continue;
                                        }

                                        std::vector<pesVec2f> nvts;
                                        makeEvenlyPolyline(vertices, nvts, _dotInterval);

                                        std::vector<pesVec2f> nnms;
                                        makeNomals(nvts, nnms);

                                        nvertices.push_back(nvts);
                                        nnormals.push_back(nnms);
                                    }

                                    // Process Stroke Analysis
                                    if (corners.size() == 0) {
                                        // paint.setAntiAlias( false );
                                        corners.clear();
                                        lines.clear();
                                        lines_flag.clear();
                                        angles.clear();
                                        angles_start.clear();
                                        angles_end.clear();

                                        for (int pl = 0; pl < nvertices.size(); pl++) {
                                            pesVec2f lastXY(999999, 999999);
                                            int cornerCount = 0;
                                            int angleCount = 0;
                                            pesVec2f cornerPts;

                                            std::vector<pesVec2f> line;
                                            lines.push_back(line);
                                            lines_flag.push_back(0);

                                            pesVec3f lastNM = nnormals[pl][0];

                                            for (int v = 0; v < nvertices[pl].size(); v++) {
                                                pesVec2f vt = nvertices[pl][v];
                                                pesVec3f nm = nnormals[pl][v];
                                                pesVec2f vtn((nm.x * (_strokeSize / 2)) + vt.x,
                                                             (nm.y * (_strokeSize / 2)) + vt.y);

                                                pesVec3f zc = nm;
                                                zc.cross(lastNM);

                                                // Curve In
                                                // if( vtn.distance( lastXY ) <= (1.2) ) {
                                                if (v != 0 &&
                                                    nm.angle(lastNM) > _cornerEntryAngleThreshol &&
                                                    zc.z > 0) {
                                                    // SkDebugf( "Angle: %0.2f\n", nm.angle(lastNM)
                                                    // );

                                                    // It's entering convex area - possible corner
                                                    cornerCount++;

                                                    // very steep angle is considered as corner
                                                    if (nm.angle(lastNM) >
                                                                _cornerSignificantAngleThreshold &&
                                                        cornerCount < _corner_threshold) {
                                                        cornerCount = _corner_threshold;
                                                    }

                                                    bool continuous_corner = false;
                                                    // prevent double corner in same place
                                                    if (corners.size() &&
                                                        vtn.distance(corners[corners.size() - 1]) <
                                                                _strokeSize) {
                                                        continuous_corner = true;
                                                    }

                                                    if (cornerCount >= _corner_threshold) {
                                                        // Start corner
                                                        if (cornerCount == _corner_threshold) {
                                                            cornerPts = vtn;  // cornerPts /
                                                                              // (cornerCount + 1);
                                                            int exc = getNearestCorner(
                                                                    cornerPts, _strokeSize / 2);
                                                            if (exc == -1) {
                                                                corners.push_back(
                                                                        cornerPts);  // duplicate
                                                                                     // corner
                                                            } else {
                                                                SkDebugf("Duplicate Corner\n");
                                                            }

                                                            lines_flag[lines_flag.size() - 1] =
                                                                    lines_flag[lines_flag.size() -
                                                                               1] |
                                                                    2;

                                                            // Start line next line from this
                                                            // cornerf
                                                            std::vector<pesVec2f> line;
                                                            line.push_back(cornerPts);

                                                            lines.push_back(line);
                                                            lines_flag.push_back(1);
                                                        }
                                                        // Start Corner - green
                                                        paint.setColor(0xFF00FF00);
                                                    } else {
                                                        // If in angle, end it
                                                        if (angleCount > 0) {
                                                            angles_end.push_back(lastXY);
                                                            angleCount = 0;
                                                        }
                                                        // else continue line until we consider
                                                        // corner
                                                        else {
                                                            lines[lines.size() - 1].push_back(vtn);
                                                        }
                                                        if (continuous_corner) {
                                                            paint.setColor(0xFF00FF00);
                                                        } else {
                                                            // Pre corner - cyan
                                                            paint.setColor(0xFF00FFFF);
                                                        }
                                                    }
                                                    lastNM = nm;
                                                } else {
                                                    bool nearCorner = false;

                                                    // prevent double corner in same place
                                                    if (corners.size() &&
                                                        vtn.distance(corners[corners.size() - 1]) <
                                                                10) {
                                                        nearCorner = true;
                                                    } else {
                                                        cornerPts.x = cornerPts.y = 0;
                                                        cornerCount = 0;
                                                    }

                                                    // Enter angle
                                                    if (!nearCorner && v != 0 &&
                                                        nm.angle(lastNM) >
                                                                _angleEntryAngleThreshol &&
                                                        zc.z < 0) {
                                                        // End line
                                                        std::vector<pesVec2f> line;
                                                        lines.push_back(line);
                                                        lines_flag.push_back(0);

                                                        // Curve out (Angle)
                                                        paint.setStrokeWidth(1);
                                                        paint.setColor(0xFFFF0000);
                                                        // canvas.drawLine( vtn.x, vtn.y, lastXY.x,
                                                        // lastXY.y, paint );
                                                        angleCount++;
                                                        if (angleCount == 1) {
                                                            angles.push_back(vt);
                                                            angles_start.push_back(lastXY);
                                                            paint.setColor(0xFFFF00FF);
                                                            // canvas.drawLine( vt.x, vt.y, vtn.x,
                                                            // vtn.y, paint );
                                                        }
                                                        paint.setColor(0xFFFF00FF);
                                                    } else {
                                                        // Line
                                                        paint.setColor(0xFF0000FF);
                                                        lines[lines.size() - 1].push_back(vtn);
                                                        if (angleCount > 0) {
                                                            angles_end.push_back(vtn);
                                                            angleCount = 0;
                                                        }
                                                    }
                                                    // Start line or curve out

                                                    lastXY = vtn;
                                                    lastNM = nm;
                                                }
                                                // Draw tiny lines
                                                paint.setStrokeWidth(1);
                                                if (_displayTINY) {
                                                    canvas.drawLine(
                                                            vt.x, vt.y, vtn.x, vtn.y, paint);
                                                }
                                                // Draw first point indicator
                                                if (pl == 0 && v == 0) {
                                                    paint.setColor(0xFFFF0000);
                                                    pesVec2f vtx((nm.x * -20 * doc_scale) + vt.x,
                                                                 (nm.y * -20 * doc_scale) + vt.y);
                                                    canvas.drawLine(
                                                            vt.x, vt.y, vtx.x, vtx.y, paint);
                                                }
                                            }
                                        }
                                    }

                                    // Adjust lines
                                    lines_id.clear();
                                    for (int ii = 0; ii < lines.size(); ii++) {
                                        if (lines[ii].size() < 2) {
                                            lines_flag[ii] = 16;
                                            continue;
                                        }
                                        int sindex = -1;
                                        int eindex = -1;

                                        if (_process_line3 && lines_flag[ii] == 3) {
                                            sindex =
                                                    getNearestCorner(lines[ii][0], _strokeSize / 2);
                                            eindex = getNearestCorner(
                                                    lines[ii][lines[ii].size() - 1],
                                                    _strokeSize / 2);
                                            if (sindex != -1 && eindex != -1) {
                                                lines_flag[ii] = 8;
                                            }
                                        }
                                        if (_process_line1 && lines_flag[ii] == 1) {
                                            sindex =
                                                    getNearestCorner(lines[ii][0], _strokeSize / 2);
                                            eindex = -1;
                                            int tindex = -1;
                                            tindex =
                                                    getTargetCorner(lines[ii][lines[ii].size() - 2],
                                                                    lines[ii][lines[ii].size() - 1],
                                                                    _strokeSize * 2);
                                            if (tindex != -1) {
                                                lines[ii][lines[ii].size() - 1] = corners[tindex];
                                                eindex = getNearestCorner(
                                                        lines[ii][lines[ii].size() - 1],
                                                        _strokeSize * 2);
                                            } else {
                                                // ปลายไม่มี corner รับ
                                                int cutIndex;
                                                tindex = getLineCut(lines[ii][lines[ii].size() - 2],
                                                                    lines[ii][lines[ii].size() - 1],
                                                                    _strokeSize * 2,
                                                                    15,
                                                                    ii,
                                                                    &cutIndex);
                                                if (tindex != -1) {
                                                    // cut the line
                                                    std::vector<pesVec2f> line1;
                                                    std::vector<pesVec2f> line2;

                                                    for (int v = 0; v < lines[tindex].size(); v++) {
                                                        if (v <= cutIndex) {
                                                            line1.push_back(lines[tindex][v]);
                                                        }
                                                        if (v >= cutIndex) {
                                                            line2.push_back(lines[tindex][v]);
                                                        }
                                                    }
                                                    lines.push_back(line1);
                                                    lines.push_back(line2);
                                                    int lf = lines_flag[tindex];
                                                    lines_flag.push_back(8);
                                                    lines_flag.push_back(8);
                                                    lines_flag[tindex] = 16;

                                                    pesVec2f c(lines[tindex][cutIndex]);
                                                    int ec = getNearestCorner(c, _strokeSize);
                                                    if (ec == -1) {
                                                        corners.push_back(c);
                                                        ec = corners.size() - 1;
                                                    }
                                                    eindex = ec;
                                                    lines[ii][lines[ii].size() - 1] = corners[ec];
                                                    lines_flag[ii] |= 2;
                                                }
                                            }
                                            if (sindex != -1 && eindex != -1) {
                                                lines_flag[ii] = 8;
                                            }
                                        }
                                        if (_process_line2 && lines_flag[ii] == 2) {
                                            sindex = -1;
                                            eindex = getNearestCorner(
                                                    lines[ii][lines[ii].size() - 1],
                                                    _strokeSize / 2);

                                            int tindex = -1;
                                            tindex = getTargetCorner(
                                                    lines[ii][1], lines[ii][0], _strokeSize * 2);
                                            if (tindex != -1) {
                                                lines[ii][0] = corners[tindex];
                                                sindex = getNearestCorner(lines[ii][0],
                                                                          _strokeSize * 2);
                                            } else {
                                                // ต้นไม่มี corner รับ
                                                int cutIndex;
                                                tindex = getLineCut(lines[ii][1],
                                                                    lines[ii][0],
                                                                    _strokeSize * 2,
                                                                    15,
                                                                    ii,
                                                                    &cutIndex);
                                                if (tindex != -1) {
                                                    // cut the line
                                                    std::vector<pesVec2f> line1;
                                                    std::vector<pesVec2f> line2;

                                                    for (int v = 0; v < lines[tindex].size(); v++) {
                                                        if (v <= cutIndex) {
                                                            line1.push_back(lines[tindex][v]);
                                                        }
                                                        if (v >= cutIndex) {
                                                            line2.push_back(lines[tindex][v]);
                                                        }
                                                    }
                                                    lines.push_back(line1);
                                                    lines.push_back(line2);
                                                    int lf = lines_flag[tindex];
                                                    lines_flag.push_back(8);
                                                    lines_flag.push_back(8);
                                                    lines_flag[tindex] = 16;

                                                    pesVec2f c(lines[tindex][cutIndex]);
                                                    int ec = getNearestCorner(c, _strokeSize);
                                                    if (ec == -1) {
                                                        corners.push_back(c);
                                                        ec = corners.size() - 1;
                                                    }
                                                    sindex = ec;
                                                    lines[ii][0] = corners[ec];
                                                    lines_flag[ii] |= 1;
                                                }
                                            }
                                            if (sindex != -1 && eindex != -1) {
                                                lines_flag[ii] = 8;
                                            }
                                        }

                                        if (_process_line0 && lines_flag[ii] == 0) {
                                            // sindex = getNearestCorner( lines[ii][0], _strokeSize
                                            // / 2 );
                                            int tindex = -1;

                                            pesVec2f v0 = lines[ii][0];
                                            for (int ll = 0; ll < lines.size(); ll++) {
                                                if (lines[ll].size() < lines[ii].size()) {
                                                    continue;
                                                }
                                                tindex =
                                                        getLineByPoint(v0, ll, _strokeSize / 2, ii);
                                                if (ii == 0) {
                                                    SkDebugf("0 in %i\n", tindex);
                                                }
                                                if (tindex == -1) continue;
                                                int v = 1;
                                                for (v = 1; v < lines[ii].size(); v++) {
                                                    pesVec2f vv = lines[ii][v];
                                                    int tx = getLineByPoint(
                                                            vv, ll, _strokeSize / 2, ii);
                                                    if (tx != tindex) {
                                                        break;
                                                    }
                                                }
                                                if (tindex != -1 && v == lines[ii].size()) {
                                                    if (ii == 2) {
                                                        SkDebugf("2 all in %i\n", tindex);
                                                    }

                                                    lines_flag[ii] = 16;
                                                    break;
                                                }
                                                ll = tindex;
                                            }
                                        }

                                        if (_process_line0 && lines_flag[ii] == 0) {
                                            sindex = -1;
                                            eindex = -1;

                                            int tindex = -1;

                                            tindex = getTargetCorner(lines[ii][1],
                                                                     lines[ii][0],
                                                                     _strokeSize * 2,
                                                                     20);

                                            if (tindex != -1) {
                                                lines[ii][0] = corners[tindex];
                                                sindex = tindex;
                                                lines_flag[ii] |= 1;
                                            } else {
                                                // ปลายไม่มี corner รับ
                                                int cutIndex;
                                                tindex = getLineCut(lines[ii][1],
                                                                    lines[ii][0],
                                                                    _strokeSize * 2,
                                                                    15,
                                                                    ii,
                                                                    &cutIndex);
                                                if (tindex != -1) {
                                                    // cut the line
                                                    std::vector<pesVec2f> line1;
                                                    std::vector<pesVec2f> line2;

                                                    for (int v = 0; v < lines[tindex].size(); v++) {
                                                        if (v <= cutIndex) {
                                                            line1.push_back(lines[tindex][v]);
                                                        }
                                                        if (v >= cutIndex) {
                                                            line2.push_back(lines[tindex][v]);
                                                        }
                                                    }
                                                    pesVec2f c(lines[tindex][cutIndex]);
                                                    int ec = getNearestCorner(c, _strokeSize);
                                                    if (ec == -1) {
                                                        corners.push_back(c);
                                                        ec = corners.size() - 1;
                                                    }
                                                    sindex = ec;
                                                    lines[ii][0] = corners[ec];
                                                    lines_flag[ii] |= 1;
                                                }
                                            }

                                            tindex =
                                                    getTargetCorner(lines[ii][lines[ii].size() - 2],
                                                                    lines[ii][lines[ii].size() - 1],
                                                                    _strokeSize * 2,
                                                                    15);
                                            if (tindex != -1) {
                                                lines[ii][lines[ii].size() - 1] = corners[tindex];
                                                eindex = tindex;
                                                lines_flag[ii] |= 2;
                                            } else {
                                                // ปลายไม่มี corner รับ
                                                int cutIndex;
                                                tindex = getLineCut(lines[ii][lines[ii].size() - 2],
                                                                    lines[ii][lines[ii].size() - 1],
                                                                    _strokeSize * 2,
                                                                    15,
                                                                    ii,
                                                                    &cutIndex);
                                                if (tindex != -1) {
                                                    // cut the line
                                                    std::vector<pesVec2f> line1;
                                                    std::vector<pesVec2f> line2;

                                                    for (int v = 0; v < lines[tindex].size(); v++) {
                                                        if (v <= cutIndex) {
                                                            line1.push_back(lines[tindex][v]);
                                                        }
                                                        if (v >= cutIndex) {
                                                            line2.push_back(lines[tindex][v]);
                                                        }
                                                    }
                                                    pesVec2f c(lines[tindex][cutIndex]);
                                                    int ec = getNearestCorner(c, _strokeSize);
                                                    if (ec == -1) {
                                                        corners.push_back(c);
                                                        ec = corners.size() - 1;
                                                    }
                                                    eindex = ec;
                                                    lines[ii][lines[ii].size() - 1] = corners[ec];
                                                    lines_flag[ii] |= 2;
                                                }
                                            }
                                            if (sindex != -1 && eindex != -1) {
                                                lines_flag[ii] = 8;
                                            }
                                        }
                                        if (_process_line8 && lines_flag[ii] == 8) {
                                            sindex =
                                                    getNearestCorner(lines[ii][0], _strokeSize / 2);
                                            if (sindex != -1) {
                                                lines[ii][0] = corners[sindex];
                                            }
                                            eindex = getNearestCorner(
                                                    lines[ii][lines[ii].size() - 1],
                                                    _strokeSize / 2);
                                            if (eindex != -1) {
                                                lines[ii][lines[ii].size() - 1] = corners[eindex];
                                            }
                                            for (int v = 0; v < lines[ii].size(); v++) {
                                                int tindex = getNearestCorner(lines[ii][v],
                                                                              _strokeSize / 4);
                                                if (tindex != -1 && tindex != sindex &&
                                                    tindex != eindex) {
                                                    std::vector<pesVec2f> line1;
                                                    std::vector<pesVec2f> line2;

                                                    for (int vv = 0; vv < lines[ii].size(); vv++) {
                                                        if (vv <= v) {
                                                            line1.push_back(lines[ii][vv]);
                                                        }
                                                        if (vv >= v) {
                                                            line2.push_back(lines[ii][vv]);
                                                        }
                                                    }
                                                    lines.push_back(line1);
                                                    sindex = getNearestCorner(line1[0],
                                                                              _strokeSize / 2);
                                                    eindex = getNearestCorner(
                                                            line1[line1.size() - 1],
                                                            _strokeSize / 2);
                                                    int f = 0;
                                                    if (sindex != -1) f |= 1;
                                                    if (eindex != -1) f |= 2;
                                                    lines_flag.push_back(f);

                                                    lines.push_back(line2);
                                                    sindex = getNearestCorner(line2[0],
                                                                              _strokeSize / 2);
                                                    eindex = getNearestCorner(
                                                            line2[line2.size() - 1],
                                                            _strokeSize / 2);
                                                    f = 0;
                                                    if (sindex != -1) f |= 1;
                                                    if (eindex != -1) f |= 2;
                                                    lines_flag.push_back(f);

                                                    lines_flag[ii] = 16;
                                                    sindex = -1;
                                                    eindex = -1;
                                                    break;
                                                }
                                            }
                                        }

                                        if (sindex == -1 || eindex == -1) {
                                            // continue;
                                        } else {
                                            int vid;
                                            if (sindex < eindex) {
                                                vid = (sindex << 16) + eindex;
                                            } else {
                                                vid = (eindex << 16) + sindex;
                                            }
                                            if (std::find(lines_id.begin(), lines_id.end(), vid) ==
                                                lines_id.end()) {
                                                lines_id.push_back(vid);
                                            } else {
                                                lines_flag[ii] = 16;
                                            }
                                        }
                                    }

                                    if (_displayDOT) {
                                        // Display Angles
                                        for (int ii = 0; ii < angles.size(); ii++) {
                                            paint.setStyle(SkPaint::Style::kFill_Style);
                                            paint.setColor(0xFF000000);
                                            canvas.drawCircle(angles[ii].x,
                                                              angles[ii].y,
                                                              3 * doc_scale,
                                                              paint);
                                            paint.setColor(0xFFFFFF00);
                                            canvas.drawCircle(angles[ii].x,
                                                              angles[ii].y,
                                                              2 * doc_scale,
                                                              paint);
                                            if (_angle_id_display) {
                                                char sz[64];
                                                sprintf(sz, "%i", ii);
                                                SkFont font;
                                                font.setSize(8);
                                                canvas.drawSimpleText(sz,
                                                                      strlen(sz),
                                                                      SkTextEncoding::kUTF8,
                                                                      angles[ii].x,
                                                                      angles[ii].y - 10,
                                                                      font,
                                                                      paint);
                                            }
                                        }

                                        // Display Angle Starts
                                        for (int ii = 0; ii < angles_start.size(); ii++) {
                                            paint.setStyle(SkPaint::Style::kFill_Style);
                                            paint.setColor(0xFF000000);
                                            canvas.drawCircle(angles_start[ii].x,
                                                              angles_start[ii].y,
                                                              3 * doc_scale,
                                                              paint);
                                            paint.setColor(0xFFFF8000);
                                            canvas.drawCircle(angles_start[ii].x,
                                                              angles_start[ii].y,
                                                              2 * doc_scale,
                                                              paint);
                                            // angle start id
                                            if (_angle_start_id_display) {
                                                char sz[64];
                                                sprintf(sz, "%i", ii);
                                                SkFont font;
                                                font.setSize(8);
                                                canvas.drawSimpleText(sz,
                                                                      strlen(sz),
                                                                      SkTextEncoding::kUTF8,
                                                                      angles_start[ii].x,
                                                                      angles_start[ii].y - 10,
                                                                      font,
                                                                      paint);
                                            }
                                        }

                                        // Display Angle Ends
                                        for (int ii = 0; ii < angles_end.size(); ii++) {
                                            paint.setStyle(SkPaint::Style::kFill_Style);
                                            paint.setColor(0xFF000000);
                                            canvas.drawCircle(angles_end[ii].x,
                                                              angles_end[ii].y,
                                                              3 * doc_scale,
                                                              paint);
                                            paint.setColor(0xFF0080FF);
                                            canvas.drawCircle(angles_end[ii].x,
                                                              angles_end[ii].y,
                                                              2 * doc_scale,
                                                              paint);

                                            // angle end id
                                            if (_angle_end_id_display) {
                                                char sz[64];
                                                sprintf(sz, "%i", ii);
                                                SkFont font;
                                                font.setSize(8);
                                                canvas.drawSimpleText(sz,
                                                                      strlen(sz),
                                                                      SkTextEncoding::kUTF8,
                                                                      angles_end[ii].x,
                                                                      angles_end[ii].y - 10,
                                                                      font,
                                                                      paint);
                                            }
                                        }

                                        // Display Corners
                                        for (int ii = 0; ii < corners.size(); ii++) {
                                            paint.setStyle(SkPaint::Style::kFill_Style);
                                            paint.setColor(0xFF000000);
                                            canvas.drawCircle(corners[ii].x,
                                                              corners[ii].y,
                                                              (4) * doc_scale,
                                                              paint);
                                            paint.setColor(0xFF80FF80);
                                            canvas.drawCircle(corners[ii].x,
                                                              corners[ii].y,
                                                              (3) * doc_scale,
                                                              paint);

                                            // corner id
                                            if (_corner_id_display) {
                                                char sz[64];
                                                sprintf(sz, "%i", ii);
                                                SkFont font;
                                                font.setSize(8);
                                                canvas.drawSimpleText(sz,
                                                                      strlen(sz),
                                                                      SkTextEncoding::kUTF8,
                                                                      corners[ii].x,
                                                                      corners[ii].y - 10,
                                                                      font,
                                                                      paint);
                                            }
                                        }

                                        // Display Lines
                                        for (int ii = 0; ii < lines.size(); ii++) {
                                            for (int v = 1; v < lines[ii].size(); v++) {
                                                if (lines_flag[ii] == 3) {
                                                    // cyan - corner to corner
                                                    paint.setColor(0xFF00FFFF);
                                                    if (!_display_line3) {
                                                        continue;
                                                    }
                                                } else if (lines_flag[ii] == 1) {
                                                    // magenta - corner to angle
                                                    paint.setColor(0xFFFF00FF);
                                                    if (!_display_line1) {
                                                        continue;
                                                    }
                                                } else if (lines_flag[ii] == 2) {
                                                    // yellow - angle to corner
                                                    paint.setColor(0xFFFFFF00);
                                                    if (!_display_line2) {
                                                        continue;
                                                    }
                                                } else if (lines_flag[ii] == 0) {
                                                    // red - angle to angle
                                                    paint.setColor(0xFFFF0000);
                                                    if (!_display_line0) {
                                                        continue;
                                                    }
                                                } else if (lines_flag[ii] == 8) {
                                                    paint.setColor(0xFFFFFFFF);
                                                    if (!_display_line8) {
                                                        continue;
                                                    }
                                                } else if (lines_flag[ii] == 16) {
                                                    paint.setColor(0xFF00FF00);
                                                    if (!_display_line16) {
                                                        continue;
                                                    }
                                                }
                                                paint.setStyle(SkPaint::Style::kStroke_Style);
                                                paint.setStrokeWidth(1);
                                                canvas.drawLine(lines[ii][v].x,
                                                                lines[ii][v].y,
                                                                lines[ii][v - 1].x,
                                                                lines[ii][v - 1].y,
                                                                paint);
                                            }

                                            // line id
                                            if (_lines_id_display) {
                                                char sz[64];
                                                sprintf(sz, "%i", ii);
                                                SkFont font;
                                                font.setSize(8);
                                                paint.setStyle(SkPaint::Style::kFill_Style);
                                                if (lines_flag[ii] == 8) {
                                                    paint.setColor(0xFFA0A0A0);
                                                }
                                                if ((lines_flag[ii] == 8 && _display_line8) ||
                                                    (lines_flag[ii] == 0 && _display_line0) ||
                                                    (lines_flag[ii] == 1 && _display_line1) ||
                                                    (lines_flag[ii] == 2 && _display_line2) ||
                                                    (lines_flag[ii] == 3 && _display_line3) ||
                                                    (lines_flag[ii] == 16 && _display_line16)) {
                                                    canvas.drawSimpleText(
                                                            sz,
                                                            strlen(sz),
                                                            SkTextEncoding::kUTF8,
                                                            lines[ii][lines[ii].size() / 4].x,
                                                            lines[ii][lines[ii].size() / 4].y - 2,
                                                            font,
                                                            paint);
                                                    // Display lines tips
                                                    if (lines[ii].size()) {
                                                        SkPaint paint2;
                                                        paint2.setStyle(
                                                                SkPaint::Style::kFill_Style);
                                                        paint2.setColor(0xFF000000);
                                                        paint2.setAntiAlias(true);
                                                        canvas.drawCircle(lines[ii][0].x,
                                                                          lines[ii][0].y,
                                                                          1,
                                                                          paint2);
                                                        canvas.drawCircle(
                                                                lines[ii][lines[ii].size() - 1].x,
                                                                lines[ii][lines[ii].size() - 1].y,
                                                                1,
                                                                paint2);
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (pesGetDocument()->getDataObject(i)->paths[iPath].flags &
                                        2) {
                                        lines_id.clear();

                                        pesPath* path = new pesPath();
                                        path->setFilled(false);
                                        path->setFillColor(pesColor(0, 0, 0, 0));
                                        path->setStrokeColor(pesGetDocument()
                                                                     ->getDataObject(i)
                                                                     ->paths[iPath]
                                                                     .getFillColor());
                                        path->setStrokeWidth(_strokeSize / pes_scale);
                                        SkDebugf("Save Strokd: %0.2f, %0.2f\n",
                                                 _strokeSize,
                                                 pes_scale);

                                        pesVec2f lastVx(99999, 99999);

                                        for (int ii = 0; ii < lines.size(); ii++) {
                                            if (lines[ii].size() > 0) {
                                                if (lines_flag[ii] == 8) {
                                                    pesVec2f vx0(lines[ii][0]);
                                                    if (vx0.x != lastVx.x || vx0.y != lastVx.y) {
                                                        path->moveTo(vx0.x, vx0.y);
                                                    } else {
                                                        SkDebugf("Skip MoveTo\n");
                                                    }
                                                    int vcount = 0;
                                                    for (int v = 1; v < lines[ii].size(); v++) {
                                                        pesVec2f vx(lines[ii][v].x, lines[ii][v].y);
                                                        if (vcount == 0 && vx.distance(vx0) < 6)
                                                            continue;
                                                        if (vx.distance(vx0) < 6) {
                                                            continue;
                                                        }

                                                        path->lineTo(vx.x, vx.y);
                                                        vcount++;
                                                        vx0 = vx;
                                                    }
                                                    lastVx = lines[ii][lines[ii].size() - 1];
                                                }
                                            }
                                        }
                                        pesGetDocument()->getDataObject(i)->paths.push_back(*path);
                                        PES5_UpdatePesImage(i);

                                        pesGetDocument()->getDataObject(i)->paths[iPath].flags &= ~2;
                                        PES5_UpdatePesImage(i);
                                        SkDebugf("Done\n");
                                    }
                                } else {
                                    pesPath pp = pesGetDocument()->getDataObject(i)->paths[iPath];
                                    SkPath skp = toSk(pp);
                                    paint.setStrokeWidth(abs(pp.getStrokeWidth()) * pes_scale);
                                    canvas.drawPath(skp, paint);
                                }

                                canvas.restore();
                            }
                        }
                    }
                }
                canvas.restore();

                // draw border
                if (i == selectedIndex || PES5_MSIsSelected(i)) {
                    canvas.save();
                    {
                        const float rs = 15 / doc_scale;
                        const float rotateArmLength = 30 / doc_scale;
                        const float borderlinethick = 4.0f / scalelinethick;
                        const float detaillinethick = 1.0f / scalelinethick;
                        constexpr auto darksmoke  = SkColorSetARGB(0xff, 0x18, 0x18, 0x18);
                        constexpr auto selectblue = SkColorSetARGB(0xff, 0x32, 0x74, 0xEE);

                        SkPaint pborder;
                        pborder.setStrokeWidth(borderlinethick);
                        pborder.setStroke(true);
                        pborder.setAntiAlias(true);
                        pborder.setColor(darksmoke);
                        pborder.setBlendMode(SkBlendMode::kDifference);

                        SkPaint pdetail;
                        pdetail.setStrokeWidth(detaillinethick);
                        pdetail.setStroke(true);
                        pdetail.setAntiAlias(true);
                        pdetail.setColor(selectblue);
                        pdetail.setBlendMode(SkBlendMode::kSrcOver);

                        if (actionDesc.action == "scale" || 
                            actionDesc.action == "scaleX" ||
                            actionDesc.action == "scaleY") {
                            if (isSingleSelect) {
                                bound.width += actionDesc.dw;
                                bound.height += actionDesc.dh;
                                canvas.translate(-actionDesc.dw / 2, -actionDesc.dh / 2);
                            }
                            else {
                                const float sx = actionDesc.sx;
                                const float sy = actionDesc.sy;

                                const pesVec2f groupcxy = groupbound.getCenter();
                                const auto localcx = pes->centerx - groupcxy.x;
                                const auto localcy = pes->centery - groupcxy.y;
                                canvas.translate(localcx * sx - localcx, localcy * sx - localcy);

                                float w = bound.width;
                                float h = bound.height;
                                
                                bound.width = w * sx;
                                bound.height = h * sy;
                                canvas.translate(-(bound.width - w) / 2, -(bound.height - h) / 2);
                            }
                        }

                        auto cxy = bound.getCenter();
                        canvas.translate(cxy.x, cxy.y);

                        if (type != pesData::OBJECT_TYPE_BACKGROUND) {
                            if (actionDesc.action == "drag") {
                                canvas.translate(actionDesc.tx, actionDesc.ty);
                            }
                            float rotateDegree = pesGetDocument()->getDataObject(i)->parameter.rotateDegree;
                            if (actionDesc.action == "rotate") {
                                if (isSingleSelect) {
                                    canvas.rotate(actionDesc.degree);
                                } else {
                                    // TODO
                                    const pesVec2f pivot = selectedLocalBound.getCenter();
                                    const auto x0 = pivot.x;
                                    const auto y0 = pivot.y;

                                    // degree to radius
                                    const auto rad = actionDesc.degree * (float)DEG_TO_RAD;

                                    // cos, sin theta
                                    const auto cosrad  = cosf(rad);
                                    const auto sinrad  = sinf(rad);

                                    cxy = bound.getCenter();

                                    const auto cxrot = x0 + (cxy.x - x0) * cosrad - (cxy.y - y0) * sinrad;
                                    const auto cyrot = y0 + (cxy.x - x0) * sinrad + (cxy.y - y0) * cosrad;
                                    canvas.translate(cxrot - cxy.x, cyrot - cxy.y);
                                    canvas.rotate(rotateDegree + actionDesc.degree);
                                }
                            } else {
                                if (rotateDegree != 0.0) {
                                    canvas.rotate(rotateDegree);
                                }
                            }
                        }

                        const float bw = bound.width;
                        const float bh = bound.height;
                        const float hbw = SkScalarHalf(bw);
                        const float hbh = SkScalarHalf(bh);

                        canvas.drawRect(makeRectFromCenter(0, 0, bw, bh), pborder);
                        if (isSingleSelect) {
                            canvas.drawLine(0, -hbh, 0, -hbh - rotateArmLength, pborder);
                        }
                        canvas.drawRect(makeRectFromCenter(0, 0, bw, bh), pdetail);
                        if (isSingleSelect) {
                            canvas.drawLine(0, -hbh, 0, -hbh - rotateArmLength, pdetail);
                        }

                        // change paint-border thickness
                        pborder.setStrokeWidth(detaillinethick);
                        // change paint-detail to fill-mode
                        pdetail.setStroke(false);
                        if (isSingleSelect && PES5_MSIsScalable()) {
                            canvas.drawRect(makeRectFromCenter(hbw, 0, rs, rs), pdetail);
                            canvas.drawRect(makeRectFromCenter(0, hbh, rs, rs), pdetail);
                            canvas.drawRect(makeRectFromCenter(hbw, hbh, rs, rs), pdetail);
                            canvas.drawRect(makeRectFromCenter(hbw, 0, rs, rs), pborder);
                            canvas.drawRect(makeRectFromCenter(0, hbh, rs, rs), pborder);
                            canvas.drawRect(makeRectFromCenter(hbw, hbh, rs, rs), pborder);
                        }

                        if (isSingleSelect) {
                            canvas.drawRect(makeRectFromCenter(0, -hbh - rotateArmLength, rs, rs), pdetail);
                            canvas.drawRect(makeRectFromCenter(0, -hbh - rotateArmLength, rs, rs), pborder);
                        }
                    }
                    canvas.restore();
                }
            }
            
            // draw border group select
            if (isMultiSelect) {
                canvas.save();
                //auto bound = PES5_MSGetBoundingBox();
                auto bound = groupbound;

                const float rs = 15 / doc_scale;
                const float rotateArmLength = 30 / doc_scale;
                const float borderlinethick = 4.0f / scalelinethick;
                const float detaillinethick = 1.0f / scalelinethick;
                const float crosslinethick = 1.5f / scalelinethick;
                const float radius = 4.0f / doc_scale;
                constexpr auto darksmoke = SkColorSetARGB(0xff, 0x18, 0x18, 0x18);
                constexpr auto selectRed = SkColorSetARGB(0xff, 0xEE, 0x74, 0x32);

                SkPaint pborder;
                pborder.setStrokeWidth(borderlinethick);
                pborder.setStroke(true);
                pborder.setAntiAlias(true);
                pborder.setColor(darksmoke);
                pborder.setBlendMode(SkBlendMode::kDifference);

                SkPaint pdetail;
                pdetail.setStrokeWidth(detaillinethick);
                pdetail.setStroke(true);
                pdetail.setAntiAlias(true);
                pdetail.setColor(selectRed);
                pdetail.setBlendMode(SkBlendMode::kSrcOver);

                if (actionDesc.action == "scale" || actionDesc.action == "scaleX" ||
                    actionDesc.action == "scaleY") {
                    groupbound.width += actionDesc.dw;
                    groupbound.height += actionDesc.dh;
                    canvas.translate(-actionDesc.dw / 2, -actionDesc.dh / 2);
                }
                auto cxy = groupbound.getCenter();
                canvas.translate(cxy.x, cxy.y);
                if (actionDesc.action == "drag") {
                    canvas.translate(actionDesc.tx, actionDesc.ty);
                }
                if (actionDesc.action == "rotate") {
                    canvas.rotate(actionDesc.degree);
                }

                const float bw = groupbound.width + 8;
                const float bh = groupbound.height+ 8;
                const float hbw = SkScalarHalf(bw);
                const float hbh = SkScalarHalf(bh);

                SkRect rectoutset = makeRectFromCenter(0, 0, bw, bh);
                canvas.drawRect(rectoutset, pborder);
                canvas.drawLine(0, -hbh, 0, -hbh - rotateArmLength, pborder);
                canvas.drawRect(rectoutset, pdetail);
                canvas.drawLine(0, -hbh, 0, -hbh - rotateArmLength, pdetail);

                // change paint-border thickness
                pborder.setStrokeWidth(detaillinethick);
                // change paint-detail to fill-mode
                pdetail.setStroke(false);

                if (PES5_MSIsScalable()) {
                    //canvas.drawRect(makeRectFromCenter(hbw, 0, rs, rs), pdetail);
                    //canvas.drawRect(makeRectFromCenter(0, hbh, rs, rs), pdetail);
                    //canvas.drawCircle(0, bh * 0.5f, radius, pdetail);
                    //canvas.drawCircle(bw * 0.5f, 0, radius, pdetail);
                    canvas.drawRect(makeRectFromCenter(hbw, hbh, rs, rs), pdetail);
                    //canvas.drawRect(makeRectFromCenter(hbw, 0, rs, rs), pborder);
                    //canvas.drawRect(makeRectFromCenter(0, hbh, rs, rs), pborder);
                    //canvas.drawCircle(0, bh * 0.5f, radius, pborder);
                    //canvas.drawCircle(bw * 0.5f, 0, radius, pborder);
                    canvas.drawRect(makeRectFromCenter(hbw, hbh, rs, rs), pborder);
                }

                canvas.drawRect(makeRectFromCenter(0, -hbh - rotateArmLength, rs, rs), pdetail);
                canvas.drawRect(makeRectFromCenter(0, -hbh - rotateArmLength, rs, rs), pborder);

                pdetail.setStrokeWidth(crosslinethick);
                pdetail.setBlendMode(SkBlendMode::kDifference);
                canvas.drawLine(0, -rs, 0, rs, pdetail);
                canvas.drawLine(-rs, 0, rs, 0, pdetail);
                canvas.drawCircle(0, 0, radius, pdetail);

                canvas.restore();
            }
        };

        if (PES5_IsStitchSimulatorMode()) {
            auto simulator = PesStitchSimulator::getInstance();
            if (simulator && simulator->isRunning()) {
                // draw Stitch Simulator
                unsigned int totalStitches = 0;
                for (int i = 0; i < cnt; i++) {
                    totalStitches += pesGetDocument()->getDataObject(i)->getTotalStitches();
                }
                int num = (int)simulator->currentStitchNum;
                if (num > totalStitches) {
                    num = totalStitches;
                }
                int count = 0;
                SkPoint last = SkPoint::Make(0, 0);
                for (int i = 0; i < cnt; i++) {
                    auto& blocks = PES5_GetStitchSimulatorData()[i];
                    for (int j = 0; j < blocks.size(); j++) {
                        pesColor fc = blocks[j].color;
                        std::vector<pesVec2f> points = blocks[j].polyline.getVertices();
                        size_t numPoint = 0;
                        for (int k = 1; k < (int)points.size(); k++) {
                            if (++count < num) {
                                last.set(points[k].x, points[k].y);
                                if (k == 1) {
                                    numPoint++;
                                }
                                numPoint++;
                            } else {
                                break;
                            }
                        }
                        // Draw Simulator
                        SkPaint paint;
                        paint.setStyle(SkPaint::kStroke_Style);

                        paint.setAntiAlias(true);

                        // draw lines
                        paint.setStrokeWidth(3);
                        paint.setARGB(0xff, fc.r, fc.g, fc.b);
                        canvas.drawPoints(SkCanvas::kPolygon_PointMode,
                                          numPoint,
                                          (SkPoint*)points.data(),
                                          paint);
                        paint.setStrokeWidth(2);
                        paint.setARGB(0x80, 0, 0, 0);
                        canvas.drawPoints(SkCanvas::kPoints_PointMode,
                                          numPoint,
                                          (SkPoint*)points.data(),
                                          paint);
                    }

                    if (count > num) {
                        break;
                    }
                }

                // draw last point(current needle)
                SkPaint paint;
                paint.setStyle(SkPaint::kFill_Style);
                paint.setAntiAlias(true);
                paint.setColor(0xffff0000);
                canvas.drawCircle(last, 4.0, paint);

                if (simulator->currentStitchNum > totalStitches) {
                    simulator->stop();
                }
            } 
            else {
                // draw only object has stitches
                for (int i = 0; i < cnt; i++) {
                    const auto pesdataobj = pesGetDocument()->getDataObject(i);
                    if (pesdataobj->parameter.visible == false) {
                        continue;
                    }

                    if (pesdataobj->parameter.type == pesData::ObjectType::OBJECT_TYPE_BACKGROUND) {
                        continue;
                    }

                    if (pesdataobj->getTotalStitches()) {
                        pesRectangle bound(pesdataobj->getBoundingBox());
                        canvas.save();
                        canvas.translate(bound.getCenter().x, bound.getCenter().y);
                        pesGetDocument()->drawPesObject(&canvas, i, doc_scale);
                        canvas.restore();
                    }
                }
            }
        } 
        else {
            drawObjects(canvas);
        }

        if (PES5_IsSatinColumnInputStarted()) {
            PES5_DrawSatinColumnInput(canvas);
        }
        else if (PES5_IsDrawingInputStarted()) {
            PES5_DrawingInputDraw(canvas);
        }
        else if (PES5_IsLinesToolInputStarted()){
            PES5_LinesToolDraw(canvas);
        }
        else if (PES5_IsPolygonToolInputStarted()){
            PES5_PolygonToolDraw(canvas);
        }
        else if (PES5_IsPathEditInputStarted()){
            SkPaint paint;
            if (this->isDarkMode) {
                paint.setColor(0x60404040);
            } else {
                paint.setColor(0xe0000000);
            }
            paint.setStyle(SkPaint::kFill_Style);
            SkRect rect;
            rect.setXYWH(0, 0, _width, _height);
            canvas.save();
            {
                canvas.scale(1/doc_scale, 1/doc_scale);
                canvas.translate(-(_width / 2) - pan.x, -(_height / 2) - pan.y);
                canvas.drawRect(rect, paint);
            }
            canvas.restore();
            PES5_PathEditDraw(canvas);
        }
        else if (PES5_IsStitchEditInputStarted()){
            SkPaint paint;
            if (this->isDarkMode) {
                paint.setColor(0x60404040);
            } else {
                paint.setColor(0xe0000000);
            }
            paint.setStyle(SkPaint::kFill_Style);
            SkRect rect;
            rect.setXYWH(0, 0, _width, _height);
            canvas.save();
            {
                canvas.scale(1/doc_scale, 1/doc_scale);
                canvas.translate(-(_width / 2) - pan.x, -(_height / 2) - pan.y);
                canvas.drawRect(rect, paint);
            }
            canvas.restore();
            PES5_StitchEditDraw(canvas);
        }

        drawGrid(canvas, bDrawGrid);
    }
    canvas.restore();

    // draw others not zoom-scale

#ifndef UNIT_2_MM
#define UNIT_2_MM(unit) (float(unit) / 10.0f)
#endif

    const auto aboveThumbSize = 60.0f;

    auto drawTooltip = [=](SkCanvas& canvas, std::string text, float x, float y) -> void {
        if (_font == NULL) {
            return;
        }
        canvas.save();
        canvas.scale(1, 1);

        SkFont font;
        font.setTypeface(_font->typeface(FONT_REGULAR));
        font.setSubpixel(true);
        font.setSize(16);

        SkFontMetrics metrics;
        font.getMetrics(&metrics);

        SkRect textBound;
        const float textW = font.measureText(text.c_str(), text.length(), SkTextEncoding::kUTF8, &textBound);
        const float textH = metrics.fDescent - metrics.fAscent;
        const float textHW = textW / 2;
        const float textHH = textH / 2;

        canvas.translate(x, y);

        SkPaint paint;
        paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
        paint.setAntiAlias(true);
        if (this->isDarkMode) {
            paint.setARGB(0xff, 0xf5, 0xf5, 0xf5);
        } else {
            paint.setARGB(0xff, 0x24, 0x24, 0x24);
        }

        SkRect rect = SkRect::MakeXYWH((0 - textHW - 8), (0 - textHH - 2), textW + 16, textH + 4);
        canvas.drawRoundRect(rect, 5, 5, paint);

        if (this->isDarkMode) {
            paint.setARGB(0xff, 0x24, 0x24, 0x24);
        } else {
            paint.setARGB(0xff, 0xf5, 0xf5, 0xf5);
        }
        canvas.drawSimpleText(text.c_str(),
                              text.length(),
                              SkTextEncoding::kUTF8,
                              (0 - textHW),
                              (0 - (metrics.fAscent + textHH)),
                              font,
                              paint);
        canvas.restore();
    };
    
    auto drawMode = [=](SkCanvas& canvas, std::string text, float x, float y) -> void {
        if (_font == NULL) {
            return;
        }
        canvas.save();
        canvas.scale(1, 1);

        SkFont font;
        font.setTypeface(_font->typeface(FONT_REGULAR));
        font.setSubpixel(true);
        font.setSize(16);

        SkFontMetrics metrics;
        font.getMetrics(&metrics);

        SkRect textBound;
        const float textW = font.measureText(text.c_str(), text.length(), SkTextEncoding::kUTF8, &textBound);
        const float textH = metrics.fDescent - metrics.fAscent;
        const float textHW = textW / 2;
        const float textHH = textH / 2;

        canvas.translate(x, y);

        SkPaint paint;
        paint.setStyle(SkPaint::Style::kStrokeAndFill_Style);
        paint.setAntiAlias(true);
        if (this->isDarkMode) {
            paint.setARGB(0xff, 0xf5, 0xf5, 0xf5);
        } else {
            paint.setARGB(0xff, 0x24, 0x24, 0x24);
        }

        SkRect rect = SkRect::MakeXYWH((0 - 8), (0 - 2), textW + 16, textH + 4);
        canvas.drawRoundRect(rect, 5, 5, paint);
        __modeRect = rect;
        __modeRect.offset(x, y);

        if (this->isDarkMode) {
            paint.setARGB(0xff, 0x24, 0x24, 0x24);
        } else {
            paint.setARGB(0xff, 0xf5, 0xf5, 0xf5);
        }
        canvas.drawSimpleText(text.c_str(),
                              text.length(),
                              SkTextEncoding::kUTF8,
                              (0),
                              (0 - (metrics.fAscent)),
                              font,
                              paint);
        canvas.restore();
    };
    
    
    canvas.save();
    {
        const int selectCount = PES5_MSSelectCount();
        const bool hasSelect = selectCount > 0;
        const bool isSingleSelect = selectedIndex > -1;
        const bool isMultiSelect = selectCount > 1;

        char buff[200];
        buff[0] = 0;

        if (isSingleSelect || isMultiSelect) {
            pesRectangle bound;
            pesData::ObjectType type = pesData::OBJECT_TYPE_SCALABLE_CONTAINER;

            if (isSingleSelect) {
                int i = selectedIndex;
                bound = pesGetDocument()->getDataObject(i)->getBoundingBox();
                type = pesGetDocument()->getDataObject(i)->parameter.type;
            } else {
                bound = PES5_MSGetBoundingBox();
            }

            if (actionDesc.action == "drag") {
                if (type != pesData::OBJECT_TYPE_BACKGROUND) {
                    const auto c = bound.getCenter();
                    const auto cx = UNIT_2_MM(actionDesc.tx + c.x);
                    const auto cy = UNIT_2_MM(actionDesc.ty + c.y);
                    sprintf(buff, "Cx:%.02f  Cy:%.02f", cx, cy);
                }
            } else if (actionDesc.action == "rotate") {
                if (type != pesData::OBJECT_TYPE_BACKGROUND) {
                    sprintf(buff, "%.02f°", actionDesc.degree);
                }
            } else if (actionDesc.action != "undefined") {
                const auto dw = UNIT_2_MM(actionDesc.dw + bound.width);
                const auto dh = UNIT_2_MM(actionDesc.dh + bound.height);
                sprintf(buff, "%.02f x %.02f mm.", dw, dh);
            }

            // show tooltip
            if (buff[0] != 0) {
                drawTooltip(canvas, buff, actionDesc.mx, actionDesc.my - aboveThumbSize);
            }
        }
    }
    canvas.restore();

#ifdef __EMSCRIPTEN__
    if (!(PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_RULER)) {
#endif
        PES5_SetHasRulur(grulerw / 2);
        drawRuler(canvas, PES5_IsPathEditInputStarted() ? !this->isDarkMode : this->isDarkMode);
#ifdef __EMSCRIPTEN__
    } else {
        PES5_SetHasRulur(0);
    }
#endif

    canvas.save();
    {
        // Draw mode
        const int mx = 10;
        const int my = 5;
        if (PES5_IsSatinColumnInputStarted()) {
            drawMode(canvas, "Satin Column", mx, my);
        }
        else if (PES5_IsDrawingInputStarted()) {
            drawMode(canvas, "Drawing", mx, my);
        }
        else if (PES5_IsLinesToolInputStarted()) {
            drawMode(canvas, "Line", mx, my);
        }
        else if (PES5_IsPolygonToolInputStarted()) {
            drawMode(canvas, "Polygon", mx, my);
        }
        else if (PES5_IsPathEditInputStarted()) {
            drawMode(canvas, "Path Edit", mx, my);
        }
        else if (PES5_IsStitchEditInputStarted()) {
            drawMode(canvas, "Stitch Edit", mx, my);
        }
        else if (PES5_IsStitchSimulatorMode()) {
            drawMode(canvas, "Stitch Simulator", mx, my);
        }
        else {
            __modeRect.setLTRB(0, 0, 0, 0);
        }
    }
    canvas.restore();
}

int   __counter = 0;
int   __w = -1;
int   __h = -1;
float __zs = 1;
float __px = 0;
float __py = 0;
void Pes5DocumentView::update() {
    bool isDirty = false;
#ifdef __EMSCRIPTEN__
    isDirty = (bool)EM_ASM_INT({ return globalThis['isDirty'] ? 1 : 0; });
#endif
    
    const auto zs = PES5_GetZoomScale();
    const auto pan = PES5_GetPan();
    if (__w != _width || __h != _height) {
        __w = _width;
        __h = _height;
        isDirty = true;
    }
    if (__px != pan.x || __py != pan.y) {
        __px = pan.x;
        __py = pan.y;
        isDirty = true;
    }
    if (__zs != zs) {
        __zs = zs;
        isDirty = true;
    }

    if (PES5_IsStitchSimulatorMode()) {
        auto simulator = PesStitchSimulator::getInstance();
        if (simulator) {
            static int onum = 0;
            if (simulator->bStop == false && simulator->bPause == false) {
                simulator->update();
                int num = (int)simulator->currentStitchNum;
                if (onum != num) {
                    onum = num;
                    isDirty = true;
                }
            } else {
                if (simulator->bStop) {
                    onum = 0;
                }
            }
        } else {
            PES5_StopStitchSimulatorMode();
            isDirty = true;
        }
    }

    if (isDirty) {
        dirty(true, false);
        __counter = 1;
    } else {
        if ((__counter % 60) == 0) {
            dirty(true, false);
            __counter = 1;
        } else {
            __counter++;
        }
    }
}

void Pes5DocumentView::setSelectedIndex(int index, bool multiselect) {
    if (index < -1 || index >= PES5_GetObjectCount()) {
        index = -1;
    }

    if (selectedIndex != index) {
        dirty(true, false);
        selectedIndex = index;
        //GetLayerList()->select_listItem(index);
        //PES5_SetObjectSelectedIndex(index);
    }
}

bool __start_pan = false;
int __prevMouseX = -1;
int __prevMouseY = -1;
bool Pes5DocumentView::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ){
    dirty(true, false);

    const uint8_t u8mod = (uint8_t)modifiers;
    bool isControlKey = (bool)(u8mod & (uint8_t)skui::ModifierKey::kControl);
    bool isShiftKey   = (bool)(u8mod & (uint8_t)skui::ModifierKey::kShift);
    
    //DecWidget::onMouse(ix, iy, state, modifiers);

    pesVec2f canvasOrigin(_width/2, _height/2);
    if( state == skui::InputState::kDown ) {
        if ((bool)(u8mod & (uint8_t)skui::ModifierKey::kControl) && !isShiftKey) {
            //SkDebugf("kDown + kControl\n");
            mouseCapture(true);
            //pressed = true;
            __start_pan = true;
            //setCursorByID("canvas", "grabbing", true);
            __prevMouseX = ix;
            __prevMouseY = iy;
            return false;
        }
        
        if (PES5_IsSatinColumnInputStarted() || 
            PES5_IsDrawingInputStarted() ||
            PES5_IsLinesToolInputStarted() || 
            PES5_IsPolygonToolInputStarted() ||
            PES5_IsPathEditInputStarted() || 
            PES5_IsStitchEditInputStarted() ||
            PES5_IsStitchSimulatorMode()) {
            // SkDebugf("x=%i, y=%i\n", ix, iy);
            //pesRectangle r(5, 5, 80, 25); // TODO: Calc Rectangle
            //if(r.inside(ix, iy)){
            if (__modeRect.fLeft <= ix && ix <= __modeRect.fRight && 
                __modeRect.fTop <= iy && iy <= __modeRect.fBottom ) {
                setCursorByID("canvas", "");
                PES5_StopSatinColumnInput();
                PES5_StopDrawingInput();
                PES5_StopLinesToolInput();
                PES5_StopPolygonToolInput();
                PES5_StopPathEditInput();
                PES5_StopStitchEditInput();
                PES5_StopStitchSimulatorMode();
                return true;
            }
        }

        if(PES5_IsSatinColumnInputStarted()){
//            pesVec2f canvasOrigin(_width/2, _height/2);
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            bool shiftPressed = (modifiers==skui::ModifierKey::kShift) || PES5_IsShiftPressed();
            if(shiftPressed){
                PES5_SatinColumnAddCurvePoint(p.x, p.y);
            }
            else{
                PES5_SatinColumnAddCornerPoint(p.x, p.y);
            }
            return false;
        }
        else if(PES5_IsDrawingInputStarted()){
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            PES5_DrawingInputAddFirstPoint(p);
            pressed = true;
            return false;
        }
        else if(PES5_IsLinesToolInputStarted()){
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            bool shiftPressed = (modifiers==skui::ModifierKey::kShift) || PES5_IsShiftPressed();
            if(shiftPressed){
                PES5_LinesToolAddCurvePoint(p.x, p.y);
            }
            else{
                PES5_LinesToolAddCornerPoint(p.x, p.y);
            }
            return false;
        }
        else if(PES5_IsPolygonToolInputStarted()){
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            bool shiftPressed = (modifiers==skui::ModifierKey::kShift) || PES5_IsShiftPressed();
            if(shiftPressed){
                PES5_PolygonToolAddCurvePoint(p.x, p.y);
            }
            else{
                PES5_PolygonToolAddCornerPoint(p.x, p.y);
            }
            return false;
        }
        //else if(PES5_IsStitchSimulatorMode()) {
        //    // ...
        //}


        
        mouseCapture( true );
        pressed = true;
        mousePressed(ix, iy, modifiers);
    }
    else if( state == skui::InputState::kMove ) {
        if(pressed && PES5_IsDrawingInputStarted()){
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            PES5_DrawingInputAddPoint(p);
            return false;
        }
        
        if (__start_pan || pressed) {
            mouseDragged(ix, iy);
        }
        else{
            if (PES5_IsSatinColumnInputStarted() || 
                PES5_IsDrawingInputStarted() ||
                PES5_IsLinesToolInputStarted() || 
                PES5_IsPolygonToolInputStarted() ||
                PES5_IsPathEditInputStarted() || 
                PES5_IsStitchEditInputStarted() ||
                PES5_IsStitchSimulatorMode()) {
                if (__modeRect.fLeft <= ix && ix <= __modeRect.fRight && 
                    __modeRect.fTop <= iy && iy <= __modeRect.fBottom ) {
                    // force cursor type is "pointer", 
                    // other setting after this will be backup to cursor-data
                    setCursorByID("canvas", "pointer", true);
                } else {
                    // clear force cursor and restore cursor type from cursor-data
                    setCursorByID("canvas", "");
                }
            }

            mouseMoved(ix, iy);
        }
    }
    else if( state == skui::InputState::kUp ) {
//        SkDebugf( "MouseUp %d, %d mod=%d\n", ix, iy, modifiers);
        if(pressed && PES5_IsDrawingInputStarted()){
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin);
            PES5_DrawingInputAddLastPoint(p);
            pressed = false;
            return false;
        }

        if (__start_pan || pressed) {
            bool panmode = __start_pan;
            mouseCapture( false );
            pressed = false;
            mouseReleased(ix, iy);
            //if (panmode) {
            //    if ((bool)(u8mod & (uint8_t)skui::ModifierKey::kControl)) {
            //        // force cursor type is "grab"
            //        setCursorByID("canvas", "grab", true);
            //    } else {
            //        // clear force cursor and restore cursor type from cursor-data
            //        setCursorByID("canvas", "");
            //    }
            //}
            mouseMoved(ix, iy);
        }
    }
    
    return true;
}

#ifdef __EMSCRIPTEN__
bool __isMac = []() {
    return (bool)EM_ASM_INT({ return navigator.userAgent.includes('Mac OS X') ? 1 : 0; });
}();
#else
// native
#if defined(SK_BUILD_FOR_MAC)
bool __isMac = true;
#else
bool __isMac = false;
#endif  //SK_BUILD_FOR_MAC
#endif  //__EMSCRIPTEN__
uint8_t __u8platformCtrlKey = __isMac ? (uint8_t)skui::ModifierKey::kCommand : (uint8_t)skui::ModifierKey::kControl;

void Pes5DocumentView::mousePressed(int ix, int iy, skui::ModifierKey modifiers){
    uint8_t u8mod = (uint8_t)modifiers;
    uint8_t u8modkeys = (u8mod & ~(uint8_t)skui::ModifierKey::kFirstPress);
    const bool isPlatformCtrlKey = (bool)(u8modkeys & __u8platformCtrlKey);
    const bool isShiftKey = (bool)(u8modkeys & (uint8_t)skui::ModifierKey::kShift);
    const bool isShiftKeyOnly = u8modkeys == (uint8_t)skui::ModifierKey::kShift;

    //SkDebugf("%d %d\n", u8modkeys, isShiftKeyOnly);
    
    if(PES5_IsPathEditInputStarted()){
        if(PES5_PathEditMousePressed(ix, iy)){
            return;
        }
        else{
            __start_pan = true;
            __prevMouseX = ix;
            __prevMouseY = iy;
            return;
        }
    }
    else if(PES5_IsStitchEditInputStarted()){
        if(PES5_StitchEditMousePressed(ix, iy)){
            return;
        }
        else{
            __start_pan = true;
            __prevMouseX = ix;
            __prevMouseY = iy;
            return;
        }
    } 
    else if (PES5_IsStitchSimulatorMode()) {
        // pan only
        __start_pan = true;
        __prevMouseX = ix;
        __prevMouseY = iy;
        return;
    }

    const int selectCount = PES5_MSSelectCount();
    const bool hasSelect = selectCount > 0;
    const bool isSingleSelect = selectedIndex > -1;
    const bool isMultiSelect = selectCount > 1;
    pesVec2f canvasOrigin(_width/2, _height/2);
    bool hit = false;
    if (hasSelect && isShiftKeyOnly) {
        int cnt = pesGetDocument()->getObjectCount();
        for(int i=cnt-1; i>=0; i--){
            if( pesGetDocument()->getDataObject(i)->parameter.locked ) {
                continue;
            }
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin,
                                           pesGetDocument()->getDataObject(i).get());
            pesRectangle localBound = pesGetDocument()->getDataObject(i)->getBoundingBox();
            if(localBound.inside(p.x, p.y)){
                PES5_SetObjectSelectedIndex(i, isShiftKeyOnly);
                selectedLocalBound.set(localBound);
                selectedCorner = "global";
                pressedX = ix;
                pressedY = iy;
                setCursorByID("canvas", "move");
                hit = true;
                break;
            }
        }
    }

    if (!hit) {
        if (hasSelect && isShiftKey && isPlatformCtrlKey) {
            //        SkDebugf("Click through\n");
            int cnt = pesGetDocument()->getObjectCount();
            int last = isMultiSelect ? cnt : selectedIndex;
            for (int i = last - 1; i >= 0; i--) {
                if (pesGetDocument()->getDataObject(i)->parameter.locked) {
                    continue;
                }
                if (isMultiSelect && PES5_MSIsSelected(i)) {
                    continue;
                }
                pesVec2f p = PES5_ToLocalPoint(
                        ix, iy, canvasOrigin, pesGetDocument()->getDataObject(i).get());
                pesRectangle localBound = pesGetDocument()->getDataObject(i)->getBoundingBox();
                if (localBound.inside(p.x, p.y)) {
                    PES5_SetObjectSelectedIndex(i, isMultiSelect);
                    selectedLocalBound.set(localBound);
                    selectedCorner = "global";
                    pressedX = ix;
                    pressedY = iy;
                    setCursorByID("canvas", "move");
                    hit = true;
                    break;
                }
            }
        }
    }

    if (!hit) {
        std::string corner = checkCorners(ix, iy);
        if(corner=="undefined"){
            setCursorByID("canvas", "default");

            int cnt = pesGetDocument()->getObjectCount();
            for(int i=cnt-1; i>=0; i--){
                if( pesGetDocument()->getDataObject(i)->parameter.locked ) {
                    continue;
                }
                pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin,
                                               pesGetDocument()->getDataObject(i).get());
                pesRectangle localBound = pesGetDocument()->getDataObject(i)->getBoundingBox();
                if(localBound.inside(p.x, p.y)){
                    PES5_SetObjectSelectedIndex(i, isShiftKeyOnly);
                    selectedLocalBound.set(localBound);
                    selectedCorner = "global";
                    pressedX = ix;
                    pressedY = iy;
                    hit = true;
                    break;
                }
            }
        }
        else{
            pressedX = ix;
            pressedY = iy;

            int rotateDegree = 0;
            if (isSingleSelect) {
                rotateDegree = pesGetDocument()->getDataParameter(selectedIndex).rotateDegree;
            }

            auto action = getActionFromCorner(corner);
            actionDesc.action = action;
            actionDesc.mx = ix;
            actionDesc.my = iy;
            actionDesc.maxAbsDegree = 0; 
            actionDesc.degree = rotateDegree;
            actionDesc.tx = 0;
            actionDesc.ty = 0;
            actionDesc.dw = 0;
            actionDesc.dh = 0;
            actionDesc.sx = 0;
            actionDesc.sy = 0;

            if (action == "scale" || action == "scaleX" || action == "scaleY") {
                auto cursor = getRotatedCornerCursor(corner, rotateDegree);
                setCursorByID("canvas", cursor);
            }

            if (isSingleSelect) {
                if (pesGetDocument()->getDataObject(selectedIndex)->parameter.locked == false) {
                    hit = true;
                }
            } else {
                hit = true;
            }
        }
        selectedCorner = corner;
    }
    
    if (!hit && !isShiftKeyOnly) {
        PES5_SetObjectSelectedIndex(-1);
        __start_pan = true;
        __prevMouseX = ix;
        __prevMouseY = iy;
    }
}

void Pes5DocumentView::mouseMoved(int ix, int iy){
    if(PES5_IsPathEditInputStarted()){
        bool b = PES5_PathEditMouseMoved(ix, iy);
        dirty(b, false);
        return;
    }
    else if(PES5_IsStitchEditInputStarted()){
        bool b = PES5_StitchEditMouseMoved(ix, iy);
        dirty(b, false);
        return;
    }
    
    const bool hasSelect = PES5_MSSelectCount();
    if (hasSelect) {
        const bool isSingleSelect = selectedIndex > -1;
        std::string corner = checkCorners(ix, iy);
        if (corner == "undefined") {
            setCursorByID("canvas", "default");
        } else {
            pressedX = ix;
            pressedY = iy;

            auto action = getActionFromCorner(corner);
            if (action == "scale" || action == "scaleX" || action == "scaleY") {
                auto cursor = getRotatedCornerCursor(corner, actionDesc.degree);
                setCursorByID("canvas", cursor);
            }
        }
        selectedCorner = corner;
    }
}

float wrap(float value, float from, float to){
    if(from>to){
        float bk = from;
        from = to;
        to = bk;
    }
    float cycle = to - from;
    if(abs(cycle)<FLT_EPSILON){
        return to;
    }
    return value - cycle * floor((value - from) / cycle);
}

int sign(float v){
    if(v<0.0) return -1;
    else return 1;
}

void Pes5DocumentView::mouseDragged(int ix, int iy){
    if(__start_pan){
        if(__prevMouseX!=-1 && __prevMouseY!=-1){
            int dx = (ix - __prevMouseX);
            int dy = (iy - __prevMouseY);
            pesVec2f pan = PES5_GetPan() + pesVec2f(dx, dy);
            PES5_SetPan(pan);
            __prevMouseX = ix;
            __prevMouseY = iy;
        }
        return;
    }
    
    if(PES5_IsPathEditInputStarted()){
        PES5_PathEditMouseDragged(ix, iy);
        return;
    }
    else if(PES5_IsStitchEditInputStarted()){
        PES5_StitchEditMouseDragged(ix, iy);
        return;
    }
    
    const bool hasSelect = PES5_MSSelectCount();
    const bool isSingleSelect = selectedIndex > -1;
    pesData* ppesdata =
            isSingleSelect ? pesGetDocument()->getDataObject(selectedIndex).get() : nullptr;

    pesVec2f canvasOrigin(_width/2, _height/2);
    
    if (selectedCorner == "undefined" && hasSelect) {
        selectedCorner = "global";
    }
    
    if(selectedCorner!="undefined"){
        std::string action = getActionFromCorner(selectedCorner);
        actionDesc.action = action;
        actionDesc.mx = ix;
        actionDesc.my = iy;
        if (action == "drag") {
            actionDesc.tx = (ix-pressedX) / PES5_GetZoomScale();
            actionDesc.ty = (iy-pressedY) / PES5_GetZoomScale();
        }
        else if(action=="rotate"){
            pesRectangle localBound(selectedLocalBound);
            pesVec2f center = localBound.getCenter();
            //pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, pesGetDocument()->getDataObject(selectedIndex).get());
            pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, ppesdata);
            pesVec2f dir(p - center);
            dir.normalize();
            pesVec2f up(0, -1);
            float rotateDegree = 0;
            if (isSingleSelect) {
                rotateDegree = pesGetDocument()->getDataParameter(selectedIndex).rotateDegree;
            }
            float angle = up.angle(dir);
            float maxAbsDegree = abs(angle);
            if (maxAbsDegree > actionDesc.maxAbsDegree) {
                actionDesc.maxAbsDegree = maxAbsDegree;
            }
            actionDesc.degree = wrap(rotateDegree + angle, -180, 180);
            //SkDebugf("degree = %.3f\n", actionDesc.degree);
        }
        else{
            // scale
            pesVec2f mouse = PES5_ToLocalPoint(ix, iy, canvasOrigin, ppesdata);
            pesVec2f pressed = PES5_ToLocalPoint(pressedX, pressedY, canvasOrigin, ppesdata);
            //float dw = mouse.x - pressed.x;
            //float dh = mouse.y - pressed.y;
            const float dw = (mouse.x - pressed.x) * 2;
            const float dh = (mouse.y - pressed.y) * 2;

            if(actionDesc.action=="scaleX"){
                //actionDesc.dw = dw * 2;
                actionDesc.dw = dw;
                actionDesc.dh = 0;
            }
            else if(actionDesc.action=="scaleY"){
                actionDesc.dw = 0;
                //actionDesc.dh = dh * 2;
                actionDesc.dh = dh;
            }
            else if(actionDesc.action=="scale"){
                //float d = sqrt(dw*dw + dh*dh);
                //actionDesc.dw = d * sign(dw) * 2;
                //actionDesc.dh = (selectedLocalBound.height/selectedLocalBound.width) * actionDesc.dw;
                const float w = selectedLocalBound.width;
                const float h = selectedLocalBound.height;
                const float scalex = (w + dw) / w;
                const float scaley = (h + dh) / h;

                //sol.1
                //const float scalemax = max(scalex, scaley);
                //const float neww = scalemax * w;
                //const float newh = scalemax * h;
                //actionDesc.dw = neww - w;
                //actionDesc.dh = newh - h;
                
                //sol.2
                const float scalemax = std::max(scalex, scaley);
                actionDesc.sx = scalemax;
                actionDesc.sy = scalemax;
                const float invscalemax = scalemax - 1;
                actionDesc.dw = invscalemax * w;
                actionDesc.dh = invscalemax * h;
            }
        }
    }
    else{
        actionDesc.action = "undefined";
    }
}

float min(float a, float b){
    if(a<b) return a;
    return b;
}

void Pes5DocumentView::mouseReleased(int ix, int iy){
//    if (selectedIndex < 0)
//        return;
    if(__start_pan){
        __start_pan = false;
        __prevMouseX = __prevMouseY = -1;
        return;
    }
    
    if(PES5_IsPathEditInputStarted()){
        PES5_PathEditMouseReleased(ix, iy);
        return;
    }
    else if(PES5_IsStitchEditInputStarted()){
        PES5_StitchEditMouseReleased(ix, iy);
        return;
    }
    
    const bool hasSelect = PES5_MSSelectCount();
    if (!hasSelect)
        return;

    const bool isSingleSelect = selectedIndex > -1;
    pesData* ppesdata =
            isSingleSelect ? pesGetDocument()->getDataObject(selectedIndex).get() : nullptr;
    
    pesVec2f canvasOrigin(_width/2, _height/2);
    
    if(selectedCorner!="undefined"){
        std::string action = getActionFromCorner(selectedCorner);
        if(action=="drag"){
            float tx = (ix-pressedX) / PES5_GetZoomScale();
            float ty = (iy-pressedY) / PES5_GetZoomScale();
            PES5_MSTranslate(tx, ty);
        }
        else if(action=="rotate"){
            const float degreeTolerance = 2.0;
            if (actionDesc.maxAbsDegree > degreeTolerance) {
                pesRectangle localBound(selectedLocalBound);
                pesVec2f center = localBound.getCenter();
                pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, ppesdata);
                pesVec2f dir(p - center);
                dir.normalize();
                pesVec2f up(0, -1);
                float rotateDegree = actionDesc.degree;
                float angle = up.angle(dir);
                actionDesc.degree = wrap(rotateDegree + angle, -180, 180);
                PES5_MSRotate2(angle);
            }
        }
        else{
            // scale
            //auto pes = pesGetDocument()->getDataObject(selectedIndex);
            //auto ocenter = isSingleSelect ? ppesdata->getBoundingBox().getCenter()
            //                              : PES5_MSGetBoundingBox().getCenter();
            if(actionDesc.action=="scaleX"){
                const float sx = (selectedLocalBound.width + actionDesc.dw) / selectedLocalBound.width;
                PES5_MSScale(sx, 1);
            }
            else if(actionDesc.action=="scaleY"){
                const float sy = (selectedLocalBound.height + actionDesc.dh) / selectedLocalBound.height;
                PES5_MSScale(1, sy);
            }
            else if(actionDesc.action=="scale"){
                const float sx = (selectedLocalBound.width + actionDesc.dw) / selectedLocalBound.width;
                const float sy = (selectedLocalBound.height + actionDesc.dh) / selectedLocalBound.height;
                PES5_MSScale(sx, sy);
            }
        }
        const bool isSingleSelect = selectedIndex > -1;
        if (!isSingleSelect) {
            actionDesc.maxAbsDegree = 0;
            actionDesc.degree = 0;
            actionDesc.tx = 0;
            actionDesc.ty = 0;
            actionDesc.dw = 0;
            actionDesc.dh = 0;
            actionDesc.sx = 0;
            actionDesc.sy = 0;
        }
    }
    
    actionDesc.action = "undefined";
}

CornerOrigin Pes5DocumentView::getOriginFromCorner(std::string corner){
    CornerOrigin origin;
    if(corner=="ml" || corner=="tl" || corner=="bl"){
        origin.x = "right";
    }
    else if(corner=="mr" || corner=="tr" || corner=="br"){
        origin.x = "left";
    }

    if(corner=="tl" || corner=="mt" || corner=="tr"){
        origin.y = "bottom";
    }
    else if(corner=="bl" || corner=="mb" || corner=="br"){
        origin.y = "top";
    }
    return origin;
}

std::string Pes5DocumentView::getActionFromCorner(std::string corner){
    if(corner=="global") return "drag";
    if(corner=="mtr") return "rotate";
    if(corner=="ml" || corner=="mr") return "scaleX";
    if(corner=="mt" || corner=="mb") return "scaleY";
    if(corner=="tl" || corner=="tr" || corner=="bl" || corner=="br") return "scale";
    
    return "undefined";
}

std::string Pes5DocumentView::checkCorners(int mx, int my){
    std::string corner = "undefined";
    
    if (PES5_MSSelectCount()) {
        const pesVec2f canvasOrigin(_width / 2, _height / 2);
        const bool isSingleSelect = selectedIndex > -1;
        pesData* ppesdata = isSingleSelect ? pesGetDocument()->getDataObject(selectedIndex).get() : nullptr;
        pesVec2f p = PES5_ToLocalPoint(mx, my, canvasOrigin, ppesdata);
        pesRectangle localBound =
                isSingleSelect ? pesGetDocument()->getDataObject(selectedIndex)->getBoundingBox()
                               : PES5_MSGetBoundingBox();
        if (!isSingleSelect) {
            localBound.x -= 4;
            localBound.y -= 4;
            localBound.width += 8;
            localBound.height += 8;
        }
        const float doc_scale = PES5_GetZoomScale();
        const float rs = 15 / doc_scale;
        const float x = localBound.x;
        const float y = localBound.y;
        const float w = localBound.width;
        const float h = localBound.height;
        const auto center = localBound.getCenter();
        const float cx = center.x;
        const float cy = center.y;
        const float px = p.x;
        const float py = p.y;
        const float rotateArmLength = 30 / doc_scale;

        pesRectangle r;
        r.setFromCenter(x + w, cy, rs, rs);
        if (r.inside(px, py)) {
            selectedLocalBound.set(localBound);
            if (isSingleSelect) {
                corner = "mr";
            }
            return corner;
        }

        r.setFromCenter(cx, y + h, rs, rs);
        if (r.inside(px, py)) {
            selectedLocalBound.set(localBound);
            if (isSingleSelect) {
                corner = "mb";
            }
            return corner;
        }

        r.setFromCenter(x + w, y + h, rs, rs);
        if (r.inside(p.x, p.y)) {
            selectedLocalBound.set(localBound);
            corner = "br";
            return corner;
        }

        r.setFromCenter(cx, y - rotateArmLength, rs, rs);
        if (r.inside(px, py)) {
            selectedLocalBound.set(localBound);
            setCursorByID("canvas", "crosshair");
            corner = "mtr";
            return corner;
        }

        if (localBound.inside(px, py)) {
            setCursorByID("canvas", "move");
            corner = "global";
            return corner;
        }
    }
    return corner;
}

const char* cursorMap[] = {
  "n-resize",
  "ne-resize",
  "e-resize",
  "se-resize",
  "s-resize",
  "sw-resize",
  "w-resize",
  "nw-resize"
};

const char* cursorOffset[] = {
  "mt",
  "tr",
  "mr",
  "br",
  "mb",
  "bl",
  "ml",
  "tl"
};

std::string Pes5DocumentView::getRotatedCornerCursor(std::string corner, float rotateDegree) {
    std::string cursor="default";
    int nOffset=0;

//#ifdef __EMSCRIPTEN__
//    EM_ASM({ console.log("rotateDegree:", $0); }, rotateDegree);
//#endif

    if (rotateDegree < 0) {
        rotateDegree = 360 + rotateDegree;
    }
    int nRotateDegree = std::round(rotateDegree + 22.5);
    nOffset = std::round((nRotateDegree%360)/45);
    if (nOffset<0){
        nOffset+=8;
    }
    int nCursorOffset=0;
    const std::size_t size = sizeof(cursorOffset);
    for (int i = 0, len = (int)size; i < len; i++) {
        if (corner == cursorOffset[i]) {
            nCursorOffset = i;
            break;
        }
    }
    nOffset+=nCursorOffset;
    nOffset%=8;
    cursor=cursorMap[nOffset];
    return cursor;
}

void Pes5DocumentView::setCursorByID(std::string id, std::string cursorStyle, bool force) {
#ifdef __EMSCRIPTEN__
    // emscripten
    EM_ASM({
      var id = '#' + UTF8ToString($0);
      var cursorStyle = UTF8ToString($1);
      var force = $2;
      var canvasElement = document.querySelector(id);
      if(!canvasElement) {
        canvasElement = document.querySelector('canvas');
      }
      if(canvasElement) {
        var cursor = canvasElement.style.cursor;
        var cursordata = canvasElement.getAttribute('cursor-data');
        var cursorforce = canvasElement.getAttribute('cursor-force');
        if (!cursorStyle) {
            force = false;
            cursor = "";
            canvasElement.style.cursor = cursor;
            cursorforce = "";
            canvasElement.setAttribute('cursor-force', cursorforce);
            cursorStyle = cursordata || 'default';
        }
        if (force) {
          if (cursorforce != cursorStyle) {
            canvasElement.setAttribute('cursor-force', cursorStyle);
          }
          if (cursor != cursorStyle) {
            canvasElement.style.cursor = cursorStyle;
          }
        }
        else {
          if (cursordata != cursorStyle) {
            canvasElement.setAttribute('cursor-data', cursorStyle);
          }
          if (cursorforce) {
            cursorStyle = cursorforce;
          }
          if (cursor != cursorStyle) {
            canvasElement.style.cursor = cursorStyle;
          }
        }
      }
    }, id.c_str(), cursorStyle.c_str(), force);
#else
    // native
    
#endif
}

void Pes5DocumentView::init() {
#ifdef __EMSCRIPTEN__
    char szx[512];
    sprintf(szx, "if( window.pes5SetCallback ){ pes5SetCallback('%lu'); }", (unsigned long)this);
    emscripten_run_script(szx);

    EM_ASM({
        eval(`
            window.fnSplitFilenameExt = function(pathfile) { 
                var filename = pathfile.substring(Math.max(pathfile.lastIndexOf('/'), pathfile.lastIndexOf('\\\\\\\\')) + 1); 
                var dotIndex = filename.lastIndexOf('.'); 
                if (dotIndex > 0) { 
                    return [ filename.substring(0, dotIndex), filename.substring(dotIndex) ]; 
                } 
                return [ filename, ""]; 
            }
        `);

        window['fnFileHandleWriteBlob'] = async function(handle, blobOrResponse) {
            console.debug('handle:', handle);
            const writable = await handle.createWritable();
            if ('stream' in blobOrResponse) {
                var stream = blobOrResponse.stream();
                await stream.pipeTo(writable);
                return handle;
            } else if ('body' in blobOrResponse) {
                await blobOrResponse.body.pipeTo(writable);
                return handle;
            }
            await writable.write(blobOrResponse);
            await writable.close();
            return handle;
        };

        window['fnSaveFilePicker'] = async function(filename, blob, types) {
            if (window.showSaveFilePicker) {
                console.debug('filename:', filename);
                var options = {};
                options['suggestedName'] = filename;
                options['startIn'] = 'documents';
                if (window['lastfilesystemhandle']) {
                    options['startIn'] = window['lastfilesystemhandle'];
                }
                if (types) {
                    options['types'] = types;
                    options['excludeAcceptAllOption'] = true;
                }
                var filehandle = await window.showSaveFilePicker(options)
                                         .then((f) => window['fnFileHandleWriteBlob'](f, blob))
                                         .catch((e) => console.debug('onrejected:', e));
                if (filehandle) {
                    window['lastfilesystemhandle'] = filehandle;
                }
                return filehandle;
            }
            return undefined;
        };

        window['fnSaveFileLegacy'] = async function(filename, blob) {
            var a = document.createElement('a');
            document.body.appendChild(a);
            a.href = window.URL.createObjectURL(blob);
            a.download = filename;
            a.onclick = function() {
                setTimeout(
                        function() {
                            window.URL.revokeObjectURL(a.href);
                            document.body.removeChild(a);
                        },
                        1500);
            };
            a.click();
        };

        window['fnDirectoryPicker'] = async function(readonly) {
            if (window.showDirectoryPicker) {
                var options = {};
                options['mode'] = readonly ? 'read' : 'readwrite';
                options['startIn'] = 'documents';
                if (window['lastfilesystemhandle']) {
                    options['startIn'] = window['lastfilesystemhandle'];
                }
                var dirhandle = await window.showDirectoryPicker(options)
                    .catch((e) => console.debug('onreject:', e));
                if (dirhandle) {
                    window['lastfilesystemhandle'] = dirhandle;
                }
                return dirhandle;
            }
            return undefined;
        };
    });
#endif
}


}
