//
//  PES5Command.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 29/8/2565 BE.
//

#include "PES5Command.hpp"
#include "tools/Resources.h"
#include "include/utils/SkTextUtils.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
//#include "emscripten/val.h"
#endif

#include <fstream>
#include "pesEngine.h"
#include "pesPathUtility.hpp"
#include "Utils/PesSystemUtils.hpp"
#include "Utils/PesPPEFUtils.hpp"
#include "GUI/PES5RightPanel.hpp"
#include "GUI/PES5DocumentView.hpp"
#include "GUI/PES5ModalDialog.hpp"
#include "Utils/PesTracingUtils.hpp"
#include "Utils/PesSatinColumn.hpp"
//----------------------
// Undo/Redo & Command
//----------------------
UndoRedo __undoRedo;
PesPathEdit __pes_path_edit;
PesStitchEdit __pes_stitch_edit;

bool CannotUndoRedo(){
    return (
                PES5_IsSatinColumnInputStarted() ||
                PES5_IsDrawingInputStarted() ||
                PES5_IsLinesToolInputStarted() ||
                PES5_IsPolygonToolInputStarted() ||
                PES5_IsStitchSimulatorMode()
            );
}

bool PES5_CanUndo(){
    return __undoRedo.canUndo();
}

bool PES5_CanRedo(){
    return __undoRedo.canRedo();
}

std::string getUndoName(){
    if(PES5_IsPathEditInputStarted()){
        return __pes_path_edit.canUndo() ? "Undo(" + __pes_path_edit.getUndoName() + ")" : "";
    }
    else if(PES5_IsStitchEditInputStarted()){
        return __pes_stitch_edit.canUndo() ? "Undo(" + __pes_stitch_edit.getUndoName() + ")" : "";
    }
    return __undoRedo.canUndo() ? "Undo(" + __undoRedo.getUndoName() + ")" : "";
}

std::string getRedoName(){
    if(PES5_IsPathEditInputStarted()){
        return __pes_path_edit.canRedo() ? "Redo(" + __pes_path_edit.getRedoName() + ")" : "";
    }
    else if(PES5_IsStitchEditInputStarted()){
        return __pes_stitch_edit.canRedo() ? "Redo(" + __pes_stitch_edit.getRedoName() + ")" : "";
    }
    return __undoRedo.canRedo() ? "Redo(" + __undoRedo.getRedoName() + ")" : "";
}

void PES5_UpdateUndoRedoTooltips(){
    std::string undoname = getUndoName();
    std::string redoname = getRedoName();
#ifdef __EMSCRIPTEN__
    EM_ASM({
        var undoElm = document.getElementById('undo-button');
        var redoElm = document.getElementById('redo-button');
        if(undoElm){
            undoElm.title = UTF8ToString($0);
        }
        if(redoElm){
            redoElm.title = UTF8ToString($1);
        }
    }, undoname.c_str(), redoname.c_str());
#endif
}

void PES5_Undo(){
    if(PES5_IsPathEditInputStarted()){
        __pes_path_edit.undo();
    }
    else if(PES5_IsStitchEditInputStarted()){
        __pes_stitch_edit.undo();
    }
    else if(!CannotUndoRedo() && PES5_CanUndo()){
        __undoRedo.undo();
    }
    PES5_UpdateUndoRedoTooltips();
}

void PES5_Redo(){
    if(PES5_IsPathEditInputStarted()){
        __pes_path_edit.redo();
    }
    else if(PES5_IsStitchEditInputStarted()){
        __pes_stitch_edit.redo();
    }
    else if(!CannotUndoRedo() && PES5_CanRedo()){
        __undoRedo.redo();
    }
    PES5_UpdateUndoRedoTooltips();
}

//void PES5_UndoRedoPushCommand(Command* command){
//    __undoRedo.pushCommand(command);
//}

void PES5_UndoRedoPushCommand(Command command){
    __undoRedo.pushCommand(std::make_shared<Command>(command));
    PES5_UpdateUndoRedoTooltips();
}

void PES5_UndoRedoPushCommand(std::shared_ptr<Command> command){
    __undoRedo.pushCommand(command);
}

std::string PES5_GetRedoName(){
    return __undoRedo.getRedoName();
}

std::string PES5_GetUndoName(){
    return __undoRedo.getUndoName();
}

void PES5_UndoRedoClear(){
    __undoRedo.clear();
    PES5_UpdateUndoRedoTooltips();
}

//----------------------

std::vector<sk_sp<SkImage>> g_pesImages;
std::vector<sk_sp<SkImage>> g_pesImagesThumbnail;
int __pes_object_selected_index = -1;

const std::string PES5_GetResourcePath(const std::string& resource) {
    return GetResourcePath(resource.c_str()).c_str();
}

void PES5_SetResourcePath(const std::string& resource) {
    SetResourcePath(resource.c_str());
}

void PES5_SetDocumentName(const std::string docname) {
    if (!docname.empty() && pesGetDocument()->getName() != docname) {
        pesGetDocument()->setName(docname);
    }
}

const std::string PES5_GetDocumentName() {
    return pesGetDocument()->getName();
}

void PES5_NewDocument(){
    // fixed: some processes are corrupted by inaccessible data due to use images.size() is the
    // counter limit. then accesses randomly using an index counter to reference another collection
    // with mismatch in size.
    PES5_SetObjectSelectedIndex(-1);
    g_pesImages.clear();
    g_pesImagesThumbnail.clear();
    pesGetDocument()->newDocument();
    PES5_ResetZoomScale();
    PES5_ResetPan();
    PES5_UpdateGUIRightPanel();
    //PES5_SetDocumentName("Untitled");

//#ifdef __EMSCRIPTEN__
//    EM_ASM({
//        if (window['scaleToFitDocView']) {
//            window['scaleToFitDocView']();
//        }
//    });
//#endif

#ifdef __EMSCRIPTEN__
    // set zoom-scale-animate in js
    EM_ASM({
        window['setDocViewBoxToFitAppViewPort'] && window['setDocViewBoxToFitAppViewPort'](0, true);
    });
#else
    if (sk_ui::GetPesDocumentView() != nullptr) {
        pesVec2f hoop = pesGetDocument()->getHoopSizeInMM();
        float hw = hoop.x;
        float hh = hoop.y;
        float docw = sk_ui::GetPesDocumentView()->width();
        float doch = sk_ui::GetPesDocumentView()->height();
        float sx = docw / (hw * 10);
        float sy = doch / (hh * 10);
        float s = std::min(sx, sy);
        PES5_ResetPan();
        PES5_ResetZoomScale();
        PES5_SetZoomScale(s * 0.95);
    }
#endif
    
    PES5_UndoRedoClear();
    PES5_MSClearSelection();
}

bool PES5_LoadPPES(const std::string & path){
    sk_sp<SkData> data = SkData::MakeFromFileName(path.c_str());
    pesBuffer buff((const char*)data->data(), data->size());
    return pesGetDocument()->loadPPESFromBuffer(buff);
//    PES5_UpdateGUIRightPanel();
}

void PES5_SavePPES(const std::string & path){
    pesBuffer buffer = pesGetDocument()->getPPESBuffer();
    if(buffer.size()){
        std::fstream fs;
        fs.open(path.c_str(), std::ios::out | std::ios::binary);
        fs.write(buffer.getData(), buffer.size());
    }
}

bool PES5_ImportPES(const std::string & path){
    sk_sp<SkData> data = SkData::MakeFromFileName(path.c_str());
    pesBuffer buff((const char*)data->data(), data->size());
    pesData pes;
    if(pes.loadPESFromBuffer(buff)){
        ImportCommand command(&pes);
        command.execute();
        PES5_UndoRedoPushCommand(command);
        return true; // load success
    }
    return false; // load fail
}

bool PES5_ImportSVG(const std::string & path){
    sk_sp<SkData> data = SkData::MakeFromFileName(path.c_str());
    pesBuffer buff((const char*)data->data(), data->size());
    pesData pes;
    if(pes.loadSVGFromBuffer(buff)){
        ImportCommand command(&pes);
        command.execute();
        PES5_UndoRedoPushCommand(command);
        return true; // load success
    }
    return false; // load fail
}

void PES5_ExportPES(const std::string & path){
    bool orderByColor = true;
    pesBuffer buffer = pesGetDocument()->getPES5Buffer(orderByColor);
    if(buffer.size()){
        std::fstream fs;
        fs.open(path.c_str(), std::ios::out | std::ios::binary);
        fs.write(buffer.getData(), buffer.size());
    }
}

void PES5_ExportPNG(const std::string & path){
    pesBuffer buffer = pesGetDocument()->getPNGBuffer();
    if(buffer.size()){
        std::fstream fs;
        fs.open(path.c_str(), std::ios::out | std::ios::binary);
        fs.write(buffer.getData(), buffer.size());
    }
}

static void release_direct_surface_storage(void* pixels, void* context) {
    if (pixels == context) {
        // SkDebugf("expected release context\n");
    }
    sk_free(pixels);
}

