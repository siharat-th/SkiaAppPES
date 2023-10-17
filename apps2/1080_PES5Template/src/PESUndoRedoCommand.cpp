//
//  PESUndoRedoCommand.cpp
//  PES5Template
//
//  Created by SIHARAT THAMMAYA on 10/10/2566 BE.
//

#include "PESUndoRedoCommand.hpp"
#include "PES5Command.hpp"
#include "pesDocument.hpp"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif
#include "Utils/PesSatinColumn.hpp"
#include "GUI/PES5DocumentView.hpp"

#define __MAX_UNDO_SIZE__   50
#define _PUSH_INDEX_        int _selectedIndex = PES5_GetObjectSelectedIndex(); if(PES5_MSSelectCount()==1){ PES5_SetObjectSelectedIndex(-1); }
#define _POP_INDEX_         if(PES5_MSSelectCount()==0 && pesInRange(_selectedIndex, 0, PES5_GetObjectCount())){ PES5_SetObjectSelectedIndex(_selectedIndex); }

UndoRedo::UndoRedo() {
}

UndoRedo::~UndoRedo() {
    clear();
}

bool UndoRedo::canUndo() {
    return !_undoCommands.empty();
}

bool UndoRedo::canRedo() {
    return !_redoCommands.empty();
}

void UndoRedo::redo() {
    auto cmd = _redoCommands.back();
    _redoCommands.pop_back();
    cmd->execute();
    _undoCommands.push_back(cmd);
}

void UndoRedo::undo() {
    auto cmd = _undoCommands.back();
    _undoCommands.pop_back();
    cmd->unExecute();
    _redoCommands.push_back(cmd);
}

void UndoRedo::pushCommand(Command* command) {
    pushCommand(std::make_shared<Command>(*command));
}

void UndoRedo::pushCommand(std::shared_ptr<Command> command) {
    int oversize = (int)(_undoCommands.size() - __MAX_UNDO_SIZE__ + 1);
    if (oversize > 0) {
        SkDebugf("Undo oversize %d\n", oversize);
        SkDebugf("before size %d\n", (int)_undoCommands.size());
        _undoCommands.erase(_undoCommands.begin(), _undoCommands.begin() + oversize);
        SkDebugf("affer erase size %d\n", (int)_undoCommands.size());
    }

    _undoCommands.push_back(command);

    _redoCommands.clear();
}

std::string UndoRedo::getRedoName() {
    return _redoCommands.back()->name;
}

std::string UndoRedo::getUndoName() {
    return _undoCommands.back()->name;
}

void UndoRedo::clear() {
    _undoCommands.clear();
    _redoCommands.clear();
}

void setDirty(){
    sk_ui::GetPesDocumentView()->dirty(true, false);
}

//--------------------------------------------------------
// COMMAND
//--------------------------------------------------------
Command::Command(std::string _name) : name(_name) {
    execute = NULL;
    unExecute = NULL;
}

TranslateCommand::TranslateCommand(std::set<int> indices, float tx, float ty)
:Command("Move")
{
    execute = [=]() {
        _PUSH_INDEX_
        for(int i : indices){
            pesGetDocument()->getDataObject(i)->translate(tx, ty);
        }
        _POP_INDEX_
        setDirty();
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        for(int i : indices){
            pesGetDocument()->getDataObject(i)->translate(-tx, -ty);
        }
        _POP_INDEX_
        setDirty();
    };
}

pesRectangle GetMaxPossibleBoundingBox(const std::set<int>& indices) {
    pesRectangle r;
    for (int i : indices) {
        if (r.isEmpty()) {
            r.set(pesGetDocument()->getDataObject(i)->getRotatedBoundingBox());
        } else {
            r.growToInclude(pesGetDocument()->getDataObject(i)->getRotatedBoundingBox());
        }
    }
    return r;
}

