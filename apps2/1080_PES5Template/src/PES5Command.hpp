//
//  PES5Command.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 29/8/2565 BE.
//

#ifndef PES5Command_hpp
#define PES5Command_hpp

#include <stdio.h>
#include <string>
#ifndef __EMSCRIPTEN__
#include "pesEngine.h"
#endif
#include "include/core/SkCanvas.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkFont.h"
#include "include/core/SkData.h"
#include <set>
#include "PESUndoRedoCommand.hpp"

class pesVec2f;
class pesData;
class pesStitchBlock;
class pesBuffer;
class pesRectangle;

//----------------------
// Undo/Redo & Command
//----------------------

bool PES5_CanUndo();
bool PES5_CanRedo();
void PES5_Undo();
void PES5_Redo();
void PES5_UndoRedoPushCommand(Command command);
void PES5_UndoRedoPushCommand(std::shared_ptr<Command> command);
std::string PES5_GetRedoName();
std::string PES5_GetUndoName();
void PES5_UndoRedoClear();
void PES5_UpdateUndoRedoTooltips();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
extern std::vector<sk_sp<SkImage>> g_pesImages;
extern std::vector<sk_sp<SkImage>> g_pesImagesThumbnail;

const std::string PES5_GetResourcePath(const std::string& resource);
void PES5_SetResourcePath(const std::string& resource);

void PES5_SetDocumentName(const std::string docname = "Untitled");
const std::string PES5_GetDocumentName();
void PES5_NewDocument();
bool PES5_LoadPPES(const std::string & path);

sk_sp<SkData> PES5_GetCachePatternAndMotif(std::string filepath);
void PES5_SetCachePatternAndMotif(std::string filepath, sk_sp<SkData> data);
void PES5_LoadPatternAndMotif();
void PES5_ReplaceWithDefaultFont(); // Backward compatible
void PES5_SavePPES(const std::string & path);

bool PES5_ImportPES(const std::string & path);
bool PES5_ImportSVG(const std::string & path);
void PES5_ExportPES(const std::string & path);
void PES5_ExportPNG(const std::string & path);

void PES5_UpdatePesImages();
void PES5_UpdatePesImage(int index);
void PES5_UpdatePesImageThumbnail(int index);
int PES5_GetPesImageCount();
sk_sp<SkImage> PES5_GetPesImage(int index);
sk_sp<SkImage> PES5_GetPesImageThumbnail(int index);
sk_sp<SkImage> PES5_GetSVGPartTypeImage( int layerIndex, std::string filter );

void PES5_ZoomIn();
void PES5_ZoomOut();
void PES5_ResetZoomScale();
void PES5_SetZoomScale( float z );
float PES5_GetZoomScale();

pesVec2f PES5_GetPan();
void PES5_SetPan(const pesVec2f & pan);
void PES5_ResetPan();
void PES5_SetHasRulur(float rulurw);
pesVec2f PES5_ToLocalPoint(float x, float y, const pesVec2f& canvasOrigin, pesData* pes = nullptr);
pesVec2f PES5_ToLocalPoint(float x, float y, const pesVec2f & canvasOrigin, const pesVec2f & objectCenter, float objectRotateDegree);

int PES5_SetObjectSelectedIndex(int index, bool multiselect = false);
int PES5_GetObjectSelectedIndex();

void PES5_OpenPPESFile();
void PES5_SavePPESFile(const char* filename = "Untitled.ppes");
void PES5_ImportPESFile();
void PES5_ImportSVGFile();
void PES5_ImportBackgroundFile();
void PES5_ExportPESFile(const char* filename = "Untitled.pes", bool bCenter=false);
void PES5_ExportPNGFile(const char* filename = "Untitled.png");
void PES5_ExportDSTFile(const char* filename = "Untitled.dst");
void PES5_ExportXXXFile(const char* filename = "Untitled.xxx");
void PES5_ExportEXPFile(const char* filename = "Untitled.exp");
void PES5_ExportJEFFile(const char* filename = "Untitled.jef");
void PES5_ExportSVGFile(const char* filename = "Untitled.svg");

void PES5_AddPesData(pesData data);

void PES5_AddPPEFText();
void PES5_AddTTFText();
void PES5_AddPPEFText_V2();
void PES5_ReplacePPEFText(int objIndex=PES5_GetObjectSelectedIndex());
void PES5_ReplaceTTFText(const sk_sp<SkTypeface> typeface, int objIndex=PES5_GetObjectSelectedIndex());
void PES5_ReplacePPEFText_V2(int objIndex=PES5_GetObjectSelectedIndex());

std::string PES5_ObjectTypeToString(int type);
int PES5_GetObjectCount();

void PES5_UpdateGUIRightPanel();

void PES5_LayerMoveUp();
void PES5_LayerMoveDown();
void PES5_LayerMerge();
void PES5_LayerBreak();
void PES5_LayerDuplicate();
void PES5_LayerDelete();

bool PES5_ColorBlockSwap(pesData * pes, int index1, int index2);

void PES5_LoadOrDownloadAsset(const char* path, std::function<void(sk_sp<SkData> data)> onLoaded);

void PES5_TracingPNGFile();
void PES5_OpenTracingDialog(sk_sp<SkData> data);

void PES5_HorizontalFlip();
void PES5_VerticalFlip();