SkImageInfo imageInfoThumbnail = SkImageInfo::Make( 96, 96, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
sk_sp<SkSurface> surfaceThumbnail = SkSurface::MakeRaster(imageInfoThumbnail);

void PES5_UpdatePesImagesThumbnail(){
    //g_pesImagesThumbnail.clear();
    int cnt = g_pesImages.size();
    for (int i = 0; i < cnt; i++) {
        SkCanvas* canvas = surfaceThumbnail->getCanvas();
        //canvas->clear(SK_ColorWHITE);
        canvas->clear(SK_ColorTRANSPARENT);

        if (i >= g_pesImagesThumbnail.size()) {
            g_pesImagesThumbnail.push_back(surfaceThumbnail->makeImageSnapshot());
        }

        sk_sp<SkImage> img = g_pesImages[i];
        if (img.get() == nullptr) {
            g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
            continue;
        }

        float w = img->width();
        float h = img->height();
        if (w == 0 || h == 0) {
            g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
            continue;
        }

        const float t_padding_w = 4.0;
        float t_scale = 1.0;
        float t_size = 96.0 - t_padding_w - t_padding_w;

        float x = 0;
        float y = 0;
        if (w > h) {
            t_scale = t_size / w;
            h = t_size * h / w;
            w = t_size;
            x = t_padding_w;
            y = (t_size - h) / 2 + t_padding_w;
        } else {
            t_scale = t_size / h;
            w = t_size * w / h;
            h = t_size;
            x = (t_size - w) / 2 + t_padding_w;
            y = t_padding_w;
        }

        canvas->drawImageRect(img, SkRect::MakeXYWH(x, y, w, h), SkSamplingOptions(SkFilterMode::kLinear));

        g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
    }
}


void PES5_UpdatePesImageThumbnail(int index) {
    // if index is not wrong, then loop once.
    int cnt = g_pesImages.size();
    for (int i = index; i < cnt; i=cnt) {
        SkCanvas* canvas = surfaceThumbnail->getCanvas();
        //canvas->clear(SK_ColorWHITE);
        canvas->clear(SK_ColorTRANSPARENT);

        if (i >= g_pesImagesThumbnail.size()) {
            g_pesImagesThumbnail.push_back(surfaceThumbnail->makeImageSnapshot());
        }

        sk_sp<SkImage> img = g_pesImages[i];
        if (img.get() == nullptr) {
            g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
            continue;
        }

        float w = img->width();
        float h = img->height();
        if (w == 0 || h == 0) {
            g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
            continue;
        }

        const float t_padding_w = 4.0;
        float t_scale = 1.0;
        float t_size = 96.0 - t_padding_w - t_padding_w;

        float x = 0;
        float y = 0;
        if (w > h) {
            t_scale = t_size / w;
            h = t_size * h / w;
            w = t_size;
            x = t_padding_w;
            y = t_padding_w + (t_size - h) / 2;
        } else {
            t_scale = t_size / h;
            w = t_size * w / h;
            h = t_size;
            x = t_padding_w + (t_size - w) / 2;
            y = t_padding_w;
        }

        canvas->drawImageRect(img, SkRect::MakeXYWH(x, y, w, h), SkSamplingOptions(SkFilterMode::kLinear));

        g_pesImagesThumbnail[i] = surfaceThumbnail->makeImageSnapshot();
    }
}

void PES5_UpdatePesImages(){
    g_pesImages.clear();
    auto pesDoc = pesGetDocument();
    int cnt = pesDoc->getObjectCount();
    for(int i=0; i<cnt; i++){
        if(pesDoc->getDataObject(i)->isScalable()){
            if(pesDoc->getDataObject(i)->getTotalStitches()){
                g_pesImages.push_back(pesDoc->makePesImageSnapshot(i));
            }else{
                g_pesImages.push_back(pesDoc->makePesScalableImageSnapshot(i));
            }
        }
        else{
            if(pesDoc->getDataObject(i)->parameter.type==pesData::OBJECT_TYPE_BACKGROUND){
                g_pesImages.push_back(pesDoc->makePesBackgroundImageSnapshot(i));
            }
            else{
                g_pesImages.push_back(pesDoc->makePesImageSnapshot(i));
            }
        }
    }
    pesDoc->updateInfo();
    PES5_UpdatePesImagesThumbnail();

#ifdef __EMSCRIPTEN__
    //char szx[512];
    //sprintf(szx, "if( window.pes5updateLayerList ) { pes5updateLayerList(-1) }");
    //emscripten_run_script(szx);
    EM_ASM({ 
        if (window["pes5updateLayerList"]) { window["pes5updateLayerList"]($0); }
    }, -1);
#endif
}

void PES5_UpdatePesImage(int index){
//    int cnt = pesGetDocument()->getObjectCount();
    int cnt = g_pesImages.size();
    if(index>=cnt){
        for(int i=cnt; i<=index; i++){
            if(pesGetDocument()->getDataObject(i)->isScalable()){
                if(pesGetDocument()->getDataObject(i)->getTotalStitches()){
                    g_pesImages.push_back(pesGetDocument()->makePesImageSnapshot(i));
                }else{
                    g_pesImages.push_back(pesGetDocument()->makePesScalableImageSnapshot(i));
                }
            }
            else{
//                g_pesImages.push_back(pesGetDocument()->makePesImageSnapshot(i));
                if(pesGetDocument()->getDataObject(i)->parameter.type==pesData::OBJECT_TYPE_BACKGROUND){
                    g_pesImages.push_back(pesGetDocument()->makePesBackgroundImageSnapshot(i));
                }
                else{
                    g_pesImages.push_back(pesGetDocument()->makePesImageSnapshot(i));
                }
            }
        }
    }
    else if(index<cnt){
        if(pesGetDocument()->getDataObject(index)->isScalable()){
            if(pesGetDocument()->getDataObject(index)->getTotalStitches()){
                g_pesImages[index] = pesGetDocument()->makePesImageSnapshot(index);
            }else{
                g_pesImages[index] = pesGetDocument()->makePesScalableImageSnapshot(index);
            }
        }
        else{
//            g_pesImages[index] = pesGetDocument()->makePesImageSnapshot(index);
            if(pesGetDocument()->getDataObject(index)->parameter.type==pesData::OBJECT_TYPE_BACKGROUND){
                g_pesImages[index] = pesGetDocument()->makePesBackgroundImageSnapshot(index);
            }
            else{
                g_pesImages[index] = pesGetDocument()->makePesImageSnapshot(index);
            }
        }
    }
    pesGetDocument()->updateInfo();
    PES5_UpdatePesImageThumbnail(index);

#ifdef __EMSCRIPTEN__
    //char szx[512];
    //sprintf(szx, "if( window.pes5updateLayerList ) { pes5updateLayerList(%i) }", index);
    //emscripten_run_script(szx);
    EM_ASM({ 
        if (window["pes5updateLayerList"]) { window["pes5updateLayerList"]($0); }
    }, index);
#endif
}

int PES5_GetPesImageCount(){
    int cnt = g_pesImages.size();
    return cnt;
}

sk_sp<SkImage> PES5_GetPesImage(int index){
    return g_pesImages[index];
}

sk_sp<SkImage> PES5_GetPesImageThumbnail(int index){
    return g_pesImagesThumbnail[index];
}

sk_sp<SkImage> PES5_GetSVGPartTypeImage( int layerIndex, std::string filter ) {
    //SkDebugf( "Snapshot %s\n", filter.c_str() );
    return pesGetDocument()->makePesScalableImageSnapshot(layerIndex, filter);
    //return g_pesImages[layerIndex];
}

float __pes_current_zoom = 1.0;
//float __pes_zoom[] = {0.25, 0.5, 1.0, 1.5, 2.0, 3.0};
//int __pes_zoom_index = 1;
void PES5_ZoomIn(){
    //if(++__pes_zoom_index>5){
    //    __pes_zoom_index = 5;
    //}
    __pes_current_zoom += 0.1;
    if( __pes_current_zoom > 3.0 ) {
        __pes_current_zoom = 3.0;
    }
}

void PES5_ZoomOut(){
    //if(--__pes_zoom_index<0){
    //    __pes_zoom_index = 0;
    //}
    __pes_current_zoom -= 0.1;
    if( __pes_current_zoom < 0.2 ) {
        __pes_current_zoom = 0.2;
    }
}

void PES5_ResetZoomScale() {
    //__pes_zoom_index = 1;
    __pes_current_zoom = 1.0;
}

void PES5_SetZoomScale( float z ) {
    __pes_current_zoom = z;
}

float PES5_GetZoomScale() {
    return __pes_current_zoom;
}


pesVec2f __pes_pan = {0, 0};
float __rulurw = 0;
pesVec2f PES5_GetPan() {
    return __pes_pan;
}

void PES5_SetPan(const pesVec2f & pan){
    __pes_pan = pan;
}

void PES5_ResetPan(){
    __pes_pan.set(0, 0);
    __rulurw = 0;
}

void PES5_SetHasRulur(float rulurw) {
    if (__rulurw != rulurw) {
        __rulurw = rulurw;
        if (rulurw) {
            __pes_pan.translate(rulurw, rulurw);
        } else {
            __pes_pan.translate(-rulurw, -rulurw);
        }
    }
}

pesVec2f PES5_ToLocalPoint(float x, float y, const pesVec2f & canvasOrigin, pesData *pes){
    pesVec2f point(x, y);
    point-=(canvasOrigin+PES5_GetPan());
    point/=PES5_GetZoomScale();
    
    if(pes){
        const float angle = pes->parameter.rotateDegree;
        if(abs(angle)>FLT_EPSILON){
            pesVec2f pivot = pes->getBoundingBox().getCenter();
            point.rotate(-angle, pivot);
        }
    }
    
    return point;
}

pesVec2f PES5_ToLocalPoint(float x, float y, const pesVec2f & canvasOrigin, const pesVec2f & objectCenter, float objectRotateDegree){
    pesVec2f point(x, y);
    point-=(canvasOrigin+PES5_GetPan());
    point/=PES5_GetZoomScale();
    
    const float angle = objectRotateDegree;
    if(abs(angle)>FLT_EPSILON){
        pesVec2f pivot = objectCenter;
        point.rotate(-angle, pivot);
    }
    
    return point;
}

int PES5_SetObjectSelectedIndex(int index, bool multiselect) {
    if (index < -1 || index >= PES5_GetObjectCount()) {
        index = -1;
    }
    //SkDebugf("PES5::index: %d, multiselect:%lu\n", index, multiselect);
    const int osize = (int)g_selected_objects.size();

    if (index > -1) {
        PES5_MSSelect(index, multiselect);
        if (g_selected_objects.size() == 1) {
            index = *g_selected_objects.begin();
        } else {
            index = -1;
        }
    } else {
        // clear all
        g_selected_objects.clear();
        
        // select all
        if (multiselect) {
            if (PES5_GetObjectCount()) {
                for(int i=0; i<PES5_GetObjectCount(); i++){
                    g_selected_objects.emplace(i);
                }
            }
        }
    }

    if (__pes_object_selected_index != index) {
        __pes_object_selected_index = index;

        sk_ui::GetPesDocumentView()->setSelectedIndex(index);
        sk_ui::GetLayerList()->select_listItem(index);
        PES5_UpdateGUIRightPanel();

        #ifdef __EMSCRIPTEN__
        EM_ASM({
            if (window["pes5updateLayerListSelect"]) { window["pes5updateLayerListSelect"]($0); }
        }, index);
        #endif
    } 
    else {
        if (osize != (int)g_selected_objects.size()) {
            #ifdef __EMSCRIPTEN__
            EM_ASM({ 
                if (window["pes5updateLayerListSelect"]) { window["pes5updateLayerListSelect"]($0); }
            }, -1);
            #endif
        }
    }


    //SkDebugf("    ::selected:%d, count:%d\n", index, (int)g_selected_objects.size());

    return __pes_object_selected_index;
}

int PES5_GetObjectSelectedIndex(){
    return __pes_object_selected_index;
}

void PES5_OpenPPESFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           let input = document.createElement('input');
           input.type = 'file';
        //    input.accept = 'application/octet-stream,.ppes';
           input.accept = 'application/octet-stream,.ppes5';
           input.onchange = async function () {
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               var fName = f['name'];
               var fNameExt = window['fnSplitFilenameExt'](fName);
               //console.debug("filename:", f['name']);
               //console.debug("fnSplitFilenameExt:", fNameExt);
               
               var r = new FileReader();
               r.onload = async function () {
                   var data = new Uint8Array(r.result);
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   window.dispatchEvent(new CustomEvent('clearpathlist'));
                   CanvasKit['PES5Template']['NewDocument']();
                   CanvasKit['PES5Template']['UpdateImages']();
                   CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                   window.dispatchEvent(new CustomEvent('layerchanged'));
                   var success = pesDoc['loadPPESFromBuffer'](pbuff);
                   if(success){
                       CanvasKit['PES5Template']['SetDocumentName'](fNameExt[0]);
                       CanvasKit['PES5Template']['LoadPatternAndMotif']();
                        CanvasKit['PES5Template']['ReplaceWithDefaultFont']();
                        CanvasKit['PES5Template']['UpdateImages']();
                        CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                        await window['pes_data']['file']['updateHash']();
                        window.dispatchEvent(new CustomEvent('layerchanged'));
                        window.dispatchEvent(new CustomEvent('ppesloaded', {detail : {path : fName}}))
                        //window.dispatchEvent(new CustomEvent('scaletofit') );
                   }
                   window['setDocViewBoxToFitAppViewPort'] && window['setDocViewBoxToFitAppViewPort'](0, true);
                   console.debug("DocumentName:", CanvasKit['PES5Template']['GetDocumentName']());
                   pbuff['delete']();
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
    );
#else
        std::vector<std::string> accept;
        // accept.push_back("ppes");
        accept.push_back("ppes5");
        PesFileDialogResult result = PesSystemLoadDialog("Load PPES File", false, "", accept);
        if(result.bSuccess){
            //SkDebugf("filename:%s\n", result.fileName.c_str());
            PES5_NewDocument();
            bool success = PES5_LoadPPES(result.getPath());
            if(success){
                SkDebugf("Success\n");
                //PES5_LoadPatternAndMotif();
                PES5_ReplaceWithDefaultFont();
                PES5_UpdatePesImages();
                PES5_UpdateGUIRightPanel();
            }
        }
#endif
}

#include <map>
#include <Utils/PesStitchSimulator.hpp>
std::map<std::string, sk_sp<SkData>> g_cachePatternAndMotif;

sk_sp<SkData> PES5_GetCachePatternAndMotif(std::string filepath) {
    if (auto search = g_cachePatternAndMotif.find(filepath.c_str());
        search != g_cachePatternAndMotif.end()) {
        return g_cachePatternAndMotif[filepath.c_str()];
    }
    return nullptr;
}

void PES5_SetCachePatternAndMotif(std::string filepath, sk_sp<SkData> data) {
    g_cachePatternAndMotif[filepath.c_str()] = data;
}

void PES5_LoadPatternAndMotif() {
    int cnt = pesGetDocument()->getObjectCount();
    for (int i = 0; i < cnt; i++) {
        // Warning C26815 The pointer is dangling because it points at a temporary instance which
        // was destroyed pesData* pes = pesGetDocument()->getDataObject(i).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(i);
        pesData* pes = shared_ptr_pesData.get();
        if (pes) {
            if (pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_TTF_TEXT) {
                //PES5_UpdateTTFFont(i, pes->parameter.fontIndex);
                int typefaceIndex = sk_ui::GetTypefaceIndex(i);
                PES5_UpdateTTFFont(i, typefaceIndex);
            }
            if (pes->parameter.fillType == pesData::FILL_TYPE_PATTERN &&
                !pes->parameter.patternFill.patternBuffer.size()) {
                std::string name = pes->parameter.patternFill.patternName;
                std::string filepath = "Pattern/" + name + ".pas";
                // PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                //    pes->parameter.patternFill.patternBuffer.set((const char*)data->data(),
                //                                                 data->size());
                //    pes->applyFill();
                //    PES5_UpdatePesImage(i);
                //    PES5_UpdateGUIRightPanel();
                //});

                if (auto search = g_cachePatternAndMotif.find(filepath.c_str());
                    search != g_cachePatternAndMotif.end()) {
                    std::cout << "Found " << search->first << " " << search->second << '\n';
                    //SkDebugf("'%s' found", filepath.c_str());
                    sk_sp<SkData> data = g_cachePatternAndMotif[filepath.c_str()];
                    pes->parameter.patternFill.patternBuffer.set((const char*)data->data(),
                                                                 data->size());
                    pes->applyFill();
                    PES5_UpdatePesImage(i);
                    PES5_UpdateGUIRightPanel();
                } else {
                    std::cout << filepath.c_str()  << " Not found\n";
                    //SkDebugf("'%s' not found", filepath.c_str());
                    PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                        std::cout << filepath.c_str()  << " loaded\n";
                        //SkDebugf("'%s' loaded", filepath.c_str());
                        g_cachePatternAndMotif[filepath.c_str()] = data;
                        pes->parameter.patternFill.patternBuffer.set((const char*)data->data(),
                                                                     data->size());
                        pes->applyFill();
                        PES5_UpdatePesImage(i);
                        PES5_UpdateGUIRightPanel();
                    });
                }
            } else if (pes->parameter.fillType == pesData::FILL_TYPE_MOTIF &&
                       !pes->parameter.motifFill.patternBuffer.size()) {
                std::string name = pes->parameter.motifFill.patternName;
                std::string filepath = "Pattern/" + name + ".pmf";
                // PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data){
                //    pes->parameter.motifFill.patternBuffer.set((const char*)data->data(),
                //    data->size()); pes->applyFill(); PES5_UpdatePesImage(i);
                //    PES5_UpdateGUIRightPanel();
                //});

                if (auto search = g_cachePatternAndMotif.find(filepath.c_str());
                    search != g_cachePatternAndMotif.end()) {
                    // std::cout << "Found " << search->first << " " << search->second << '\n';
                    sk_sp<SkData> data = g_cachePatternAndMotif[filepath.c_str()];
                    pes->parameter.motifFill.patternBuffer.set((const char*)data->data(),
                                                               data->size());
                    pes->applyFill();
                    PES5_UpdatePesImage(i);
                    PES5_UpdateGUIRightPanel();
                } else {
                    // std::cout << "Not found\n";
                    PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                        g_cachePatternAndMotif[filepath.c_str()] = data;
                        pes->parameter.motifFill.patternBuffer.set((const char*)data->data(),
                                                                   data->size());
                        pes->applyFill();
                        PES5_UpdatePesImage(i);
                        PES5_UpdateGUIRightPanel();
                    });
                }
            }
            if (pes->parameter.strokeType == pesData::STROKE_TYPE_MOTIF &&
                !pes->parameter.motif.patternBuffer.size()) {
                std::string name = pes->parameter.motif.patternName;
                std::string filepath = "Pattern/" + name + ".pmf";
                // PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data){
                //    pes->parameter.motif.patternBuffer.set((const char*)data->data(),
                //    data->size()); pes->applyStroke(); PES5_UpdatePesImage(i);
                //    PES5_UpdateGUIRightPanel();
                //});

                if (auto search = g_cachePatternAndMotif.find(filepath.c_str());
                    search != g_cachePatternAndMotif.end()) {
                    // std::cout << "Found " << search->first << " " << search->second << '\n';
                    sk_sp<SkData> data = g_cachePatternAndMotif[filepath.c_str()];
                    pes->parameter.motif.patternBuffer.set((const char*)data->data(), data->size());
                    pes->applyStroke();
                    PES5_UpdatePesImage(i);
                    PES5_UpdateGUIRightPanel();
                } else {
                    // std::cout << "Not found\n";
                    PES5_LoadOrDownloadAsset(filepath.c_str(), [=](sk_sp<SkData> data) {
                        g_cachePatternAndMotif[filepath.c_str()] = data;
                        pes->parameter.motif.patternBuffer.set((const char*)data->data(),
                                                               data->size());
                        pes->applyStroke();
                        PES5_UpdatePesImage(i);
                        PES5_UpdateGUIRightPanel();
                    });
                }
            }
        }
    }
}

void PES5_ReplaceWithDefaultFont(){
    int cnt = PES5_GetObjectCount();
    for(int i=0; i<cnt; i++){
        //Warning C26815 The pointer is dangling because it points at a temporary instance which was destroyed
        //pesData* pes = pesGetDocument()->getDataObject(i).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(i);
        pesData* pes = shared_ptr_pesData.get();
        auto type = pes->parameter.type;
        if(type==pesData::OBJECT_TYPE_PES2_TEXT){
            SkDebugf("PES5_ReplaceWithDefaultFont() text=%s\n", pes->parameter.text.c_str());
            pes->parameter.fontName = "Thai001";
            pes->parameter.setType(pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT);
            pes->parameter.fillType = pesData::FILL_TYPE_SATIN_COLUMN;
            pes->parameter.strokeType = pesData::STROKE_TYPE_NONE;
            pes->parameter.lastFontSize = pes->parameter.fontSize;
            pes->parameter.ppefScaleX = pes->parameter.ppefScaleY = 1.0;
            PES5_ReplacePPEFText(i);
        }
    }
}

void PES5_SavePPESFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getPPESBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    // {'accept' : {'application/ppes' : '.ppes'}},
                    {'accept' : {'application/ppes5' : '.ppes5'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
                //console.debug('handle:', handle);
                if (handle && handle['kind'] == "file") {
                    var fNameExt = window['fnSplitFilenameExt'](handle['name']);
                    CanvasKit['PES5Template']['SetDocumentName'](fNameExt[0]);
                    await window['pes_data']['file']['updateHash']();
                    console.debug("DocumentName:", CanvasKit['PES5Template']['GetDocumentName']());
                }
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
                await window['pes_data']['file']['updateHash']();
            }
        })();        
    },
    filename);
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Save PPES File");
        if(result.bSuccess){
            PES5_SavePPES(result.getPath());
        }
#endif
}

void PES5_ImportPESFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           let input = document.createElement('input');
           input.type = 'file';
           input.accept = 'application/octet-stream,.pes';
           input.onchange = function(){
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               var r = new FileReader();
               r.onload = function () {
                   var data = new Uint8Array(r.result);
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   var pes = new CanvasKit['pesData']();
                   if(pes['loadPESFromBuffer'](pbuff, true)){
//                       console.log('import .pes: OK');
                       pesDoc['addObject'](pes);
                       CanvasKit['PES5Template']['UpdateImages']();
                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                   }
                   pes['delete']();
                   pbuff['delete']();
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
    );
#else
        std::vector<std::string> accept;
        accept.push_back("pes");
        PesFileDialogResult result = PesSystemLoadDialog("Import PES File", false, "", accept);
        if(result.bSuccess){
            //SkDebugf("result.getPath(): %s\n", result.getPath().c_str());
            //SkDebugf("filename:%s\n", result.fileName.c_str());
            PES5_ImportPES(result.getPath());
            PES5_UpdatePesImages();
        }
#endif
}