ScaleCommand::ScaleCommand(std::set<int> indices, float sx, float sy, pesVec2f origin)
:Command("Scale")
{
    execute = [=]() {
        _PUSH_INDEX_
        const auto groupcx = origin.x;
        const auto groupcy = origin.y;
        for (int i : indices) {
            const auto pes = pesGetDocument()->getDataObject(i);
            const auto localcx = pes->centerx - groupcx;
            const auto localcy = pes->centery - groupcy;
            pes->translate(localcx * sx - localcx, localcy * sy - localcy);
            const auto ocxy = pes->getBoundingBox().getCenter();
            pes->scale(sx, sy);
            const auto cxy = pes->getBoundingBox().getCenter();
            pes->translate(ocxy.x - cxy.x, ocxy.y - cxy.y);
            PES5_UpdatePesImage(i);
        }
        _POP_INDEX_
        setDirty();
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        const float invsx = 1 / sx;
        const float invsy = 1 / sy;
        const auto groupcx = origin.x;
        const auto groupcy = origin.y;
        for (int i : indices) {
            const auto pes = pesGetDocument()->getDataObject(i);
            const auto localcx = pes->centerx - groupcx;
            const auto localcy = pes->centery - groupcy;
            pes->translate(localcx * invsx - localcx, localcy * invsx - localcy);
            const auto ocxy = pes->getBoundingBox().getCenter();
            pes->scale(invsx, invsy);
            const auto cxy = pes->getBoundingBox().getCenter();
            pes->translate(ocxy.x - cxy.x, ocxy.y - cxy.y);
            PES5_UpdatePesImage(i);
        }
        _POP_INDEX_
        setDirty();
    };
}

RotateCommand::RotateCommand(std::set<int> indices, float degree, pesVec2f pivot)
:Command("Rotate")
{
    execute = [=]() {
        _PUSH_INDEX_
        //const pesVec2f pivot = GetMaxPossibleBoundingBox(indices).getCenter();
        const auto x0 = pivot.x;
        const auto y0 = pivot.y;

        // degree to radius
        const auto rad = degree * (float)DEG_TO_RAD;

        // cos, sin theta
        const auto cosrad  = cosf(rad);
        const auto sinrad  = sinf(rad);

        for (int i : indices) {
            const auto pes = pesGetDocument()->getDataObject(i);
            const auto obb = pes->getBoundingBox();
            const auto cxy = obb.getCenter();
            const auto cxrot = x0 + (cxy.x - x0) * cosrad - (cxy.y - y0) * sinrad;
            const auto cyrot = y0 + (cxy.x - x0) * sinrad + (cxy.y - y0) * cosrad;
            pes->translate(cxrot - cxy.x, cyrot - cxy.y);
            pes->parameter.rotateDegree = pesWrap(pes->parameter.rotateDegree + degree, 0, 360);
        }
        _POP_INDEX_
        setDirty();
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        //const pesVec2f pivot = GetMaxPossibleBoundingBox(indices).getCenter();
        const auto x0 = pivot.x;
        const auto y0 = pivot.y;

        // degree to radius
        const auto rad = -degree * (float)DEG_TO_RAD;

        // cos, sin theta
        const auto cosrad  = cosf(rad);
        const auto sinrad  = sinf(rad);

        for (int i : indices) {
            const auto pes = pesGetDocument()->getDataObject(i);
            const auto obb = pes->getBoundingBox();
            const auto cxy = obb.getCenter();
            const auto cxrot = x0 + (cxy.x - x0) * cosrad - (cxy.y - y0) * sinrad;
            const auto cyrot = y0 + (cxy.x - x0) * sinrad + (cxy.y - y0) * cosrad;
            pes->translate(cxrot - cxy.x, cyrot - cxy.y);
            pes->parameter.rotateDegree = pesWrap(pes->parameter.rotateDegree - degree, 0, 360);
        }
        _POP_INDEX_
        setDirty();
    };
}

DuplicateCommand::DuplicateCommand(int index)
:Command("Duplicate")
{
    int oldCount = PES5_GetObjectCount();
    
    execute = [=]() {
        _PUSH_INDEX_
        if(pesGetDocument()->duplicateObject(index)){
            g_pesImages.insert(g_pesImages.begin() + index, g_pesImages[index]->makeRasterImage());
            g_pesImagesThumbnail.insert(g_pesImagesThumbnail.begin() + index, g_pesImagesThumbnail[index]->makeRasterImage());
            PES5_UpdateGUIRightPanel();
            PES5_SetObjectSelectedIndex(index + 1);
            setDirty();
        }
        else{
            _POP_INDEX_
        }
    };
    
    unExecute = [=]() {
        if(PES5_GetObjectCount()>oldCount){
            _PUSH_INDEX_
            if(pesGetDocument()->deleteObject(index + 1)){
                g_pesImages.erase(g_pesImages.begin() + index + 1);
                g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + index + 1);
                PES5_UpdateGUIRightPanel();
                if (index+1 >= PES5_GetObjectCount()) {
                    PES5_SetObjectSelectedIndex(-1);
                }
                else{
                    _POP_INDEX_
                }
                setDirty();
            }
            else{
                _POP_INDEX_
            }
        }
    };
}