void PES5_StartSatinColumnInput();
void PES5_StopSatinColumnInput();
bool PES5_IsSatinColumnInputStarted();
void PES5_DrawSatinColumnInput(SkCanvas & canvas);
void PES5_SatinColumnAddCornerPoint(float x, float y);
void PES5_SatinColumnAddCurvePoint(float x, float y);
void PES5_SatinColumnRemovePreviousPoint();

// f040 - pencil
void PES5_StartDrawingInput();
void PES5_StopDrawingInput();
bool PES5_IsDrawingInputStarted();
void PES5_DrawingInputDraw(SkCanvas & canvas);
void PES5_DrawingInputAddFirstPoint(const pesVec2f & p);
void PES5_DrawingInputAddPoint(const pesVec2f & p);
void PES5_DrawingInputAddLastPoint(const pesVec2f & p);

// Pen (Lines tool)
void PES5_StartLinesToolInput();
void PES5_StopLinesToolInput();
bool PES5_IsLinesToolInputStarted();
void PES5_LinesToolDraw(SkCanvas & canvas);
void PES5_LinesToolAddCornerPoint(float x, float y);
void PES5_LinesToolAddCurvePoint(float x, float y);
void PES5_LinesToolRemovePreviousPoint();

// Polygon (Polygon tool)
void PES5_StartPolygonToolInput();
void PES5_StopPolygonToolInput();
bool PES5_IsPolygonToolInputStarted();
void PES5_PolygonToolDraw(SkCanvas & canvas);
void PES5_PolygonToolAddCornerPoint(float x, float y);
void PES5_PolygonToolAddCurvePoint(float x, float y);
void PES5_PolygonToolRemovePreviousPoint();

// Path Edit
void PES5_StartPathEditInput();
void PES5_StopPathEditInput();
bool PES5_IsPathEditInputStarted();
void PES5_PathEditDraw(SkCanvas & canvas);
bool PES5_PathEditMousePressed(int ix, int iy);
bool PES5_PathEditMouseMoved(int ix, int iy);
bool PES5_PathEditMouseDragged(int ix, int iy);
bool PES5_PathEditMouseReleased(int ix, int iy);

// Stitch Edit
void PES5_StartStitchEditInput();
void PES5_StopStitchEditInput();
bool PES5_IsStitchEditInputStarted();
void PES5_StitchEditDraw(SkCanvas & canvas);
bool PES5_StitchEditMousePressed(int ix, int iy);
bool PES5_StitchEditMouseMoved(int ix, int iy);
bool PES5_StitchEditMouseDragged(int ix, int iy);
bool PES5_StitchEditMouseReleased(int ix, int iy);
bool PES5_StitchEditInsert();
bool PES5_StitchEditRemove();

// Stitch Simulator Mode
void PES5_StartStitchSimulatorMode();
void PES5_StopStitchSimulatorMode();
bool PES5_IsStitchSimulatorMode();

// Outline Shape
void PES5_AddObjectOutlineShape(float fInset=1.0, bool bRemoveHole=true);

void PES5_SetShiftPressed(bool pressed);
bool PES5_IsShiftPressed();

void PES5_OpenShapeDialog();
void PES5_OpenPatternFillDialog();
void PES5_OpenMotifFillDialog();
void PES5_OpenMotifStrokeDialog();
void PES5_OpenSettingDialog();
void PES5_OpenOutlineShapeDialog();

void PES5_PrepareStitchSimulatorData();
std::vector<std::vector<pesStitchBlock>> & PES5_GetStitchSimulatorData();

// Layer Info.
int PES5_GetLayerType(int layerIndex=PES5_GetObjectSelectedIndex());
int PES5_GetLayerTotalStitches(int layerIndex=PES5_GetObjectSelectedIndex());
int PES5_GetLayerNumColors(int layerIndex=PES5_GetObjectSelectedIndex());

void PES5_SelectLayerList(int index);

void PES5_UpdateTTFFont( int objIndex, int ttfIndex );

bool PES5_ImportBufferAs(const pesBuffer& buff, const std::string as, const std::string options = nullptr);
pesBuffer PES5_ExportBufferAs(const std::string as, const std::string options = nullptr);

void PES5_SetDarkMode(bool isDarkMode);

void PES5_Translate(float tx, float ty);

//-----------------
// Multi Selection
//-----------------
extern std::set<int> g_selected_objects;
int PES5_MSSelectCount();
void PES5_MSClearSelection();
void PES5_MSSelectAll();
bool PES5_MSIsSelecAll();
bool PES5_MSIsSelected(int idx);
bool PES5_MSSelect(int idx, bool multiSelect=true);
bool PES5_MSDeSelect(int idx);
pesRectangle PES5_MSGetBoundingBox();
bool PES5_MSIsScalable();
void PES5_MSTranslate(float tx, float ty);
void PES5_MSScale(float sx, float sy); // Note: scale from left-top
void PES5_MSRotate(float degree); // Note: rotate around getBoundingBox().getCenter()
void PES5_MSRotate2(float degree);
void PES5_MSTranslateToCenter();
void PES5_MSPositonScale(float sx, float sy);
void PES5_MSDelete();

void PES5_ApplyParameter(int index);

#endif /* PES5Command_hpp */