void PES5_ImportSVGFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           let input = document.createElement('input');
           input.type = 'file';
           input.accept = 'image/svg+xml,.svg';
           input.onchange = function(){
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               console.debug("filename:", f.name);
               var r = new FileReader();
               r.onload = function () {
                   var data = new Uint8Array(r.result);
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   var pes = new CanvasKit['pesData']();
                   if(pes['loadSVGFromBuffer'](pbuff, true)){
                       pesDoc['addObject'](pes);
                       CanvasKit['PES5Template']['UpdateImages']();
                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                   }
                   pes['delete']();
                   pbuff['delete']();
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
    );
#else
        std::vector<std::string> accept;
        accept.push_back("svg");
        PesFileDialogResult result = PesSystemLoadDialog("Import SVG File", false, "", accept);
        if(result.bSuccess){
            //SkDebugf("result.getPath(): %s\n", result.getPath().c_str());
            //SkDebugf("filename:%s\n", result.fileName.c_str());
            PES5_ImportSVG(result.getPath());
            PES5_UpdatePesImages();
        }
#endif
}

void PES5_ImportBackgroundFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           let input = document.createElement('input');
           input.type = 'file';
           input.accept = 'image/png,.png';
           input.onchange = function(){
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               console.debug("filename:", f.name);
               var r = new FileReader();
               r.onload = function () {
                   var data = new Uint8Array(r.result);
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   
                   var pes = new CanvasKit['pesData']();
                   if(pes['loadBackgroundFromBuffer'](pbuff, true)){
                       pesDoc['addObject'](pes);
                       CanvasKit['PES5Template']['UpdateImages']();
                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                   }
                   pes['delete']();
                   pbuff['delete']();
                   
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
    );
#else
    /*
        // Jimmy - not implemented yet
        std::vector<std::string> accept;
        accept.push_back("svg");
        PesFileDialogResult result = PesSystemLoadDialog("Import SVG File", false, "", accept);
        if(result.bSuccess){
            //SkDebugf("result.getPath(): %s\n", result.getPath().c_str());
            //SkDebugf("filename:%s\n", result.fileName.c_str());
            PES5_ImportSVG(result.getPath());
            PES5_UpdatePesImages();
        }
    */
#endif
}

void PES5_ExportPESFile(const char* filename, bool center){
    SkDebugf( "Recalculate all\n" );
    pesDocument *doc = pesGetDocument();
    int nSize = doc->getDataObjects().size();
    for( int i=0; i<nSize; i++ ) {
        doc->getDataObject(i)->recalculate();
    }

#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);
        var bCenter = $1;

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getPES5Buffer'](false, bCenter);
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/pes' : '.pes'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename, center);    
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export PES File");
        if(result.bSuccess){
            PES5_ExportPES(result.getPath());
        }
#endif
}

void PES5_ExportPNGFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getPNGBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'image/png' : '.png'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename);
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export PNG File");
        if(result.bSuccess){
            PES5_ExportPNG(result.getPath());
        }
#endif
}

// DST
void PES5_ExportDSTFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getDSTBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);

            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();            
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/dst' : '.dst'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename);    
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export DST File");
        if(result.bSuccess){
            std::string path = result.getPath();
            pesBuffer buffer = pesGetDocument()->getDSTBuffer();
            if(buffer.size()){
                std::fstream fs;
                fs.open(path.c_str(), std::ios::out | std::ios::binary);
                fs.write(buffer.getData(), buffer.size());
            }
        }
#endif
}

// XXX
void PES5_ExportXXXFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getXXXBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);

            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();            
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/xxx' : '.xxx'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename);    
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export XXX File");
        if(result.bSuccess){
            std::string path = result.getPath();
            pesBuffer buffer = pesGetDocument()->getXXXBuffer();
            if(buffer.size()){
                std::fstream fs;
                fs.open(path.c_str(), std::ios::out | std::ios::binary);
                fs.write(buffer.getData(), buffer.size());
            }
        }
#endif
}

// EXP
void PES5_ExportEXPFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getEXPBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);

            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();            
            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/exp' : '.exp'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename);
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export EXP File");
        if(result.bSuccess){
            std::string path = result.getPath();
            pesBuffer buffer = pesGetDocument()->getEXPBuffer();
            if(buffer.size()){
                std::fstream fs;
                fs.open(path.c_str(), std::ios::out | std::ios::binary);
                fs.write(buffer.getData(), buffer.size());
            }
        }
#endif
}

// JEF
void PES5_ExportJEFFile(const char* filename){
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }

        var savefilename = UTF8ToString($0);

        (async function() {
            let pesdoc = CanvasKit['pesDocument']['getInstance']();
            let pesbuff = pesdoc['getJEFBuffer']();
            let arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](pesbuff);
            let blob = new Blob([arraybuff]);
            pesbuff.clear();
            pesbuff.delete();
            //pesdoc.delete();

            console.debug("saving file:", savefilename);
            if (window.showSaveFilePicker) {
                var types = [
                    {'accept' : {'application/jef' : '.jef'}},
                ];
                var handle = await window['fnSaveFilePicker'](savefilename, blob, types);
            }
            else {
                await window['fnSaveFileLegacy'](savefilename, blob);
            }
        })();        
    },
    filename);
#else
        PesFileDialogResult result = PesSystemSaveDialog(filename, "Export JEF File");
        if(result.bSuccess){
            std::string path = result.getPath();
            pesBuffer buffer = pesGetDocument()->getJEFBuffer();
            if(buffer.size()){
                std::fstream fs;
                fs.open(path.c_str(), std::ios::out | std::ios::binary);
                fs.write(buffer.getData(), buffer.size());
            }
        }
#endif
}

// SVG(Scan N Cut)
void PES5_ExportSVGFile(const char* filename) {
    if (!pesGetDocument()) {
        return;
    }
    int objIndex = PES5_GetObjectSelectedIndex();
    const int objs = pesGetDocument()->getObjectCount();
    if (objIndex < -1 || objIndex >= objs) {
        return;
    }

    std::string strFilename = std::filesystem::path(filename).stem().string();
    if (strFilename.empty()) {
        return;
    }

    const char* szext = ".svg";
    const bool forScanNcut = true;

#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        // if(window['controlCenter']['activation']['activated']==false){
        //     // alert('กรุณาตรวจสอบการลงทะเบียน');
        //     console.log('กรุณาตรวจสอบการลงทะเบียน');
        //     return;
        // }
        
        var objs = $0;
        var index = $1;
        var filename = UTF8ToString($2);
        var ext = UTF8ToString($3);
        var forScanNcut = $4;
        var pad = (objs).toString().length;
        var fnMakeFileName = (index) => filename + '-' + (index).toString().padStart(pad, '0') + ext;

        var fnMakeBlob = (index, forScanNcut) => {
            var pesdata = CanvasKit['pesDocument']['getInstance']()['getDataObject'](index);
            var svgbuff = pesdata['getSVGBuffer'](forScanNcut);
            var arraybuff = CanvasKit['pesUtility']['PesBufferToByteArray'](svgbuff);
            // svgbuff['delete']();
            let b = new Blob([arraybuff]);
            svgbuff.clear();
            svgbuff.delete();
            pesdata.delete();
            return b;
        };

        (async function() {
            if (index == -1) {
                if (window.showDirectoryPicker) {
                    var dirhandle = await window['fnDirectoryPicker'](false);
                    if (dirhandle) {
                        var options = {'create' : true};
                        var files = [];
                        for (var i = 0; i < objs; i++) {
                            var blob = fnMakeBlob(i, forScanNcut);
                            var savefilename = fnMakeFileName(i);
                            console.debug("saving file:", savefilename);
                            var filehandle = await dirhandle.getFileHandle(savefilename, options)
                                            .then((f) => window['fnFileHandleWriteBlob'](f, blob))
                                            .catch((e) => console.debug('onrejected:', e));
                            files.push(filehandle);
                        }
                    }
                }
                else {
                    for (var i = 0; i < objs; i++) {
                        var blob = fnMakeBlob(i, forScanNcut);
                        var savefilename = fnMakeFileName(i);
                        console.debug("saving file:", savefilename);
                        await window['fnSaveFileLegacy'](savefilename, blob);
                    }
                }
            }
            else {
                var blob = fnMakeBlob(index, forScanNcut);
                var savefilename = fnMakeFileName(index);
                console.debug("saving file:", savefilename);
                if (window.showSaveFilePicker) {
                    var types = [
                        {'accept' : {'image/svg+xml' : '.svg'}},
                    ];
                    var handle = await window['fnSaveFilePicker'](savefilename, blob, types);
                }
                else {
                    await window['fnSaveFileLegacy'](savefilename, blob);
                }
            }
        })();
    },
    objs,
    objIndex,
    strFilename.c_str(),
    szext,
    forScanNcut);
#else
    const char* aszformats[] = {"%d", "%02d", "%03d"};
    const int paddingindex = objs < 10 ? 0 : objs < 100 ? 1 : 2;
    const char* szformat = aszformats[paddingindex];
    char szbuff64[64];
    if (objIndex == -1) {
        PesFileDialogResult result = PesSystemLoadDialog("Export SVG Files", true);
        if (result.bSuccess) {
            std::filesystem::path pathdir = std::filesystem::path(result.getPath());
            if (!std::filesystem::exists(pathdir.string())) {
                if (!std::filesystem::create_directories(pathdir.string())) {
                    SkDebugf("Could not open export file: error on create directories.\n");
                    return;
                }
            }

            for (int i = 0; i < objs; i++) {
                szbuff64[0] = 0;
                sprintf(szbuff64, szformat, i);
                std::filesystem::path path = pathdir / std::string(strFilename + "-" + szbuff64 + szext);

                SkDebugf("saving file:%s\n", path.string().c_str());

                pesBuffer buffer = pesGetDocument()->getDataObject(i)->getSVGBuffer(forScanNcut);
                if (buffer.size()) {
                    std::fstream fs;
                    fs.open(path.string(), std::ios::out | std::ios::binary);
                    fs.write(buffer.getData(), buffer.size());
                }
            }
        }
    } else {
        szbuff64[0] = 0;
        sprintf(szbuff64, szformat, objIndex);
        strFilename = strFilename + "-" + szbuff64 + szext;

        PesFileDialogResult result = PesSystemSaveDialog(strFilename.c_str(), "Export SVG File");
        if (result.bSuccess) {
            std::filesystem::path path = std::filesystem::path(result.getPath());
            if (path.has_parent_path()) {
                std::string str_path_dir = path.parent_path().string();
                if (!std::filesystem::exists(str_path_dir)) {
                    if (!std::filesystem::create_directories(str_path_dir)) {
                        SkDebugf("Could not open export file: error on create directories.\n");
                        return;
                    }
                }
            }

            SkDebugf("saving file:%s\n", path.string().c_str());

            pesBuffer buffer = pesGetDocument()->getDataObject(objIndex)->getSVGBuffer(forScanNcut);
            if (buffer.size()) {
                std::fstream fs;
                fs.open(path.string(), std::ios::out | std::ios::binary);
                fs.write(buffer.getData(), buffer.size());
            }
        }
    }
#endif
}

void PES5_AddPesData(pesData data){
    ImportCommand command(&data);
    command.execute();
    PES5_UndoRedoPushCommand(command);
}

void PES5_AddPPEFText(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           var url = globalThis['resourcePath'] + '/PPEF/Thai001.ppef';
           fetch(url)
           .then((response) => response.arrayBuffer())
           .then((buffer) => {
               //console.log('ppef loaded');
               // open ppef
               var uints = new Uint8Array(buffer);
               var defaultPPEF = new window['SQLJS']();
               defaultPPEF['openPPEF'](uints);
               defaultPPEF['readPPEFConfig']();
               
               // query
               var text = "ภิญญ์จักรปัก";
               if( window['pes_data'] && window['pes_data']['language'] == 'en' ) {
                    text = "Hello World";           
               }
               var shapes = defaultPPEF['getStringAsShapes'](text, 0, 0);
               if(shapes['size']()){
                   var pes = CanvasKit['pesFactory']['MakePesData']();
                   pes['clear']();
                   pes['paths'] = shapes;
                   var param = CanvasKit['pesFactory']['MakeParam']();
                   param['setType'](CanvasKit['PesObjectType']['OBJECT_TYPE_SCALABLE_PPEF_TEXT']);
                   param['text'] = text;
                   param['isPPEFPath'] = true;
                   param['fontName'] = "Thai001"; //"PPEF/Thai001.ppef";
                   param['fillColorIndex'] = 11;
                   param['fillType'] = CanvasKit['PesFillType']['FILL_TYPE_SATIN_COLUMN'];
                   param['strokeType'] = CanvasKit['PesStrokeType']['STROKE_TYPE_NONE'];
                   pes['parameter'] = param;
                   const unit_per_mm = 10;
                   var s = (1/300) * (pes['parameter']['fontSize']*unit_per_mm);
                   pes['scale'](s, s);
                   
                   // reset cumulative scale
                   param = pes['parameter'];
                   param['lastFontSize'] = param['fontSize'];
                   param['ppefScaleX'] = param['ppefScaleY'] = 1;
                   pes['parameter'] = param;

                   pes['applyPPEFFill']();
                   var center = pes['getBoundingBox']()['getCenter']();
                   pes['translate'](-center.x, - center.y);
                   
//                   var cnt = CanvasKit['pesDocument']['getInstance']()['addObject'](pes);
//                   if(cnt){
//                       CanvasKit['PES5Template']['UpdateImage'](cnt-1);
//                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
//                   }
                   CanvasKit['PES5Template']['AddPesData'](pes);
               }
           });
    );
#else
    pesData pes;
    pes.parameter.fontName = "Thai001";
    std::string filePath = PES5_GetResourcePath("PPEF/" + pes.parameter.fontName + ".ppef");
    std::string str = "ภิญญ์จักรปัก";
    pes.parameter.text = str.c_str();
    pes.parameter.fillColorIndex = 11; // Deep Gold
    PPEF_Reader defaultPPEF(filePath.c_str());
    defaultPPEF.readPPEFConfig();
    std::vector<pesPath> shapes = defaultPPEF.getStringAsShapes(pes.parameter.text, 1.0, 0, 0);
    if(shapes.size()){
        pes.paths = shapes;
        pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT);
        pes.parameter.fillType = pesData::FILL_TYPE_SATIN_COLUMN;
        pes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
        const int unit_per_mm = 10;
        float s = (1.0/300.0) * (pes.parameter.fontSize*unit_per_mm);
        pes.scale(s, s);
        pes.parameter.lastFontSize = pes.parameter.fontSize;
        pes.parameter.ppefScaleX = pes.parameter.ppefScaleY = 1.0;
        pes.applyPPEFFill();
        pesVec2f center = pes.getBoundingBox().getCenter();
        if(center.x!=0.0 || center.y!=0){
            pes.translate(-center.x, -center.y);
        }
//        int cnt = pesGetDocument()->addObject(pes);
//        if(cnt){
//            PES5_UpdatePesImage(cnt-1);
//            PES5_UpdateGUIRightPanel();
//        }
        PES5_AddPesData(pes);
    }
#endif
}

void PES5_AddTTFText(){
#ifdef __EMSCRIPTEN__
    int en = EM_ASM_INT({
        return window['pes_data']['language'] == 'en' ? 1 : 0;
    }, 100);
#else
    int en = 0;
#endif
    pesData pes;
    sk_sp<SkTypeface> typeface = sk_ui::GetDefaultTypeface();
    const int ptSize = pes.parameter.fontSize * 10; // 8 * 10
    pes.parameter.fontName = sk_ui::GetDefaultTypefaceName(); //"JS-Boaboon";
    std::string str = "ภิญญ์จักรปัก";
    if( en ) {
        str = "Hello World";
    }
    pes.parameter.text = str.c_str();
    pes.parameter.fillColorIndex = 11; // Deep Gold
    pes.parameter.colorIndex = 23; // Dark Grey
    SkFont font(typeface, ptSize);
    SkPath path;
    SkTextUtils::GetPath(str.c_str(), str.length(), SkTextEncoding::kUTF8, 0, 0, font, &path);
    pesPath pes_path = toPes(path);
    pes_path.setFilled(true);
    pes_path.setStrokeWidth(2);
    pes_path.setFillColor(pesGetBrotherColor(pes.parameter.fillColorIndex));
    pes_path.setStrokeColor(pesGetBrotherColor(pes.parameter.colorIndex));
    pes.paths.push_back(pes_path);
    
    pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_TTF_TEXT);
    pes.parameter.isTTFPath = true;
    pes.parameter.lastFontSize = pes.parameter.fontSize;
    pes.parameter.ppefScaleX = pes.parameter.ppefScaleX;
    
    pesVec2f center = pes.getBoundingBox().getCenter();
    if(center.x!=0 || center.y!=0){
        pes.translate(-center.x, -center.y);
    }