DeleteCommand::DeleteCommand(int index)
:Command("Delete")
{
    pesData pes = *pesGetDocument()->getDataObject(index);
    auto oldImage = g_pesImages[index];
    auto oldImageThumbnail = g_pesImagesThumbnail[index];
    
    execute = [=]() {
        _PUSH_INDEX_
        if(pesGetDocument()->deleteObject(index)){
            g_pesImages.erase(g_pesImages.begin() + index);
            g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + index);
            PES5_UpdateGUIRightPanel();
            if (index >= PES5_GetObjectCount()) {
                PES5_SetObjectSelectedIndex(-1);
            }
            else{
                _POP_INDEX_
            }
            setDirty();
        }
        else{
            _POP_INDEX_
        }
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        auto & objects = pesGetDocument()->getDataObjects();
        objects.insert(objects.begin() + index, make_shared<pesData>(pes));
        g_pesImages.insert(g_pesImages.begin() + index, oldImage);
        g_pesImagesThumbnail.insert(g_pesImagesThumbnail.begin() + index, oldImageThumbnail);
//        PES5_UpdateGUIRightPanel();
//        PES5_SetObjectSelectedIndex(index);
        PES5_SetObjectSelectedIndex(index);
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

DeleteCommand::DeleteCommand(std::set<int> indices)
: Command("Delete All")
{
    struct history{
        pesData pes;
        sk_sp<SkImage> oldImage;
        sk_sp<SkImage> oldImageThumbnail;
    };
    std::map<int, history> histories;
    for(int i : indices){
        histories[i] = history{*pesGetDocument()->getDataObject(i), g_pesImages[i], g_pesImagesThumbnail[i]};
    }
    
    execute = [=]() {
        _PUSH_INDEX_
        for(auto it=indices.rbegin(); it!=indices.rend(); it++){
            int i = *it;
            if(pesGetDocument()->deleteObject(i)){
                g_pesImages.erase(g_pesImages.begin() + i);
                g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + i);
            }
        }
        g_selected_objects.clear();
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
    
    unExecute = [=]() {
        _PUSH_INDEX_
        auto & objects = pesGetDocument()->getDataObjects();
        for(int i : indices){
            history h = histories.at(i);
            objects.insert(objects.begin() + i, make_shared<pesData>(h.pes));
            g_pesImages.insert(g_pesImages.begin() + i, h.oldImage);
            g_pesImagesThumbnail.insert(g_pesImagesThumbnail.begin() + i, h.oldImageThumbnail);
        }
        g_selected_objects = indices;
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

void sendBackward(int index){
    if(pesGetDocument()->moveObjectBack(index)){
        std::swap(g_pesImages[index], g_pesImages[index - 1]);
        std::swap(g_pesImagesThumbnail[index], g_pesImagesThumbnail[index - 1]);
        PES5_SetObjectSelectedIndex(index - 1);
        PES5_UpdateGUIRightPanel();

#ifdef __EMSCRIPTEN__
        EM_ASM({
            if (window["pes5updateLayerList"]) {
                window["pes5updateLayerList"]($0);
                window["pes5updateLayerList"]($1);
            }
        }, index-1, index);
#endif
    }
}

void bringForward(int index){
    if(pesGetDocument()->moveObjectFront(index)){
        std::swap(g_pesImages[index], g_pesImages[index + 1]);
        std::swap(g_pesImagesThumbnail[index], g_pesImagesThumbnail[index + 1]);
        PES5_SetObjectSelectedIndex(index + 1);
        PES5_UpdateGUIRightPanel();

#ifdef __EMSCRIPTEN__
        EM_ASM({
            if (window["pes5updateLayerList"]) {
                window["pes5updateLayerList"]($0);
                window["pes5updateLayerList"]($1);
            }
        }, index, index+1);
#endif
    }
}

SendBackwardCommand::SendBackwardCommand(int index)
:Command("Send Backward")
{
    execute = [=]() {
        _PUSH_INDEX_
        sendBackward(index);
        _POP_INDEX_
        setDirty();
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        bringForward(index - 1);
        _POP_INDEX_
        setDirty();
    };
}

BringForwardCommand::BringForwardCommand(int index)
:Command("Bring Forward")
{
    execute = [=]() {
        _PUSH_INDEX_
        bringForward(index);
        _POP_INDEX_
        setDirty();
    };

    unExecute = [=]() {
        _PUSH_INDEX_
        sendBackward(index + 1);
        _POP_INDEX_
        setDirty();
    };
}
//#include "PES5DocumentView.hpp"
MergeCommand::MergeCommand(int index)
:Command("Merge")
{
    pesData srcData = *pesGetDocument()->getDataObject(index);
    pesData dstData = *pesGetDocument()->getDataObject(index + 1);
    auto dstImage = g_pesImages[index + 1];
    auto dstImageThumbnail = g_pesImagesThumbnail[index + 1];
    
    execute = [=]() {
        _PUSH_INDEX_
        if(pesGetDocument()->mergeObject(index)){
            PES5_UpdatePesImage(index);
            g_pesImages.erase(g_pesImages.begin() + index + 1);
            g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + index + 1);
//            PES5_UpdateGUIRightPanel();
//            PES5_SetObjectSelectedIndex(index);
            PES5_SetObjectSelectedIndex(index);
            PES5_UpdateGUIRightPanel();
            setDirty();
        }
        else{
            _POP_INDEX_
        }
    };
    
    unExecute = [=]() {
        _PUSH_INDEX_
        auto & objects = pesGetDocument()->getDataObjects();
        objects[index] = make_shared<pesData>(srcData);
        objects.insert(objects.begin() + index + 1, make_shared<pesData>(dstData));
        PES5_UpdatePesImage(index);
        g_pesImages.insert(g_pesImages.begin() + index + 1, dstImage);
        g_pesImagesThumbnail.insert(g_pesImagesThumbnail.begin() + index + 1, dstImageThumbnail);
//        PES5_UpdateGUIRightPanel();
//        PES5_SetObjectSelectedIndex(index);
        PES5_SetObjectSelectedIndex(index);
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

BreakCommand::BreakCommand(int index)
: Command("Break")
{
    int oldCount = PES5_GetObjectCount();
    pesData oldPes = *pesGetDocument()->getDataObject(index);
    execute = [=](){
        _PUSH_INDEX_
        pesStitchBlockList blocks = oldPes.fillBlocks;
        pesGetDocument()->deleteObject(index);
        g_pesImages.erase(g_pesImages.begin() + index);
        g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + index);
        for(int i=0; i<blocks.size(); i++){
            pesData data;
            data.parameter.setType(pesData::OBJECT_TYPE_PES);
            data.fillBlocks.push_back(blocks[i]);
            data.recalculate();
            int cnt = pesGetDocument()->addObject(data);
            if(cnt){
                PES5_UpdatePesImage(cnt-1);
            }
        }
        blocks.clear();
//        PES5_UpdateGUIRightPanel();
//        PES5_SetObjectSelectedIndex(-1);
        PES5_SetObjectSelectedIndex(-1);
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        auto & objects = pesGetDocument()->getDataObjects();
        objects[index] = make_shared<pesData>(oldPes);
        objects.resize(oldCount);
        g_pesImages.resize(oldCount);
        g_pesImagesThumbnail.resize(oldCount);
        PES5_UpdatePesImage(index);
//        PES5_UpdateGUIRightPanel();
//        PES5_SetObjectSelectedIndex(index);
        PES5_SetObjectSelectedIndex(index);
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

VertitalFlipCommand::VertitalFlipCommand(int index)
: Command("Vertical Flip")
{
    execute = [=](){
        _PUSH_INDEX_
        pesGetDocument()->getDataObject(index)->verticalFlip();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        pesGetDocument()->getDataObject(index)->verticalFlip();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        setDirty();
    };
}

HorizontalFlipCommand::HorizontalFlipCommand(int index)
: Command("Horizontal Flip")
{
    execute = [=](){
        _PUSH_INDEX_
        pesGetDocument()->getDataObject(index)->horizontalFlip();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        pesGetDocument()->getDataObject(index)->horizontalFlip();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        setDirty();
    };
}

ColorBlockSwapCommand::ColorBlockSwapCommand(pesData * pes, int index1, int index2)
: Command("Swap Color")
{
    execute = [=](){
        _PUSH_INDEX_
        std::swap(pes->fillBlocks[index1], pes->fillBlocks[index2]);
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        std::swap(pes->fillBlocks[index1], pes->fillBlocks[index2]);
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

ImportCommand::ImportCommand(pesData * data)
: Command("Import")
{
    pesData oldPes = *data;
    execute = [=](){
        _PUSH_INDEX_
        pesData pes = oldPes;
        int cnt = pesGetDocument()->addObject(pes);
        if(cnt){
            int index = cnt - 1;
            PES5_UpdatePesImage(index);
            PES5_UpdateGUIRightPanel();
            PES5_SetObjectSelectedIndex(index);
            setDirty();
        }
        else{
            _POP_INDEX_
        }
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        int cnt = PES5_GetObjectCount();
        int index = cnt -1 ;
        if(pesGetDocument()->deleteObject(index)){
            g_pesImages.erase(g_pesImages.begin() + index);
            g_pesImagesThumbnail.erase(g_pesImagesThumbnail.begin() + index);
            PES5_UpdateGUIRightPanel();
            PES5_SetObjectSelectedIndex(-1);
            setDirty();
        }
        else{
            _POP_INDEX_
        }
    };
}

ChangeParameterCommand::ChangeParameterCommand(int index, pesData::Parameter oldParam, pesData::Parameter newParam, std::vector<pesPath> oldPaths, std::vector<pesPath> newPaths)
:Command("Property")
{
    execute = [=](){
        _PUSH_INDEX_
        auto pes = pesGetDocument()->getDataObject(index);
        pes->parameter = newParam;
        pes->paths = newPaths;
        auto type = pes->parameter.type;
        if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT){
            pes->applyPPEFFill();
        }
        else if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2){
            pes->applyPPEF_V2_Fill();
        }
        else{
            pes->applyFill();
        }
        pes->applyStroke();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
    
    unExecute = [=](){
        _PUSH_INDEX_
        auto pes = pesGetDocument()->getDataObject(index);
        pes->parameter = oldParam;
        pes->paths = oldPaths;
        auto type = pes->parameter.type;
        if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT){
            pes->applyPPEFFill();
        }
        else if(type==pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2){
            pes->applyPPEF_V2_Fill();
        }
        else{
            pes->applyFill();
        }
        pes->applyStroke();
        PES5_UpdatePesImage(index);
        _POP_INDEX_
        PES5_UpdateGUIRightPanel();
        setDirty();
    };
}

// PathEdit
PathEditMoveCommand::PathEditMoveCommand(PesPathEdit * pathEdit, std::vector<pesPath> oldPaths, std::vector<pesPath> newPaths)
: Command("Move")
{
    execute = [=]{
        _PUSH_INDEX_
        pathEdit->paths = newPaths;
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        pathEdit->paths = oldPaths;
        _POP_INDEX_
        setDirty();
    };
}

// StitchEdit
StitchEditMoveCommand::StitchEditMoveCommand(PesStitchEdit * stitchEdit, pesVec2f oldPoint, pesVec2f newPoint, pesVec2f * selectedNode)
:Command("Move")
{
    execute = [=]{
        _PUSH_INDEX_
        if(selectedNode!=NULL){
            selectedNode->set(newPoint);
        }
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        if(selectedNode!=NULL){
            selectedNode->set(oldPoint);
        }
        _POP_INDEX_
        setDirty();
    };
}

StitchEditMoveCommand::StitchEditMoveCommand(PesStitchEdit * stitchEdit, pesVec2f oldPoint, pesVec2f newPoint, pesStitchBlock *selectedBlock, int selectedIndex)
: Command("Move")
{
    execute = [=]{
        _PUSH_INDEX_
        if(selectedBlock!=NULL){
            selectedBlock->polyline.getVertices()[selectedIndex] = newPoint;
        }
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        if(selectedBlock!=NULL){
            selectedBlock->polyline.getVertices()[selectedIndex] = oldPoint;
        }
        _POP_INDEX_
        setDirty();
    };
}

StitchEditInsertCommand::StitchEditInsertCommand(PesStitchEdit * stitchEdit)
: Command("Insert")
{
    int oldIndex = stitchEdit->selectedIndex;
    auto & selectedBlock = stitchEdit->selectedBlock;
    execute = [=]{
        _PUSH_INDEX_
        int selIndex = oldIndex; //stitchEdit->selectedIndex;
        auto & vertices = selectedBlock->polyline.getVertices(); //stitchEdit->selectedBlock->polyline.getVertices();
        int leftIndex, rightIndex;
        if(selIndex==selectedBlock->size()-1){
            leftIndex = selIndex -1;
            rightIndex = selIndex;
        }
        else{
            leftIndex = selIndex;
            rightIndex = selIndex+1;
        }
        
        pesVec2f p0 = vertices[leftIndex];
        pesVec2f p1 = vertices[rightIndex];
        pesVec2f pBetween = (p0 + p1) / 2;
        
        vertices.insert(vertices.begin()+rightIndex, pBetween);
        stitchEdit->selectedNode = &vertices[rightIndex];
        stitchEdit->selectedIndex = rightIndex;
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        auto & vertices = selectedBlock->polyline.getVertices(); //stitchEdit->selectedBlock->polyline.getVertices();
        vertices.erase(vertices.begin() + oldIndex);
        stitchEdit->selectedIndex = oldIndex;
        _POP_INDEX_
        setDirty();
    };
}

StitchEditRemoveCommand::StitchEditRemoveCommand(PesStitchEdit * stitchEdit)
: Command("Delete")
{
    int oldIndex = stitchEdit->selectedIndex;
    pesVec2f oldPoint = *stitchEdit->selectedNode;
    auto & oldBlock = stitchEdit->selectedBlock;
    execute = [=]{
        _PUSH_INDEX_
        int selIndex = oldIndex; //stitchEdit->selectedIndex;
        auto & vertices = oldBlock->polyline.getVertices();  //stitchEdit->selectedBlock->polyline.getVertices();
        if(stitchEdit->hoveredNode == &vertices[selIndex]){
            stitchEdit->hoveredNode = NULL; // reset hover node
        }
        vertices.erase(vertices.begin()+selIndex);
        
        if(--selIndex<0)
            selIndex=0;
        
        if(pesInRange(selIndex, 0, oldBlock->size()-1)){
            stitchEdit->selectedNode = &vertices[selIndex];
            stitchEdit->selectedIndex = selIndex;
        }
        else{
            stitchEdit->selectedNode = NULL;
            stitchEdit->selectedBlock = NULL;
            stitchEdit->selectedIndex = -1;
            stitchEdit->hoveredNode = NULL;
        }
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        auto & vertices = oldBlock->polyline.getVertices();  //stitchEdit->selectedBlock->polyline.getVertices();
        vertices.insert(vertices.begin() + oldIndex, oldPoint);
        stitchEdit->selectedNode = &vertices[oldIndex];
        stitchEdit->selectedBlock = oldBlock;
        stitchEdit->selectedIndex = oldIndex;
        _POP_INDEX_
        setDirty();
    };
}

StitchEditStitchBlockCommand::StitchEditStitchBlockCommand(PesStitchEdit * stitchEdit, pesStitchBlockList oldFillBlocks, pesStitchBlockList oldStrokeBlocks, pesStitchBlockList fillBlocks, pesStitchBlockList strokeBlocks)
: Command("Edit")
{
    execute = [=]{
        _PUSH_INDEX_
        stitchEdit->fillBlocks = fillBlocks;
        stitchEdit->strokeBlocks = strokeBlocks;
        _POP_INDEX_
        setDirty();
    };
    
    unExecute = [=]{
        _PUSH_INDEX_
        stitchEdit->fillBlocks = oldFillBlocks;
        stitchEdit->strokeBlocks = oldStrokeBlocks;
        _POP_INDEX_
        setDirty();
    };
}