//    int cnt = pesGetDocument()->addObject(pes);
//    if(cnt){
//        PES5_UpdatePesImage(cnt-1);
//        PES5_UpdateGUIRightPanel();
//    }
    PES5_AddPesData(pes);
}

void PES5_AddPPEFText_V2(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           var url = globalThis['resourcePath'] + '/PPEF2/M001.ppef2';
           fetch(url)
           .then((response) => response.arrayBuffer())
           .then((buffer) => {
               //console.log('ppef2 loaded');
               // open ppef
               var uints = new Uint8Array(buffer);
               var defaultPPEF = new window['SQLJS']();
               defaultPPEF['openPPEF'](uints);
               defaultPPEF['readPPEFConfig']();

               // query
               var text = "~";
               var colors = CanvasKit['pesFactory']['MakeParam']().colors;
               var shapes = defaultPPEF['getStringAsShapesAndColors'](text, colors,0, 0);
               if(shapes['size']()){
                   var pes = CanvasKit['pesFactory']['MakePesData']();
                   pes['clear']();
                   pes['paths'] = shapes;
                   var param = CanvasKit['pesFactory']['MakeParam']();
                   param['setType'](CanvasKit['PesObjectType']['OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2']);
                   param['text'] = text;
                   param['isPPEFPath'] = true;
                   param['fontName'] = "M001";
                   param['fillColorIndex'] = 11;
                   param['fillType'] = CanvasKit['PesFillType']['FILL_TYPE_SATIN_COLUMN'];
                   param['strokeType'] = CanvasKit['PesStrokeType']['STROKE_TYPE_NONE'];
                   param['colors'] = colors;
                   pes['parameter'] = param;
                   const unit_per_mm = 10;
                   var s = (1/300) * (pes['parameter']['fontSize']*unit_per_mm);
                   pes['scale'](s, s);

                   // reset cumulative scale
                   param = pes['parameter'];
                   param['lastFontSize'] = param['fontSize'];
                   param['ppefScaleX'] = param['ppefScaleY'] = 1;
                   pes['parameter'] = param;

                   pes['applyPPEF_V2_Fill']();
                   var center = pes['getBoundingBox']()['getCenter']();
                   pes['translate'](-center.x, - center.y);

//                   var cnt = CanvasKit['pesDocument']['getInstance']()['addObject'](pes);
//                   if(cnt){
//                       CanvasKit['PES5Template']['UpdateImage'](cnt-1);
//                       CanvasKit['PES5Template']['UpdateGUIRightPanel']();
//                   }
                   CanvasKit['PES5Template']['AddPesData'](pes);
               }
               defaultPPEF['closePPEF']();
           });
    );
#else
    pesData pes;
    pes.parameter.fontName = "M001";
    std::string filePath = PES5_GetResourcePath("PPEF2/" + pes.parameter.fontName + ".ppef2");
    std::string str = "~";
    pes.parameter.text = str.c_str();
    PPEF_Reader defaultPPEF(filePath.c_str());
    defaultPPEF.readPPEFConfig();
    std::vector<pesPath> shapes = defaultPPEF.getStringAsShapesAndColors(pes.parameter.text, pes.parameter.colors, 1.0, 0, 0);
    if(shapes.size()){
        pes.paths = shapes;
        pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2);
        pes.parameter.fillType = pesData::FILL_TYPE_SATIN_COLUMN;
        pes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
        const int unit_per_mm = 10;
        float s = (1.0/300.0) * (pes.parameter.fontSize*unit_per_mm);
        pes.scale(s, s);
        pes.parameter.lastFontSize = pes.parameter.fontSize;
        pes.parameter.ppefScaleX = pes.parameter.ppefScaleY = 1.0;
        pes.applyPPEF_V2_Fill();
        pesVec2f center = pes.getBoundingBox().getCenter();
        if(center.x!=0.0 || center.y!=0){
            pes.translate(-center.x, -center.y);
        }
//        int cnt = pesGetDocument()->addObject(pes);
//        if(cnt){
//            PES5_UpdatePesImage(cnt-1);
//            PES5_UpdateGUIRightPanel();
//        }
        PES5_AddPesData(pes);
    }
#endif
}


pesEffect * CreateEffect(pesData::Parameter parameter){
    float angle = parameter.angleValue;
    float radius = parameter.radiusValue;
    int baselineIndex = parameter.shapeIndex;
    bool bItalic = parameter.italic;
    bool bCreateBorder = parameter.border;
    pesEffect *effect = NULL;
    
      if(baselineIndex==0){
          effect = new pesEffectNormal();
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==1){
          effect = new pesEffectArchTop();
//          ((pesEffectArchTop*)effect)->angle = angle;
//          ((pesEffectArchTop*)effect)->radius = radius;
          ((pesEffectArchTop*)effect)->angle = parameter.angleValue;
          ((pesEffectArchTop*)effect)->radius = parameter.radiusValue;
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==2){
          effect = new pesEffectArchBottom();
//          ((pesEffectArchBottom*)effect)->angle = angle;
//          ((pesEffectArchBottom*)effect)->radius = radius;
          ((pesEffectArchTop*)effect)->angle = parameter.angleValue;
          ((pesEffectArchTop*)effect)->radius = parameter.radiusValue;
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==3){
          effect = new pesEffectCircle();
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==4){
          effect = new pesEffectSineWave();
          ((pesEffectSineWave*)effect)->magnitude = parameter.waveMagnitude;
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==5){
          bool bChevronUp = true;
          effect = new pesEffectChevron(bChevronUp);
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==6){
          bool bChevronUp = false;
          effect = new pesEffectChevron(bChevronUp);
          effect->borderGap = 100 + parameter.borderGap;
          effect->borderGapY = 100 + parameter.borderGapY;
      }
      else if(baselineIndex==7){
          bool bSlantUp = true;
          effect = new pesEffectSlant(bSlantUp);
          ((pesEffectSlant*)effect)->angle = parameter.slantUpAngle;
      }
      else if(baselineIndex==8){
          bool bSlantUp = false;
          effect = new pesEffectSlant(bSlantUp);
          ((pesEffectSlant*)effect)->angle = parameter.slantDownAngle;
      }
      else if(baselineIndex==9){
          effect= new pesEffectTriangleUp();
      }
      else if(baselineIndex==10){
          effect= new pesEffectTriangleDown();
      }
      else if(baselineIndex==11){
          effect = new pesEffectFadeRight();
      }
      else if(baselineIndex==12){
          effect= new pesEffectFadeLeft();
      }
      else if(baselineIndex==13){
          effect = new pesEffectFadeUp();
          ((pesEffectFadeUp*)effect)->slantFactor = parameter.fadeUpSlant;
      }
      else if(baselineIndex==14){
          effect = new pesEffectFadeDown();
          ((pesEffectFadeDown*)effect)->slantFactor = parameter.fadeDownSlant;
      }
      else if(baselineIndex==15){
          effect = new pesEffectInflate();
      }

    if(effect){
        effect->bItalic = bItalic;
        effect->bCreateBorder = bCreateBorder;
    }
    
    return effect;
}

void PES5_ReplacePPEFText(int objIndex){
    if(objIndex>=0 && objIndex<PES5_GetObjectCount()){
#ifdef __EMSCRIPTEN__
       EM_ASM(
          {
              if (window['updatePPEFText']) {
                  window['updatePPEFText']($0);
                  return;
              }

              var index = $0;
              var pesdata = CanvasKit['pesDocument']['getInstance']()['getDataObject'](index);
              var text = pesdata['parameter']['text']['trim']();
              var name = pesdata['parameter']['fontName'];
              var url = globalThis['resourcePath'] + '/PPEF/' + name + '.ppef';
              fetch(url)
              .then((response) => response.arrayBuffer())
              .then((buffer) => {
                  // open ppef
                  var uints = new Uint8Array(buffer);
                  var oldCenter = pesdata['getBoundingBox']()['getCenter']();
                  var ppef = new window['SQLJS']();
                  ppef['openPPEF'](uints);
                  ppef['readPPEFConfig']();
                  
                  // query
                  var shapes = ppef['getStringAsShapes'](text, pesdata['parameter']['extraLetterSpace'],pesdata['parameter']['extraSpace']);
                  if(shapes['size']()){
                    var data = pesdata;
                    var angle = data['parameter']['angleValue'];
                    var radius = data['parameter']['radiusValue'];

                    var baselineIndex = data['parameter']['shapeIndex'];
                    var bItalic = data['parameter']['italic'];
                    var bCreateBorder = data['parameter']['border'];
                    var effect = null;

                    if(baselineIndex==0){
                        effect = new CanvasKit['pesEffectNormal']();
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==1){
                        effect = new CanvasKit['pesEffectArchTop']();
//                        effect['angle'] = angle;
//                        effect['radius'] = radius;
                        effect['angle'] = data['parameter']['angleValue'];
                        effect['radius'] = data['parameter']['radiusValue'];
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==2){
                        effect = new CanvasKit['pesEffectArchBottom']();
//                        effect['angle'] = angle;
//                        effect['radius'] = radius;
                        effect['angle'] = data['parameter']['angleValue'];
                        effect['radius'] = data['parameter']['radiusValue'];
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==3){
                        effect = new CanvasKit['pesEffectCircle']();
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==4){
                        effect = new CanvasKit['pesEffectSineWave']();
                        effect['magnitude'] = data['parameter']['waveMagnitude'];
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==5){
                        var bChevronUp = true;
                        effect = new CanvasKit['pesEffectChevron'](bChevronUp);
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==6){
                        var bChevronUp = false;
                        effect = new CanvasKit['pesEffectChevron'](bChevronUp);
                        effect['borderGap'] = 100 + data['parameter']['borderGap'];
                        effect['borderGapY'] = 100 + data['parameter']['borderGapY'];
                    }
                    else if(baselineIndex==7){
                        var bSlantUp = true;
                        effect = new CanvasKit['pesEffectSlant'](bSlantUp);
                        effect['angle'] = data['parameter']['slantUpAngle'];
                    }
                    else if(baselineIndex==8){
                        var bSlantUp = false;
                        effect = new CanvasKit['pesEffectSlant'](bSlantUp);
                        effect['angle'] = data['parameter']['slantDownAngle'];
                    }
                    else if(baselineIndex==9){
                        effect= new CanvasKit['pesEffectTriangleUp']();
                    }
                    else if(baselineIndex==10){
                        effect= new CanvasKit['pesEffectTriangleDown']();
                    }
                    else if(baselineIndex==11){
                        effect = new CanvasKit['pesEffectFadeRight']();
                    }
                    else if(baselineIndex==12){
                        effect= new CanvasKit['pesEffectFadeLeft']();
                    }
                    else if(baselineIndex==13){
                        effect = new CanvasKit['pesEffectFadeUp']();
                        effect['slantFactor'] = data['parameter']['fadeUpSlant'];
                    }
                    else if(baselineIndex==14){
                        effect = new CanvasKit['pesEffectFadeDown']();
                        effect['slantFactor'] = data['parameter']['fadeDownSlant'];
                    }
                    else if(baselineIndex==15){
                        effect = new CanvasKit['pesEffectInflate']();
                    }

                    if(effect){
                      effect['bItalic'] = bItalic;
                      effect['bCreateBorder'] = bCreateBorder;
                    }
                    if(effect){
                      effect['applyPaths'](shapes);
                      if(baselineIndex==1 || baselineIndex==2){
                          var param = pesdata['parameter'];
                          param['angleValue'] = effect['angle'];
                          param['radiusValue'] = effect['radius'];
                          pesdata['parameter'] = param;
                      }
                      effect['delete']();
                    }
                      
                    pesdata['clearPaths']();
                    pesdata['paths'] = shapes;
                    const unit_per_mm = 10;
                    var s = (1/300)*(pesdata['parameter']['fontSize']*unit_per_mm);
                    var param = pesdata['parameter']['copy']();
                    if(param['lastFontSize']!=param['fontSize']){
                      param['lastFontSize'] = param['fontSize'];
                      param['ppefScaleX'] = param['ppefScaleY'] = 1.0;
                    }
                    pesdata['scale'](s*param['ppefScaleX'], s*param['ppefScaleY']);
                    pesdata['parameter'] = param;
                    var newCenter = pesdata['getBoundingBox']()['getCenter']();
                    pesdata['translate'](oldCenter['x']-newCenter['x'], oldCenter['y']-newCenter['y']);
                    CanvasKit['PES5Template']['UpdateImage'](index);
                    CanvasKit['PES5Template']['UpdateGUIRightPanel']();
                  }
                  ppef['closePPEF']();
              })
              .finally(()=>{
                  pesdata.delete();
              });
          }
          ,objIndex
       );
#else
        //Warning C26815 The pointer is dangling because it points at a temporary instance which was destroyed
        //pesData* pes = pesGetDocument()->getDataObject(objIndex).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(objIndex);
        pesData* pes = shared_ptr_pesData.get();
        std::string text = pes->parameter.text;
        std::string filePath = PES5_GetResourcePath("PPEF/" + pes->parameter.fontName + ".ppef");
        pesVec2f oldCenter = pes->getBoundingBox().getCenter();
        PPEF_Reader ppef(filePath.c_str());
        ppef.readPPEFConfig();
        std::vector<pesPath> shapes = ppef.getStringAsShapes(text, 1.0,
                                                             pes->parameter.extraLetterSpace,
                                                             pes->parameter.extraSpace);
        if(shapes.size()){
            pesEffect* effect = CreateEffect(pes->parameter);
            if (effect) {
                effect->applyPaths(shapes);
                int effectIndex = pes->parameter.shapeIndex;
                if (effectIndex == 1) {
                    pes->parameter.angleValue = ((pesEffectArchTop*)effect)->angle;
                    pes->parameter.radiusValue = ((pesEffectArchTop*)effect)->radius;
                } 
                else if (effectIndex == 2) {
                    pes->parameter.angleValue = ((pesEffectArchBottom*)effect)->angle;
                    pes->parameter.radiusValue = ((pesEffectArchBottom*)effect)->radius;
                }
                delete effect;
            }
            
            pes->paths.clear();
            pes->paths = shapes;
            const int unit_per_mm = 10;
            float s = (1.0/300.0)*(pes->parameter.fontSize*unit_per_mm);
            if(pes->parameter.lastFontSize!=pes->parameter.fontSize){
                pes->parameter.lastFontSize = pes->parameter.fontSize;
                pes->parameter.ppefScaleX = pes->parameter.ppefScaleY = 1.0;
            }
            float sx = pes->parameter.ppefScaleX; float sy = pes->parameter.ppefScaleY;
            pes->scale(s*pes->parameter.ppefScaleX, s*pes->parameter.ppefScaleY);
            pes->parameter.ppefScaleX = sx; pes->parameter.ppefScaleY = sy;
            
            pesVec2f newCenter = pes->getBoundingBox().getCenter();
            pes->translate(oldCenter.x-newCenter.x, oldCenter.y-newCenter.y);
            PES5_UpdatePesImage(objIndex);
        }
#endif
    }
}

void PES5_ReplaceTTFText(const sk_sp<SkTypeface> typeface, int objIndex){
    if(objIndex>=0 && objIndex<PES5_GetObjectCount()){
        //Warning C26815 The pointer is dangling because it points at a temporary instance which was destroyed
        //pesData* pes = pesGetDocument()->getDataObject(objIndex).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(objIndex);
        pesData* pes = shared_ptr_pesData.get();
        pesVec2f oldCenter = pes->getBoundingBox().getCenter();
        std::string str(pes->parameter.text);
        const int ptSize = pes->parameter.fontSize * 10;
        SkFont font(typeface, ptSize);
        SkPath path;
        SkTextUtils::GetPath(str.c_str(), str.length(), SkTextEncoding::kUTF8, 0, 0, font, &path);
        pesPath pes_path = toPes(path);
        pes_path.setFilled(true);
        pes_path.setStrokeWidth(2);
        pes_path.setFillColor(pes->paths[0].getFillColor());
        pes_path.setStrokeColor(pes->paths[0].getStrokeColor());
        pes->paths.clear();
        pes->paths.push_back(pes_path);
        if(pes->parameter.lastFontSize!=pes->parameter.fontSize){
            pes->parameter.lastFontSize = pes->parameter.fontSize;
            pes->parameter.ppefScaleX = pes->parameter.ppefScaleY = 1.f;
        }

        // backup cumulative scale value inside pesData
        float sx = pes->parameter.ppefScaleX; float sy = pes->parameter.ppefScaleY;
        // then scale pesData
        pes->scale(pes->parameter.ppefScaleX, pes->parameter.ppefScaleY);
        // rollback cumulative scale value inside pesData
        pes->parameter.ppefScaleX = sx; pes->parameter.ppefScaleY = sy;

        //fixed: update stroke-type dynamic
        pes->scale(1.f, 1.f);

        pesVec2f newCenter = pes->getBoundingBox().getCenter();
        pes->translate(oldCenter.x-newCenter.x, oldCenter.y-newCenter.y);
        
        PES5_UpdatePesImage(objIndex);
    }
}

void PES5_ReplacePPEFText_V2(int objIndex){
    if(objIndex>=0 && objIndex<PES5_GetObjectCount()){
#ifdef __EMSCRIPTEN__
       EM_ASM(
          {
              var index = $0;
              var pesdata = CanvasKit['pesDocument']['getInstance']()['getDataObject'](index);
              var text = pesdata['parameter']['text']['trim']();
              var name = pesdata['parameter']['fontName'];
              var url = globalThis['resourcePath'] + '/PPEF2/' + name + '.ppef2';
              fetch(url)
              .then((response) => response.arrayBuffer())
              .then((buffer) => {
                  // open ppef
                  var uints = new Uint8Array(buffer);
                  var oldCenter = pesdata['getBoundingBox']()['getCenter']();
                  var ppef = new window['SQLJS']();
                  ppef['openPPEF'](uints);
                  ppef['readPPEFConfig']();

                  // query
                  var colors = CanvasKit['pesFactory']['MakeParam']().colors;
                  
                  var shapes = ppef['getStringAsShapesAndColors'](text,
                                                                  colors,
                                                                  pesdata['parameter']['extraLetterSpace'],
                                                                  pesdata['parameter']['extraSpace']);
                  
                  if(shapes['size']()){
                    pesdata['clearPaths']();
                    pesdata['paths'] = shapes;
                    const unit_per_mm = 10;
                    var s = (1/300)*(pesdata['parameter']['fontSize']*unit_per_mm);
                      
//                    // not work
//                    pesdata['parameter']['colors'] = colors;
                      
                    // work like a charm
                    var param0 = pesdata['parameter'];
                    param0['colors'] = colors;
                    pesdata['parameter'] = param0;
                      
                    var param = pesdata['parameter']['copy']();
                    if(param['lastFontSize']!=param['fontSize']){
                      param['lastFontSize'] = param['fontSize'];
                      param['ppefScaleX'] = param['ppefScaleY'] = 1.0;
                    }
//                    console.log('(EMASM)Color size ' + pesdata['parameter']['colors']['size']());
//                    var n = pesdata['parameter']['colors']['size']();
//                    for(var i=0; i<n; i++){
//                        console.log(pesdata['parameter']['colors']['get'](i) + ",");
//                    }
//                    console.log('end(EMASM)');
                    pesdata['scale'](s*param['ppefScaleX'], s*param['ppefScaleY']);
                    pesdata['parameter'] = param;
                    var newCenter = pesdata['getBoundingBox']()['getCenter']();
                    pesdata['translate'](oldCenter['x']-newCenter['x'], oldCenter['y']-newCenter['y']);
                    CanvasKit['PES5Template']['UpdateImage'](index);
                  }
                  
                  ppef['closePPEF']();
                  
              })
              .finally(()=>{
                  pesdata.delete();
              });
          }
          ,objIndex
       );
#else
        //Warning C26815 The pointer is dangling because it points at a temporary instance which was destroyed
        //pesData* pes = pesGetDocument()->getDataObject(objIndex).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(objIndex);
        pesData* pes = shared_ptr_pesData.get();
        std::string text = pes->parameter.text;
        std::string filePath = PES5_GetResourcePath("PPEF2/" + pes->parameter.fontName + ".ppef2");
        pesVec2f oldCenter = pes->getBoundingBox().getCenter();
        PPEF_Reader ppef(filePath.c_str());
        ppef.readPPEFConfig();
        std::vector<pesPath> shapes = ppef.getStringAsShapesAndColors(text,
                                                             pes->parameter.colors,
                                                             1.0,
                                                             pes->parameter.extraLetterSpace,
                                                             pes->parameter.extraSpace);
        if(shapes.size()){
            pes->paths.clear();
            pes->paths = shapes;
            const int unit_per_mm = 10;
            float s = (1.0/300.0)*(pes->parameter.fontSize*unit_per_mm);
            if(pes->parameter.lastFontSize!=pes->parameter.fontSize){
                pes->parameter.lastFontSize = pes->parameter.fontSize;
                pes->parameter.ppefScaleX = pes->parameter.ppefScaleY = 1.0;
            }
            float sx = pes->parameter.ppefScaleX; float sy = pes->parameter.ppefScaleY;
            pes->scale(s*pes->parameter.ppefScaleX, s*pes->parameter.ppefScaleY);
            pes->parameter.ppefScaleX = sx; pes->parameter.ppefScaleY = sy;
            
            pesVec2f newCenter = pes->getBoundingBox().getCenter();
            pes->translate(oldCenter.x-newCenter.x, oldCenter.y-newCenter.y);
            PES5_UpdatePesImage(objIndex);
        }
#endif
    }
}

std::string PES5_ObjectTypeToString(int type){
    if(type==pesData::OBJECT_TYPE_PES2_TEXT || type==pesData::OBJECT_TYPE_PES || type==pesData::OBJECT_TYPE_SHAPE){
        return "Stitch";
    }
    else if(type==pesData::OBJECT_TYPE_BACKGROUND){
        return "Background";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_SVG_FILE){
        return "SVG";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT){
        return "PPEF Text";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_TTF_TEXT){
        return "TTF Text";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN){
        return "Satin Column";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2){
        return "Monogram";
    }
    else if(type==pesData::OBJECT_TYPE_SCALABLE_CONTAINER){
        return "Group";
    }
    return "Unknow";
}

int PES5_GetObjectCount(){
    return pesGetDocument()->getObjectCount();
}

void PES5_UpdateGUIRightPanel(){
    sk_ui::UpdateGUIRightPanel();
}

void PES5_LayerMoveUp(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        SendBackwardCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_LayerMoveDown(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        BringForwardCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_LayerMerge(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        MergeCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_LayerBreak(){
    //const int COLORS_LIMIT = 8;
    const int COLORS_LIMIT = 16;
    int index = PES5_GetObjectSelectedIndex();
    if((index!=-1) && (index<PES5_GetObjectCount())){
        //Warning C26815 The pointer is dangling because it points at a temporary instance which was destroyed
        //pesData* pes = pesGetDocument()->getDataObject(index).get();
        auto shared_ptr_pesData = pesGetDocument()->getDataObject(index);
        pesData* pes = shared_ptr_pesData.get();
        bool canBreak = (pes->parameter.type==pesData::OBJECT_TYPE_PES && pes->fillBlocks.size()<=COLORS_LIMIT);
        if(canBreak){
            BreakCommand command(index);
            command.execute();
            PES5_UndoRedoPushCommand(command);
        }
    }
}

void PES5_SelectLayerList( int index ) {
    sk_ui::GetLayerList()->select_listItem(index);
}

void PES5_LayerDuplicate(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        DuplicateCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_LayerDelete(){
//    int index = PES5_GetObjectSelectedIndex();
//    if(pesInRange(index, 0, PES5_GetObjectCount())){
//        DeleteCommand command(index);
//        command.execute();
//        PES5_UndoRedoPushCommand(command);
//    }
    
    PES5_MSDelete();
}

bool PES5_ColorBlockSwap(pesData * pes, int index1, int index2){
    if(index1>=0 && index1<pes->fillBlocks.size() &&
       index2>=0 && index2<pes->fillBlocks.size() &&
       index1!=index2){
        ColorBlockSwapCommand command(pes, index1, index2);
        command.execute();
        PES5_UndoRedoPushCommand(command);
        return true;
    }
    return false;
    
}

void PES5_HorizontalFlip(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        HorizontalFlipCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_VerticalFlip(){
    int index = PES5_GetObjectSelectedIndex();
    if(pesInRange(index, 0, PES5_GetObjectCount())){
        VertitalFlipCommand command(index);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_LoadOrDownloadAsset(const char* path, std::function<void(sk_sp<SkData> data)> onLoaded){
#ifdef __EMSCRIPTEN__
    struct lamda_t{
        std::function<void(sk_sp<SkData> data)> onLoaded;
    };
    lamda_t* plamda = new lamda_t();
    plamda->onLoaded = onLoaded;
    emscriptenDownloadAsset( plamda, path,
        [](struct emscripten_fetch_t *fetch) {
            lamda_t* plamda = (lamda_t*)fetch->userData;
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            if(data->size()){
                plamda->onLoaded(data);
            }
            delete plamda;
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "PES5_LoadOrDownloadAsset() Download failed\n" );
            lamda_t* plamda = (lamda_t*)fetch->userData;
            delete plamda;
        }
    );
#else
    sk_sp<SkData> data = GetResourceAsData(path);
    if(data && data->size()){
        onLoaded(data);
    }
#endif
}


void PES5_TracingPNGFile(){
#ifdef __EMSCRIPTEN__
    EM_ASM(
           let input = document.createElement('input');
           input.type = 'file';
        //    input.accept = 'image/png,.png';
           input.accept = 'image/png, .png, image/jpg, .jpg, image/jpeg, image/bmp, .bmp';
           input.onchange = function(){
               var pesDoc = CanvasKit['pesDocument']['getInstance']();
               var f = input.files[0];
               console.debug("filename:", f.name);
               var r = new FileReader();
               r.onload = function () {
                   var data = new Uint8Array(r.result);
                   var pbuff = CanvasKit['pesFactory']['MakeBuffer']();
                   pbuff['set'](data);
                   CanvasKit['PES5Template']['OpenTracingDialog'](pbuff);
                   pbuff['delete']();
               };
               r.readAsArrayBuffer(f);
           };
           
           input.click();
    );
#else
        std::vector<std::string> accept;
        accept.push_back("png");
        accept.push_back("jpg");
        accept.push_back("jpeg");
        accept.push_back("bmp");
        PesFileDialogResult result = PesSystemLoadDialog("Tracing PNG File", false, "", accept);
        if(result.bSuccess){
            //SkDebugf("filename:%s\n", result.fileName.c_str());
            sk_sp<SkData> data = SkData::MakeFromFileName(result.getPath().c_str());
            PES5_OpenTracingDialog(data);
        }
#endif
}

void PES5_OpenTracingDialog(sk_sp<SkData> data){
    if(data && data->size()){
        SkBitmap bitmap;
        if(DecodeDataToBitmap(data, &bitmap)){
            sk_ui::GetPesTracingDialog()->setBitmap(bitmap);
            sk_ui::GetPesTracingDialog()->resetParameter();
            sk_ui::GetPesTracingDialog()->onApply();
            sk_ui::GetPesTracingDialog()->show();
        }
    }
}

bool __satin_column_started = false;
PesSatinColumn __satin_column;
void PES5_StartSatinColumnInput(){
    SkDebugf( "Start Satin Column\n" );
    if(__satin_column_started==false){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Satin Column'); }");
#endif
        //sk_ui::GetPesDocumentView()->setSelectedIndex(-1); // deselect
        PES5_SetObjectSelectedIndex(-1); // deselect
        __satin_column_started = true;
        __satin_column.reset();
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "crosshair");
    }
}

void PES5_StopSatinColumnInput(){
//    SkDebugf( "Stop Satin Column\n" );
    if(__satin_column_started){
        SkDebugf( "__satin_column_started\n" );
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __satin_column_started = false;
        auto csp = __satin_column.csp;
        if(csp[0].knots.size()>=2 && csp[0].knots.size()==csp[1].knots.size()){
            pesData pes;
            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN);
            pes.parameter.isSatinColumnPath = true;
            pes.parameter.fillType = pesData::FILL_TYPE_SATIN_COLUMN;
            pes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
            pes.parameter.fillColorIndex = 11; // Deep gold
            csp[0].path.setFillColor(pesGetBrotherColor(pes.parameter.fillColorIndex));
            csp[1].path.setFillColor(pesGetBrotherColor(pes.parameter.fillColorIndex));
            csp[0].path.setStrokeWidth(2.0);
            csp[1].path.setStrokeWidth(2.0);
            csp[0].path.setStrokeHexColor(0x6494ed);
            csp[1].path.setStrokeHexColor(0xffa500);
            pes.paths.push_back(csp[0].path);
            pes.paths.push_back(csp[1].path);
            pes.applyFill();
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
            PES5_AddPesData(pes);
        }
    }
    __satin_column.reset();
    sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
}

bool PES5_IsSatinColumnInputStarted(){
    return __satin_column_started;
}

void PES5_DrawSatinColumnInput(SkCanvas & canvas){
    __satin_column.draw(canvas);
}

void PES5_SatinColumnAddCornerPoint(float x, float y){
    __satin_column.addCornerPoint(x, y);
}

void PES5_SatinColumnAddCurvePoint(float x, float y){
    __satin_column.addCurvePoint(x, y);
}

void PES5_SatinColumnRemovePreviousPoint(){
    __satin_column.removePreviousPoint();
}


bool __drawing_started = false;
std::vector<pesPath> __drawing_paths;
void PES5_StartDrawingInput(){
    if(__satin_column_started==false){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Drawing'); }");
#endif
        //sk_ui::GetPesDocumentView()->setSelectedIndex(-1);  // deselect
        PES5_SetObjectSelectedIndex(-1);
        __drawing_started = true;
        __drawing_paths.clear();
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "crosshair");
    }
}

void PES5_StopDrawingInput(){
    if(__drawing_started){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __drawing_started = false;
        if(__drawing_paths.size()){
            pesData pes;
            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
            pes.paths.clear();
            pes.paths = __drawing_paths;
            for(auto & p : pes.paths){
                p.setStrokeWidth(2.0);
                p.setStrokeHexColor(0x000000);
                p.setFillHexColor(0x4FB4DE);
                if(p.getCommands().back().type==pesPath::Command::_close){
                    p.setFilled(true);
                }
            }
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
            PES5_AddPesData(pes);
        }
    }
    __drawing_paths.clear();
    sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
}

bool PES5_IsDrawingInputStarted(){
    return __drawing_started;
}

void PES5_DrawingInputDraw(SkCanvas & canvas){
    SkPaint paint;
    paint.setStrokeWidth(3);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setAntiAlias(true);
    paint.setARGB(255, 0, 0, 255);
    
    for(auto & p : __drawing_paths){
        SkPath skpath = toSk(p);
        canvas.drawPath(skpath, paint);
    }
}

pesVec2f __first_point;
void PES5_DrawingInputAddFirstPoint(const pesVec2f & p){
    __first_point.set(p);
    __drawing_paths.emplace_back();
    __drawing_paths.back().moveTo(p.x, p.y);
}

void PES5_DrawingInputAddPoint(const pesVec2f & p){
    __drawing_paths.back().lineTo(p.x, p.y);
}

void PES5_DrawingInputAddLastPoint(const pesVec2f & p){
    float CLOSE_TOLERANCE = 25; // 25mm
    float l = __first_point.distance(p);
    __drawing_paths.back().lineTo(p.x, p.y);
    bool isClose = false;
    if(l<CLOSE_TOLERANCE){
        __drawing_paths.back().close();
        isClose = true;
    }
    
    // smooth
    if(__drawing_paths.back().getOutline().size()==1){
        pesPolyline ps = __drawing_paths.back().getOutline()[0].getSmoothed(5, 0.5);
        ps.simplify();
        __drawing_paths.back().clear();
        for(int i=0; i<ps.size(); i++){
            if(i==0)
                __drawing_paths.back().moveTo(ps.getVertices()[i].x, ps.getVertices()[i].y);
            else
                __drawing_paths.back().lineTo(ps.getVertices()[i].x, ps.getVertices()[i].y);
        }
        if(isClose)
            __drawing_paths.back().close();
    }
}

// PEN (Lines Tool)
bool __lines_tool_started = false;
PesLines __pes_lines;
void PES5_StartLinesToolInput(){
    SkDebugf( "Start Lines Tool\n" );
    if(__lines_tool_started==false){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Line'); }");
#endif
        //sk_ui::GetPesDocumentView()->setSelectedIndex(-1);  // deselect
        PES5_SetObjectSelectedIndex(-1);
        __lines_tool_started = true;
        __pes_lines.reset();
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "crosshair");
    }
}

void PES5_StopLinesToolInput(){
    if(__lines_tool_started){
        SkDebugf( "__lines_tool_started\n" );
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __lines_tool_started = false;
        auto csp = __pes_lines.csp;
        if(csp.knots.size()>=2){
            pesData pes;
            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
            pes.parameter.fillType = pesData::FILL_TYPE_NONE;
            pes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
            pes.parameter.colorIndex = 20; // Black
            csp.path.setStrokeColor(pesGetBrotherColor(pes.parameter.colorIndex));
            csp.path.setStrokeWidth(2.0);
            csp.path.setFilled(false);
            pes.paths.push_back(csp.path);
//            pes.applyStroke();
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
            PES5_AddPesData(pes);
        }
    }
    __pes_lines.reset();
    sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
}

bool PES5_IsLinesToolInputStarted(){
    return __lines_tool_started;
}

void PES5_LinesToolDraw(SkCanvas & canvas){
    __pes_lines.draw(canvas);
}

void PES5_LinesToolAddCornerPoint(float x, float y){
    __pes_lines.addCornerPoint(x, y);
}

void PES5_LinesToolAddCurvePoint(float x, float y){
    __pes_lines.addCurvePoint(x, y);
}

void PES5_LinesToolRemovePreviousPoint(){
    __pes_lines.removePreviousPoint();
}


// Polygon (Polygon Tool)
bool __polygon_tool_started = false;
PesPolygon __pes_polygon;
void PES5_StartPolygonToolInput(){
    SkDebugf( "Start Polygon Tool\n" );
    if(__polygon_tool_started==false){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Polygon'); }");
#endif
        //sk_ui::GetPesDocumentView()->setSelectedIndex(-1);  // deselect
        PES5_SetObjectSelectedIndex(-1);
        __polygon_tool_started = true;
        __pes_polygon.reset();
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "crosshair");
    }
}

void PES5_StopPolygonToolInput(){
    if(__polygon_tool_started){
        SkDebugf( "__polygon_tool_started\n" );
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __polygon_tool_started = false;
        auto csp = __pes_polygon.csp;
        if(csp.knots.size()>=3){
            pesData pes;
            pes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
            pes.parameter.fillType = pesData::FILL_TYPE_NONE;
            pes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
//            pes.parameter.colorIndex = 20; // Black
            csp.path.close();
            csp.path.setStrokeWidth(2.0);
            csp.path.setStrokeHexColor(0x000000);
            csp.path.setFillHexColor(0x4FB4DE);
            csp.path.setFilled(true);
            pes.paths.push_back(csp.path);
//            int cnt = pesGetDocument()->addObject(pes);
//            if(cnt){
//                PES5_UpdatePesImage(cnt-1);
//                PES5_UpdateGUIRightPanel();
//            }
            PES5_AddPesData(pes);
        }
    }
    __pes_polygon.reset();
    sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
}

bool PES5_IsPolygonToolInputStarted(){
    return __polygon_tool_started;
}

void PES5_PolygonToolDraw(SkCanvas & canvas){
    __pes_polygon.draw(canvas);
}

void PES5_PolygonToolAddCornerPoint(float x, float y){
    __pes_polygon.addCornerPoint(x, y);
}

void PES5_PolygonToolAddCurvePoint(float x, float y){
    __pes_polygon.addCurvePoint(x, y);
}

void PES5_PolygonToolRemovePreviousPoint(){
    __pes_polygon.removePreviousPoint();
}


// Path Edit
bool __path_edit_started = false;
//PesPathEdit __pes_path_edit;
int __pe_previousMouseX = -1;
int __pe_previousMouseY = -1;
std::vector<pesPath> oldPaths, newPaths;
bool __pathChanged = false;
void PES5_StartPathEditInput(){
//    SkDebugf( "Start Path Edit\n" );
    int selectedIndex = PES5_GetObjectSelectedIndex();
    if(__path_edit_started==false && pesInRange(selectedIndex, 0, pesGetDocument()->getObjectCount())
       && pesGetDocument()->getDataObject(selectedIndex).get()->paths.size()){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Path Edit'); }");
#endif
        __path_edit_started = true;
        __pes_path_edit.reset();
        __pes_path_edit.pesData = pesGetDocument()->getDataObject(selectedIndex).get(); // capture
        __pes_path_edit.paths = __pes_path_edit.pesData->paths; // copy
        __pes_path_edit.center = __pes_path_edit.pesData->getBoundingBox().getCenter(); //copy
        __pes_path_edit.rotateDegree = __pes_path_edit.pesData->parameter.rotateDegree; // copy
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
    }
    __pe_previousMouseX = -1;
    __pe_previousMouseY = -1;
    oldPaths.clear();
    newPaths.clear();
    __pathChanged = false;
}

void PES5_StopPathEditInput(){
    if(__path_edit_started){
        SkDebugf( "__path_edit_started\n" );
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __path_edit_started = false;
        if(__pes_path_edit.pesData!=NULL){
            __pes_path_edit.pesData->paths.clear();
            __pes_path_edit.pesData->paths = __pes_path_edit.paths;
            auto type = __pes_path_edit.pesData->parameter.type;
//            if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT || type==pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN){
            if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT){
                __pes_path_edit.pesData->applyPPEFFill();
            }
            else if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2){
                __pes_path_edit.pesData->applyPPEF_V2_Fill();
            }
            else{
                __pes_path_edit.pesData->applyFill();
            }
            __pes_path_edit.pesData->applyStroke();
            for(int i=0; i<pesGetDocument()->getObjectCount(); i++){
                if(pesGetDocument()->getDataObject(i).get() == __pes_path_edit.pesData){
                    PES5_UpdatePesImage(i);
                    PES5_UpdateGUIRightPanel();
                    break;
                }
            }
        }
    }
    __pes_path_edit.reset();
    __pe_previousMouseX = -1;
    __pe_previousMouseY = -1;
    oldPaths.clear();
    newPaths.clear();
    __pathChanged = false;
}

bool PES5_IsPathEditInputStarted(){
    return __path_edit_started;
}

void PES5_PathEditDraw(SkCanvas & canvas){
    __pes_path_edit.draw(canvas);
}

pesVec2f *__grab_cp_ptr = NULL;
//bool PES5_PathEditMousePressed(int ix, int iy){
//    bool interacted = false;
//    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
//    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_path_edit.center, __pes_path_edit.rotateDegree);
//
//    __grab_cp_ptr = NULL;
//    const float s = 6;
//    const float r = s/2;
//    int pathNum = (int)__pes_path_edit.paths.size();
//    for(int i=pathNum-1; i>=0; i--){
//        int cmdNum = (int)__pes_path_edit.paths[i].getCommands().size();
//        pesPath *path = &__pes_path_edit.paths[i];
//        for(int j=cmdNum-1; j>=0; j--){
//            pesPath::Command * cmd = &path->getCommands()[j];
//            pesRectangle rect;
//            rect.setFromCenter(cmd->to.x, cmd->to.y, s, s);
//            if(rect.inside(p.x, p.y)){
//                interacted = true;
//                __pes_path_edit.selectedObject.reset();
//                __pes_path_edit.selectedObject.node.path_ptr = path;
//                __pes_path_edit.selectedObject.node.cmd_ptr = (pesVec2f*)&cmd->to;
//                if(j>0 && (cmd->type==pesPath::Command::_bezierTo || cmd->type==pesPath::Command::_quadBezierTo)){
//                    __pes_path_edit.selectedObject.node.prev_cp_ptr = (pesVec2f*)&cmd->cp2;
//                }
//                if(j<cmdNum-1 && (path->getCommands()[j+1].type==pesPath::Command::_bezierTo || path->getCommands()[j+1].type==pesPath::Command::_quadBezierTo)){
//                    __pes_path_edit.selectedObject.node.next_cp_ptr = (pesVec2f*)&path->getCommands()[j+1].cp1;
//                }
//                break;
//            }
//        }
//    }
//
//    if(!interacted && __pes_path_edit.selectedObject.node.cmd_ptr){
//        if(__pes_path_edit.selectedObject.node.next_cp_ptr){
//            pesVec2f cp = *__pes_path_edit.selectedObject.node.next_cp_ptr;
//            pesRectangle rect;
//            rect.setFromCenter(cp.x, cp.y, s, s);
//            if(rect.inside(p.x, p.y)){
//                interacted = true;
//                __grab_cp_ptr = __pes_path_edit.selectedObject.node.next_cp_ptr;
//            }
//        }
//
//        if(!interacted && __pes_path_edit.selectedObject.node.prev_cp_ptr){
//            pesVec2f cp = *__pes_path_edit.selectedObject.node.prev_cp_ptr;
//            pesRectangle rect;
//            rect.setFromCenter(cp.x, cp.y, s, s);
//            if(rect.inside(p.x, p.y)){
//                interacted = true;
//                __grab_cp_ptr = __pes_path_edit.selectedObject.node.prev_cp_ptr;
//            }
//        }
//    }
//
//    if(interacted){
//        __pe_previousMouseX = ix;
//        __pe_previousMouseY = iy;
//    }
//    else{
//        __pes_path_edit.selectedObject.reset();
//    }
//    return interacted;
//}
bool PES5_PathEditMousePressed(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_path_edit.center, __pes_path_edit.rotateDegree);
    
    __grab_cp_ptr = NULL;
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    
    if(!interacted && __pes_path_edit.selectedObject.node.cmd_ptr){
        if(__pes_path_edit.selectedObject.node.next_cp_ptr){
            pesVec2f cp = *__pes_path_edit.selectedObject.node.next_cp_ptr;
            pesRectangle rect;
            rect.setFromCenter(cp.x, cp.y, s, s);
            if(rect.inside(p.x, p.y)){
                interacted = true;
                __grab_cp_ptr = __pes_path_edit.selectedObject.node.next_cp_ptr;
            }
        }
        
        if(!interacted && __pes_path_edit.selectedObject.node.prev_cp_ptr){
            pesVec2f cp = *__pes_path_edit.selectedObject.node.prev_cp_ptr;
            pesRectangle rect;
            rect.setFromCenter(cp.x, cp.y, s, s);
            if(rect.inside(p.x, p.y)){
                interacted = true;
                __grab_cp_ptr = __pes_path_edit.selectedObject.node.prev_cp_ptr;
            }
        }
    }
    
    if(!interacted){
        int pathNum = (int)__pes_path_edit.paths.size();
        for(int i=pathNum-1; i>=0; i--){
            if (__pes_path_edit.paths[i].bVisible == false) {
                continue;
            }
            int cmdNum = (int)__pes_path_edit.paths[i].getCommands().size();
            pesPath *path = &__pes_path_edit.paths[i];
            for(int j=cmdNum-1; j>=0; j--){
                pesPath::Command * cmd = &path->getCommands()[j];
                if(cmd->type==pesPath::Command::_close)
                    continue;
                pesRectangle rect;
                rect.setFromCenter(cmd->to.x, cmd->to.y, s, s);
                if(rect.inside(p.x, p.y)){
                    interacted = true;
                    __pes_path_edit.selectedObject.reset();
                    __pes_path_edit.selectedObject.node.path_ptr = path;
                    __pes_path_edit.selectedObject.node.cmd_ptr = (pesVec2f*)&cmd->to;
                    if(j>0 && (cmd->type==pesPath::Command::_bezierTo || cmd->type==pesPath::Command::_quadBezierTo)){
                        __pes_path_edit.selectedObject.node.prev_cp_ptr = (pesVec2f*)&cmd->cp2;
                    }
                    if(j<cmdNum-1 && (path->getCommands()[j+1].type==pesPath::Command::_bezierTo || path->getCommands()[j+1].type==pesPath::Command::_quadBezierTo)){
//                        __pes_path_edit.selectedObject.node.next_cp_ptr = (pesVec2f*)&path->getCommands()[j+1].cp1;
                        if(path->getCommands()[j+1].type==pesPath::Command::_quadBezierTo){
                            __pes_path_edit.selectedObject.node.next_cp_ptr = (pesVec2f*)&path->getCommands()[j+1].cp2;
                        }
                        else{
                            __pes_path_edit.selectedObject.node.next_cp_ptr = (pesVec2f*)&path->getCommands()[j+1].cp1;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    if(interacted){
        __pe_previousMouseX = ix;
        __pe_previousMouseY = iy;
        oldPaths = __pes_path_edit.paths;
        __pathChanged = false;
    }
    else{
        __pes_path_edit.selectedObject.reset();
        oldPaths.clear();
    }
    return interacted;
}


bool PES5_PathEditMouseMoved(int ix, int iy){
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_path_edit.center, __pes_path_edit.rotateDegree);
    
    __pes_path_edit.hoverCommand = NULL;
    int pathNum = (int)__pes_path_edit.paths.size();
    for(int i=pathNum-1; i>=0; i--){
        if (__pes_path_edit.paths[i].bVisible == false) {
            continue;
        }
        int cmdNum = (int)__pes_path_edit.paths[i].getCommands().size();
        pesPath *path = &__pes_path_edit.paths[i];
        for(int j=cmdNum-1; j>=0; j--){
            pesPath::Command * cmd = &path->getCommands()[j];
            if(cmd->type==pesPath::Command::_close)
                continue;
            pesRectangle rect;
            rect.setFromCenter(cmd->to.x, cmd->to.y, s, s);
            if(rect.inside(p.x, p.y)){
                interacted = true;
                __pes_path_edit.hoverCommand = &path->getCommands()[j];
                break;
            }
        }
    }
    
    return interacted;
}

bool PES5_PathEditMouseDragged(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p1 = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_path_edit.center, __pes_path_edit.rotateDegree);
    
    if(__pe_previousMouseX!=-1 && __pe_previousMouseY!=-1){
        if(__grab_cp_ptr){ // drag cp (drag node's arm)
            interacted = true;
            pesVec2f p0 = PES5_ToLocalPoint(__pe_previousMouseX, __pe_previousMouseY,
                                            canvasOrigin, __pes_path_edit.center,
                                            __pes_path_edit.rotateDegree);
            pesVec2f d = p1 - p0;
            __grab_cp_ptr->x+=d.x;
            __grab_cp_ptr->y+=d.y;
            __pathChanged = d.length();
        }
        else if(__pes_path_edit.selectedObject.node.path_ptr!=NULL){ // drag node
            interacted = true;
            pesVec2f p0 = PES5_ToLocalPoint(__pe_previousMouseX, __pe_previousMouseY,
                                            canvasOrigin, __pes_path_edit.center,
                                            __pes_path_edit.rotateDegree);
            pesVec2f d = p1 - p0;
            __pes_path_edit.selectedObject.translate(d.x, d.y);
            __pathChanged = d.length();
        }
        __pe_previousMouseX = ix;
        __pe_previousMouseY = iy;
    }
    return interacted;
}

bool PES5_PathEditMouseReleased(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_path_edit.center, __pes_path_edit.rotateDegree);
    if(__pe_previousMouseX!=-1 || __pe_previousMouseY!=-1){
        __pe_previousMouseX = -1;
        __pe_previousMouseY = -1;

        if(__pathChanged){
            newPaths = __pes_path_edit.paths;
            PathEditMoveCommand command(&__pes_path_edit, oldPaths, newPaths);
            command.execute();
            __pes_path_edit.pushCommand(command);
            __pathChanged = false;
        }
    }
    __grab_cp_ptr = NULL;
    return interacted;
}


// Stitch Edit
bool __stitch_edit_started = false;
//PesStitchEdit __pes_stitch_edit;
int __se_previousMouseX = -1;
int __se_previousMouseY = -1;
pesVec2f oldPoint, newPoint;
pesVec2f *selectedNode = NULL;
void PES5_StartStitchEditInput(){
    int selectedIndex = PES5_GetObjectSelectedIndex();
    if(__stitch_edit_started==false && pesInRange(selectedIndex, 0, pesGetDocument()->getObjectCount())
       && pesGetDocument()->getDataObject(selectedIndex)->getTotalStitches()){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Stitch Edit'); }");
#endif
        __stitch_edit_started = true;
        __pes_stitch_edit.reset();
        __pes_stitch_edit.pesData = pesGetDocument()->getDataObject(selectedIndex).get(); // capture
        __pes_stitch_edit.fillBlocks = __pes_stitch_edit.pesData->fillBlocks; // copy
        __pes_stitch_edit.strokeBlocks = __pes_stitch_edit.pesData->strokeBlocks; // copy
        __pes_stitch_edit.center = __pes_stitch_edit.pesData->getBoundingBox().getCenter(); //copy
        __pes_stitch_edit.rotateDegree = __pes_stitch_edit.pesData->parameter.rotateDegree; // copy
        sk_ui::GetPesDocumentView()->setCursorByID("canvas", "default");
    }
    __se_previousMouseX = -1;
    __se_previousMouseY = -1;
    selectedNode = NULL;
}

void PES5_StopStitchEditInput(){
    if(__stitch_edit_started){
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __stitch_edit_started = false;
        if(__pes_stitch_edit.pesData!=NULL){
            __pes_stitch_edit.pesData->fillBlocks.clear();
            __pes_stitch_edit.pesData->strokeBlocks.clear();
            __pes_stitch_edit.pesData->fillBlocks = __pes_stitch_edit.fillBlocks;
            __pes_stitch_edit.pesData->strokeBlocks = __pes_stitch_edit.strokeBlocks;
            for(int i=0; i<pesGetDocument()->getObjectCount(); i++){
                if(pesGetDocument()->getDataObject(i).get() == __pes_stitch_edit.pesData){
                    PES5_UpdatePesImage(i);
                    PES5_UpdateGUIRightPanel();
                    break;
                }
            }
        }
    }
    __pes_stitch_edit.reset();
    __se_previousMouseX = -1;
    __se_previousMouseY = -1;
    selectedNode = NULL;
}

bool PES5_IsStitchEditInputStarted(){
    return __stitch_edit_started;
}

void PES5_StitchEditDraw(SkCanvas & canvas){
    __pes_stitch_edit.draw(canvas);
}

bool PES5_StitchEditMousePressed(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_stitch_edit.center, __pes_stitch_edit.rotateDegree);
    
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    
    // stroke blocks (reverse iterator)
    if(!interacted){
        auto & block = __pes_stitch_edit.strokeBlocks;
        for(auto itb = block.rbegin(); itb!=block.rend(); itb++){
            auto & vertices = itb->polyline.getVertices();
            int idx = vertices.size() - 1;
            for(auto itv = vertices.rbegin(); itv!=vertices.rend(); itv++, idx--){
                if(itv->distance(p) < r){
                    interacted = true;
                    __pes_stitch_edit.selectedNode = &(*itv);
                    __pes_stitch_edit.selectedBlock = &(*itb);
                    __pes_stitch_edit.selectedIndex = idx;
                    break;
                }
            }
            if(__pes_stitch_edit.selectedNode)
                break;
        }
    }
    
    // fill blocks (reverse iterator)
    if(!interacted){
        auto & block = __pes_stitch_edit.fillBlocks;
        for(auto itb = block.rbegin(); itb!=block.rend(); itb++){
            auto & vertices = itb->polyline.getVertices();
            int idx = vertices.size() - 1;
            for(auto itv = vertices.rbegin(); itv!=vertices.rend(); itv++, idx--){
                if(itv->distance(p) < r){
                    interacted = true;
                    __pes_stitch_edit.selectedNode = &(*itv);
                    __pes_stitch_edit.selectedBlock = &(*itb);
                    __pes_stitch_edit.selectedIndex = idx;
                    break;
                }
            }
            if(__pes_stitch_edit.selectedNode)
                break;
        }
    }
    
    
    if(interacted){
        __se_previousMouseX = ix;
        __se_previousMouseY = iy;
        oldPoint = *__pes_stitch_edit.selectedNode;
        selectedNode = __pes_stitch_edit.selectedNode;
    }
    else{
        __pes_stitch_edit.selectedNode = NULL;
        __pes_stitch_edit.selectedBlock = NULL;
        __pes_stitch_edit.selectedIndex = -1;
    }
    return interacted;
}

bool PES5_StitchEditMouseMoved(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_stitch_edit.center, __pes_stitch_edit.rotateDegree);
    
    float doc_scale = PES5_GetZoomScale();
    const float s = 10/doc_scale;
    const float r = s/2;
    
    __pes_stitch_edit.hoveredNode = NULL;
    
    // stroke blocks (reverse iterator)
    if(__pes_stitch_edit.hoveredNode==NULL){
        auto & block = __pes_stitch_edit.strokeBlocks;
        for(auto itb = block.rbegin(); itb!=block.rend(); itb++){
            auto & vertices = itb->polyline.getVertices();
            for(auto itv = vertices.rbegin(); itv!=vertices.rend(); itv++){
                if(itv->distance(p) < r){
                    interacted = true;
                    __pes_stitch_edit.hoveredNode = &(*itv);
                    break;
                }
            }
            if(__pes_stitch_edit.hoveredNode)
                break;
        }
    }
    
    // fill blocks (reverse iterator)
    if(__pes_stitch_edit.hoveredNode==NULL){
        auto & block = __pes_stitch_edit.fillBlocks;
        for(auto itb = block.rbegin(); itb!=block.rend(); itb++){
            auto & vertices = itb->polyline.getVertices();
            for(auto itv = vertices.rbegin(); itv!=vertices.rend(); itv++){
                if(itv->distance(p) < r){
                    interacted = true;
                    __pes_stitch_edit.hoveredNode = &(*itv);
                    break;
                }
            }
            if(__pes_stitch_edit.hoveredNode)
                break;
        }
    }
    
    return interacted;
}

bool PES5_StitchEditMouseDragged(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p1 = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_stitch_edit.center, __pes_stitch_edit.rotateDegree);
    
    if(__se_previousMouseX!=-1 && __se_previousMouseY!=-1){
        if(__pes_stitch_edit.selectedNode){
            interacted = true;
            pesVec2f p0 = PES5_ToLocalPoint(__se_previousMouseX, __se_previousMouseY,
                                            canvasOrigin, __pes_stitch_edit.center,
                                            __pes_stitch_edit.rotateDegree);
            pesVec2f d = p1 - p0;
            __pes_stitch_edit.selectedNode->translate(d);
        }
        __se_previousMouseX = ix;
        __se_previousMouseY = iy;
    }
    
    return interacted;
}

bool PES5_StitchEditMouseReleased(int ix, int iy){
    bool interacted = false;
    pesVec2f canvasOrigin(sk_ui::GetPesDocumentView()->width()/2, sk_ui::GetPesDocumentView()->height()/2);
    pesVec2f p = PES5_ToLocalPoint(ix, iy, canvasOrigin, __pes_stitch_edit.center, __pes_stitch_edit.rotateDegree);
    if(__se_previousMouseX!=-1 || __se_previousMouseY!=-1){
        __se_previousMouseX = -1;
        __se_previousMouseY = -1;
        newPoint = *__pes_stitch_edit.selectedNode;
        if(oldPoint!=newPoint){
            StitchEditMoveCommand command(&__pes_stitch_edit, oldPoint, newPoint, __pes_stitch_edit.selectedBlock, __pes_stitch_edit.selectedIndex);
            command.execute();
            __pes_stitch_edit.pushCommand(command);
        }
    }
    return interacted;
}

bool PES5_StitchEditInsert(){
    bool inserted = false;
    if(__pes_stitch_edit.selectedNode && __pes_stitch_edit.selectedBlock){
        if(__pes_stitch_edit.selectedBlock->size()>=2
           && pesInRange(__pes_stitch_edit.selectedIndex, 0, __pes_stitch_edit.selectedBlock->size()-1)){
            StitchEditInsertCommand command(&__pes_stitch_edit);
            command.execute();
            __pes_stitch_edit.pushCommand(command);
            inserted = true;
        }
    }
    return inserted;
}

bool PES5_StitchEditRemove(){
    bool removed = false;
    if(__pes_stitch_edit.selectedNode && __pes_stitch_edit.selectedBlock){
        if(pesInRange(__pes_stitch_edit.selectedIndex, 0, __pes_stitch_edit.selectedBlock->size()-1)){
            StitchEditRemoveCommand command(&__pes_stitch_edit);
            command.execute();
            __pes_stitch_edit.pushCommand(command);
            removed = true;
        }
    }
    return removed;
}

bool __isStitchSimulatorMode = false;
int __lastSelectedIdx = -1;
void PES5_StartStitchSimulatorMode() {
    if (!__isStitchSimulatorMode) {
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode('Stitch Simulator'); }");
#endif
        __isStitchSimulatorMode = true;
        __lastSelectedIdx = PES5_GetObjectSelectedIndex();
        PES5_SetObjectSelectedIndex(-1);
    }
}

void PES5_StopStitchSimulatorMode() {
    if (__isStitchSimulatorMode) {
#ifdef __EMSCRIPTEN__
        emscripten_run_script("if( window.setDocViewMode ) { setDocViewMode(''); }");
#endif
        __isStitchSimulatorMode = false;

        auto sim = PesStitchSimulator::getInstance();
        sim->stop();

        //__simulatorData.clear();

        PES5_SetObjectSelectedIndex(__lastSelectedIdx);
    }
}

bool PES5_IsStitchSimulatorMode() {
    return __isStitchSimulatorMode;
}


// Outline Shape
#include "include/pathops/SkPathOps.h"
#include <include/private/base/SkPathEnums.h>
#include <src/utils/SkPolyUtils.h>

//enum SkPathOp {
//    kDifference_SkPathOp,         //!< subtract the op path from the first path
//    kIntersect_SkPathOp,          //!< intersect the two paths
//    kUnion_SkPathOp,              //!< union (inclusive-or) the two paths
//    kXOR_SkPathOp,                //!< exclusive-or the two paths
//    kReverseDifference_SkPathOp,  //!< subtract the first path from the op path
//};
//bool SK_API Op(const SkPath& one, const SkPath& two, SkPathOp op, SkPath* result);
//bool SK_API Simplify(const SkPath& path, SkPath* result);
//bool SK_API TightBounds(const SkPath& path, SkRect* result);
void PES5_AddObjectOutlineShape(float fInset, bool bRemoveHole){
    int selectedIndex = PES5_GetObjectSelectedIndex();
    if(pesInRange(selectedIndex, 0, PES5_GetObjectCount()-1)){
        auto pes = pesGetDocument()->getDataObject(selectedIndex);
        auto paths = pes->paths;

        if (pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT ||
            pes->parameter.type == pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN) {

            // std::vector<pesPath> newpaths;
            SkPath allpaths;

            for (int i = 0, ii = (int)paths.size(); i < ii; i++) {
                auto path = paths[i];
                vector<pesPath> subpaths = path.getSubPath();

                // Merge odd and even paths
                pesPath newpath;

                for (int j = 0, jj = (int)subpaths.size(); j < jj; j++) {
                    auto path0 = subpaths[j++];
                    auto path1 = subpaths[j];
                    auto skpath0 = toSk(path0);
                    auto skpath1 = toSk(path1);
                    auto skp = skpath0;

                    // reversePathTo is private method
                    // use reverseAddPath instead
                    // then join to 2'nd path by replace
                    // a command 'moveto' with 'lineto'
                    //
                    // SkPoint pt;
                    // skpath1.getLastPt(&pt);
                    // skp.lineTo(pt);
                    // skp.reversePathTo(skpath1);

                    skp.reverseAddPath(skpath1);
                    skp.close();

                    auto pp = toPes(skp);

                    // convert M to L
                    int path1StartIdx = (int)path0.getCommands().size();
                    pp.getCommands()[path1StartIdx].type = pesPath::Command::Type::_lineTo;

                    newpath.append(pp);
                }

                vector<pesPath> newsubpaths = newpath.getSubPath();
                if (newsubpaths.size() > 0) {
                    SkPath skp = toSk(newsubpaths[0]);
                    for (int k = 1, kk = (int)newsubpaths.size(); k < kk; k++) {
                        Op(skp, toSk(newsubpaths[k]), kUnion_SkPathOp, &skp);
                    }

                    const int countPoints = skp.countPoints();
                    SkTDArray<SkPoint> skpoints;
                    skpoints.reserve(countPoints);
                    skp.getPoints(skpoints.begin(), countPoints);
                    if (SkGetPolygonWinding(skpoints.begin(), countPoints) == 1) {
                        // if cw, reverse them to ccw
                        SkPath skpccw;
                        skpccw.reverseAddPath(skp);
                        skp = skpccw;
                    }

                    // newpaths.push_back(toPes(skp));
                    allpaths.addPath(skp);
                }
            }

            // seperate many outlines to many paths
            // paths = newpaths;

            // push all outlines in one path
            paths.clear();
            paths.push_back(toPes(allpaths));
        }

        if (paths.size()) {
            SkPath skp;
            int firstIndex;
            for (int i = 0; i < paths.size(); i++) {
                if (paths[i].bVisible) {
                    skp = toSk(paths[i]);
                    firstIndex = i;
                    break;
                }
            }

            for (int j = firstIndex + 1; j < paths.size(); j++) {
                if (paths[j].bVisible) {
                    SkPath skp1 = toSk(paths[j]);
                    Op(skp, skp1, kUnion_SkPathOp, &skp);
                }
            }

            // !!!
            // SkPath strokePath = pesData().SkiaPathStroke( skp, fInset );
            // Simplify(strokePath, &strokePath);
            // Op(strokePath, skp, SkPathOp::kUnion_SkPathOp, &skp);
            // Simplify(skp, &skp);

            pesPath ppath = toPes(skp);
            ppath.setFilled(false);
            ppath.setStrokeWidth(2.0);
            ppath.setStrokeHexColor(0x000000);
            
            vector<pesPolyline> offsets;
            if (fabs(fInset) >= 0.05) {
                const float pes_one_mm = 10.0;
                const float compensate = fInset * pes_one_mm;
//                enum JoinType {jtSquare, jtRound, jtMiter};
                offsets = pesClipper::getOffsets(ppath.getSubPath(),
                                                                     compensate,
                                                                     ClipperLib::jtSquare,
                                                                     ClipperLib::etClosedPolygon);                
            }
            else{
                offsets = ppath.getOutline();
            }
            
            if(bRemoveHole){
                float maxArea = 0;
                for(auto & offset : offsets){
                    float area = offset.getArea();
                    if(fabs(area) > fabs(maxArea)){
                        maxArea = area;
                    }
                }
                // remove hole
                pesRemove(offsets, [=](pesPolyline & p)->bool{
                    if(maxArea>0)
                        return p.getArea() <= 0;
                    return p.getArea() >= 0;
                });
            }
            
            ppath.clear();
            for (auto& offset : offsets) {
                offset.simplify();
                auto& vertices = offset.getVertices();
                ppath.moveTo(vertices[0].x, vertices[0].y);
                for (int i = 1; i < vertices.size(); i++) {
                    ppath.lineTo(vertices[i].x, vertices[i].y);
                }
                ppath.close();
            }
            
            pesData newPes;
            newPes.parameter.setType(pesData::OBJECT_TYPE_SCALABLE_SVG_FILE);
            newPes.parameter.fillType = pesData::FILL_TYPE_NONE;
            newPes.parameter.strokeType = pesData::STROKE_TYPE_NONE;
            newPes.paths.push_back(ppath);
            newPes.parameter.rotateDegree = pes->parameter.rotateDegree;
//            int cnt = pesGetDocument()->addObject(newPes);
//            if (cnt) {
//                PES5_UpdatePesImage(cnt - 1);
//                PES5_UpdateGUIRightPanel();
//            }
            PES5_AddPesData(newPes);
        }
    }
}


bool __shift_pressed = false;
void PES5_SetShiftPressed(bool pressed){
    __shift_pressed = pressed;
}

bool PES5_IsShiftPressed(){
    return __shift_pressed;
}

void PES5_OpenShapeDialog(){
    //static bool runonce = false;
    //if (!runonce) {
    //    sk_ui::GetPesShapeDialog()->dirty(true, true);
    //    runonce = true;
    //}
    sk_ui::GetPesShapeDialog()->show();
}

void PES5_OpenPatternFillDialog(){
    //static SkOnce runonec;
    //static bool runonce = false; 
    //if (!runonce) {
    //    sk_ui::GetPesPatternFillDialog()->dirty(true, true);
    //    runonce = true;
    //}
    sk_ui::GetPesPatternFillDialog()->show();
}

void PES5_OpenMotifFillDialog(){
    //static bool runonce = false;
    //if (!runonce) {
    //    sk_ui::GetPesMotifFillDialog()->dirty(true, true);
    //    runonce = true;
    //}
    sk_ui::GetPesMotifFillDialog()->show();
}

void PES5_OpenMotifStrokeDialog(){
    //static bool runonce = false;
    //if (!runonce) {
    //    sk_ui::GetPesMotifStrokeDialog()->dirty(true, true);
    //    runonce = true;
    //}
    sk_ui::GetPesMotifStrokeDialog()->show();
}

void PES5_OpenSettingDialog(){
    sk_ui::GetPesSettingDialog()->initUi();
    //static bool runonce = false;
    //if (!runonce) {
    //    sk_ui::GetPesSettingDialog()->dirty(true, true);
    //    runonce = true;
    //}
    sk_ui::GetPesSettingDialog()->show();
}

void PES5_OpenOutlineShapeDialog(){
    int selectedIndex = PES5_GetObjectSelectedIndex();
    if(pesInRange(selectedIndex, 0, PES5_GetObjectCount()-1)){
        auto pes = pesGetDocument()->getDataObject(selectedIndex);
        if(pes->paths.size()){
            sk_ui::GetPesOutlineShapeDialog()->show();
        }
    }
}

std::vector<pesStitchBlockList> __simulatorData;
void PES5_PrepareStitchSimulatorData(){
    __simulatorData.clear();
    int cnt = PES5_GetObjectCount();
    for(int i=0; i<cnt; i++){
        if( pesGetDocument()->getDataObject(i)->parameter.visible == false ) {
            continue;
        }
        __simulatorData.emplace_back();
        pesGetDocument()->getDataObject(i)->getStitchBlockList(__simulatorData.back());
        float rotateDegree = pesGetDocument()->getDataObject(i)->parameter.rotateDegree;
        if(rotateDegree!=0){
            pesVec2f center = pesGetDocument()->getDataObject(i)->getBoundingBox().getCenter();
            for(auto & block : __simulatorData.back()){
                block.rotateAround(rotateDegree, center);
            }
        }
    }
}

std::vector<pesStitchBlockList> & PES5_GetStitchSimulatorData(){
    return __simulatorData;
}

// Layer Info.
int PES5_GetLayerType(int layerIndex){
    if(layerIndex==-1 || layerIndex>PES5_GetObjectCount()-1)
        return -1;
    
    return pesGetDocument()->getDataObject(layerIndex)->parameter.type;
}

int PES5_GetLayerTotalStitches(int layerIndex){
    if(layerIndex==-1 || layerIndex>PES5_GetObjectCount()-1)
        return -1;
    
    return pesGetDocument()->getDataObject(layerIndex)->getTotalStitches();
}

int PES5_GetLayerNumColors(int layerIndex){
    if(layerIndex==-1 || layerIndex>PES5_GetObjectCount()-1)
        return -1;
    
    return pesGetDocument()->getDataObject(layerIndex)->colors.size();
}

bool PES5_ImportBufferAs(const pesBuffer& buff, const std::string as, const std::string options) {
    bool bReplaceColor = (options=="ReplaceColor");
    std::vector<uint8_t> colors;
    // SkDebugf("pesGetDocument()->getObjectCount() = %d\n", pesGetDocument()->getObjectCount());
    if(bReplaceColor && pesGetDocument()->getObjectCount()==1){
        auto old = pesGetDocument()->getDataObject(0);
        for(auto & b : old->fillBlocks){
            colors.push_back(b.colorIndex);
        }
    }
    // SkDebugf("colors.size() = %d\n", colors.size());
    // SkDebugf("options = %s\n", options.c_str());
    // SkDebugf("bReplaceColor = %d\n", bReplaceColor);
    
    if (as == "PPES") {
        bool ret = pesGetDocument()->loadPPESFromBuffer(buff);
        if (ret) {
            PES5_UndoRedoClear();
            PES5_MSClearSelection();        
        }
        return ret;
    } 
    else if (as == "PES") {
        pesData pes;
        if (pes.loadPESFromBuffer(buff)) {
            if(bReplaceColor){
                int num = std::min(colors.size(), pes.fillBlocks.size());
                for(int i=0; i<num; i++){
                    pes.fillBlocks[i].setColorFromIndex(colors[i]);
                }
                pes.recalculate();
            }
            //pesGetDocument()->addObject(pes);
            PES5_AddPesData(pes);
            return true;
        }
    }
    else if (as == "SVG") {
        pesData pes;
        if (pes.loadSVGFromBuffer(buff)) {
            //pesGetDocument()->addObject(pes);
            PES5_AddPesData(pes);
            return true;
        }
    }
    else if (as == "DST") {
        pesData pes;
        if (pes.loadDSTFromBuffer(buff)) {
            if(bReplaceColor){
                int num = std::min(colors.size(), pes.fillBlocks.size());
                // SkDebugf("num = %d\n", num);
                for(int i=0; i<num; i++){
                    // SkDebugf("Replace color %d with %d\n", pes.fillBlocks[i].colorIndex, colors[i]);
                    pes.fillBlocks[i].setColorFromIndex(colors[i]);
                }
                pes.recalculate();
            }
            //pesGetDocument()->addObject(pes);
            PES5_AddPesData(pes);
            return true;
        }
    }

    return false;
}

pesBuffer PES5_ExportBufferAs(const std::string as, const std::string options) {
    if (as == "PPES") {
        return pesGetDocument()->getPPESBuffer();
    } 
    else if (as == "PES") {
        int center = !(options == "0");
        return pesGetDocument()->getPES5Buffer(center);
    }
    else if (as == "PNG") {
        int previewStitch = (options=="1" || options=="true");
        if(previewStitch){
            return pesGetDocument()->getPreviewPNGBuffer();
        }
        else{
            return pesGetDocument()->getPNGBuffer();
        }
    }
    else if (as == "DST") {
        return pesGetDocument()->getDSTBuffer();
    }
    else if (as == "EXP") {
        return pesGetDocument()->getEXPBuffer();
    }
    else if (as == "JEF") {
        return pesGetDocument()->getJEFBuffer();
    }
    else if (as == "XXX") {
        return pesGetDocument()->getXXXBuffer();
    }
    else if (as == "SVG") {
        int objIndex = -1;
        int forScanNcut = 1;
        std::vector<std::string> params = pesSplitString(options, ",");
        int len = params.size();
        if (len > 0) {
            if (pesStringIsDigits(params[0])) {
                objIndex = pesToInt(params[0]);
            }
        }
        if (len > 1) {
            if (pesStringIsDigits(params[1])) {
                forScanNcut = pesToInt(params[1]);
            }
        }
        if (objIndex > -1 && objIndex < PES5_GetObjectCount()) {
            return pesGetDocument()->getDataObject(objIndex)->getSVGBuffer(forScanNcut);
        }
    }
    return pesBuffer();
}

void PES5_SetDarkMode(bool isDarkMode) {
    auto *p = sk_ui::GetPesDocumentView();
    if (p) {
        p->setDarkMode(isDarkMode);
    }
}

void PES5_Translate(float tx, float ty){
    if(PES5_IsPathEditInputStarted()){
        __pes_path_edit.selectedObject.translate(tx, ty);
    }
    else if(PES5_IsStitchEditInputStarted()){
        if(__pes_stitch_edit.selectedNode){
            __pes_stitch_edit.selectedNode->translate(tx, ty);
        }
    }
    else{
        if(PES5_GetObjectSelectedIndex()!=-1){
            pesGetDocument()->getDataObject(PES5_GetObjectSelectedIndex())->translate(tx, ty);
        }
    }
    sk_ui::GetPesDocumentView()->dirty(true, false);
}



//-----------------
// Multi Selection
//-----------------
std::set<int> g_selected_objects;
int PES5_MSSelectCount(){
    return (int)g_selected_objects.size(); 
}

void PES5_MSClearSelection() {
    PES5_SetObjectSelectedIndex(-1);
}

void PES5_MSSelectAll() {
    PES5_SetObjectSelectedIndex(-1, true);
}

bool PES5_MSIsSelecAll() {
    return g_selected_objects.size() == PES5_GetObjectCount();
}

bool PES5_MSIsSelected(int idx) {
    return g_selected_objects.size() && g_selected_objects.find(idx) != g_selected_objects.end();
}

bool PES5_MSSelect(int idx, bool multiSelect) {
    const int osize = (int)g_selected_objects.size();
    bool ret = false;
    if (pesInRange(idx, 0, PES5_GetObjectCount())) {
        if(multiSelect){
            if(g_selected_objects.find(idx)!=g_selected_objects.end()){
                // If exist then DeSelect
                return PES5_MSDeSelect(idx);
            }
            else{
                g_selected_objects.emplace(idx);
                sk_ui::GetPesDocumentView()->dirty(true);
                ret = true;
            }
        }
        else{
            // Normal Selection keep only one object
            g_selected_objects.clear();
            g_selected_objects.emplace(idx);
            sk_ui::GetPesDocumentView()->dirty(true);
            ret = true;
        }
    }
    if (osize != (int)g_selected_objects.size()) {
        sk_ui::GetPesDocumentView()->dirty(true);
    }
    return ret;
}

bool PES5_MSDeSelect(int idx){
    if(pesInRange(idx, 0, PES5_GetObjectCount())){
        g_selected_objects.erase(idx);
        sk_ui::GetPesDocumentView()->dirty(true);
        return true;
    }
    return false;
}

pesRectangle PES5_MSGetBoundingBox(){ // All BoundingBox
    pesRectangle r;
    for(int idx: g_selected_objects){
        if(r.isEmpty()){
            r.set(pesGetDocument()->getDataObject(idx)->getRotatedBoundingBox());
        }
        else{
            r.growToInclude(pesGetDocument()->getDataObject(idx)->getRotatedBoundingBox());
        }
    }
    return r;
}

bool PES5_MSIsScalable(){
    for(int i : g_selected_objects){
        if(pesGetDocument()->getDataObject(i)->isScalable()==false)
            return false;
    }
    return true;
}

void PES5_MSTranslate(float tx, float ty){
//    for(int i : g_selected_objects){
//        pesGetDocument()->getDataObject(i)->translate(tx, ty);
//    }
//    if (PES5_MSSelectCount()) {
//        sk_ui::GetPesDocumentView()->dirty(true);
//    }
    if(tx!=0 || ty!=0){
        TranslateCommand command(g_selected_objects, tx, ty);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_MSScale(float sx, float sy){
//    for(int i : g_selected_objects){
//        const auto pes = pesGetDocument()->getDataObject(i);
//        const auto ocxy = pes->getBoundingBox().getCenter();
//        pes->scale(sx, sy);
//        const auto cxy = pes->getBoundingBox().getCenter();
//        pes->translate(ocxy.x - cxy.x, ocxy.y - cxy.y);
//        PES5_UpdatePesImage(i);
//    }
//    if (PES5_MSSelectCount()) {
//        sk_ui::GetPesDocumentView()->dirty(true);
//    }
    if(sx!=1 || sy!=1){
        pesVec2f origin = PES5_MSGetBoundingBox().getCenter();
        ScaleCommand command(g_selected_objects, sx, sy, origin);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_MSRotate(float degree){
    pesVec2f pivot = PES5_MSGetBoundingBox().getCenter();
    for(int i : g_selected_objects){
        auto pes = pesGetDocument()->getDataObject(i);
        if(pes->isScalable()){
            for(auto & p:pes->paths){
                p.rotateAround(degree, pivot);
            }
        }
        
        for(auto & block:pes->fillBlocks){
            block.rotateAround(degree, pivot);
        }
        
        for(auto & block:pes->strokeBlocks){
            block.rotateAround(degree, pivot);
        }
        
        pes->calcBoundingBox();
        PES5_UpdatePesImage(i);
    }

    sk_ui::GetPesDocumentView()->dirty(true);
}

const float fDEG_TO_RAD = (float)DEG_TO_RAD;
void PES5_MSRotate2(float degree){
    if(degree!=0){
        pesVec2f pivot = PES5_MSGetBoundingBox().getCenter();
        RotateCommand command(g_selected_objects, degree, pivot);
        command.execute();
        PES5_UndoRedoPushCommand(command);
    }
}

void PES5_MSTranslateToCenter() {
    if (PES5_MSSelectCount()) {
        const pesVec2f cxy = PES5_MSGetBoundingBox().getCenter();
        PES5_MSTranslate(-cxy.x, -cxy.y);
    }
}

void PES5_MSPositonScale(float sx, float sy) {
    if (PES5_MSSelectCount() > 1) {
        const pesVec2f groupcxy = PES5_MSGetBoundingBox().getCenter();
        const auto groupcx = groupcxy.x;
        const auto groupcy = groupcxy.y;
        for (int i : g_selected_objects) {
            const auto pes = pesGetDocument()->getDataObject(i);
            const auto localcx = pes->centerx - groupcx;
            const auto localcy = pes->centery - groupcy;
            pes->translate(localcx * sx - localcx, localcy * sy - localcy);
        }
        sk_ui::GetPesDocumentView()->dirty(true);
    }
}

void PES5_MSDelete(){
    if (PES5_MSSelectCount()) {
        int index = PES5_GetObjectSelectedIndex();
        if (index > -1) {
            if(pesInRange(index, 0, PES5_GetObjectCount())){
                DeleteCommand command(index);
                command.execute();
                PES5_UndoRedoPushCommand(command);
                if (index >= PES5_GetObjectCount()) {
                    PES5_SetObjectSelectedIndex(-1);
                //} else {
                //    PES5_UpdatePesImage(index);
                }
            }
        } else {
            DeleteCommand command(g_selected_objects);
            command.execute();
            PES5_UndoRedoPushCommand(command);
        }
    }
}

void PES5_ApplyParameter(int index){
    auto pes = pesGetDocument()->getDataObject(index);
    ChangeParameterCommand command(index, pes->savedParameter, pes->parameter, pes->savedPaths, pes->paths);
    command.execute();
    PES5_UndoRedoPushCommand(command);
}

