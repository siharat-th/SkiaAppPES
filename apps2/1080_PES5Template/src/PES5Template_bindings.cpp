//
//  PES5Template_bindings.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 9/8/2565 BE.
//

#include <stdio.h>
#include <string>

#include "PES5Command.hpp"

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "modules/canvaskit/WasmCommon.h"
#include "include/core/SkData.h"
#include "modules/pes/include/pesEngine.h"

#include "include/core/SkEncodedImageFormat.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkPathUtils.h"
#include "include/utils/SkBase64.h"
#include "include/core/SkData.h"
#include "include/core/SkScalar.h"
#include "include/core/SkSurface.h"
#include "include/core/SkVertices.h"
#include "include/pathops/SkPathOps.h"
#include "include/utils/SkTextUtils.h"

#include "modules/pes/include/pesPathUtility.hpp"
#include "PES5Template.hpp"

using namespace emscripten;
using namespace std;

class PES5Template{
public:
    static long FLAG_HIDE_TITLE_BAR;
    static long FLAG_HIDE_TOOL_BAR;
    static long FLAG_HIDE_TOOL_BOX;
    static long FLAG_HIDE_LAYER_BOX;
    static long FLAG_HIDE_RULER;
    static long FLAG_NO_WELCOME;
    static long FLAG_HIDE_JUMP_STITCH;
    static long FLAG_RENDER_TEXTURE;
    static void Init( long flags ) {
        PES5_GLOBAL_FLAGS = flags;

        char* c_str = (char*)EM_ASM_PTR({
            if (!('resourcePath' in globalThis)) {
                globalThis['resourcePath'] = "/resources";
            }
            var jsString = globalThis['resourcePath'];
            var lengthBytes = lengthBytesUTF8(jsString) + 1;
            var stringOnWasmHeap = _malloc(lengthBytes);
            stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
            return stringOnWasmHeap;
        });
        std::string str = c_str;
        PES5Template::SetResourcePath(str);
        free(c_str);
    }
    static void UpdateImages(){
        PES5_UpdatePesImages();
    }
    
    static void UpdateImage(int index){
        PES5_UpdatePesImage(index);
    }
    
    static int GetObjectSelectedIndex(){
        return PES5_GetObjectSelectedIndex();
    }
    
    static void SetObjectSelectedIndex(int index) {
        PES5_SetObjectSelectedIndex(index);
    }
    
    static void SetObjectSelectedIndex(int index, bool multiselect) {
        PES5_SetObjectSelectedIndex(index, multiselect);
    }
    
    static std::string GetResourcePath(const std::string& resource) {
        return PES5_GetResourcePath(resource);
    }
    
    static void SetResourcePath(const std::string& resource) {
        PES5_SetResourcePath(resource);
    }

    static void NewDocument(){
        PES5_NewDocument();
    }

    static void SetDocumentName(std::string docname) {
        PES5_SetDocumentName(docname);
    }
    
    static std::string GetDocumentName() {
        return PES5_GetDocumentName();
    }

    static int GetObjectCount() { 
        return PES5_GetObjectCount();
    } 
    
    static void UpdateGUIRightPanel(){
        PES5_UpdateGUIRightPanel();
    }

    static void UpdatePesImage( int objIndex){
        PES5_UpdatePesImage(objIndex);
    }

    static void OpenTracingDialog(const pesBuffer & pbuff){
        sk_sp<SkData> data = SkData::MakeWithCopy(pbuff.getData(), pbuff.size());
        PES5_OpenTracingDialog(data);
    }
    
    static void LoadPatternAndMotif(){
        PES5_LoadPatternAndMotif();
    }
    
    static std::string GetLayerTypeString(int index){
        if( index < 0 ) {
            return "";
        }
        auto type = pesGetDocument()->getDataParameter(index).type;
        //SkDebugf( "Layer Type: %i\n", type );
        return PES5_ObjectTypeToString(type);
    }

    static std::string GetLayerTextString(int index) {
        if (index < 0) {
            return "";
        }
        return pesGetDocument()->getDataParameter(index).text;
    }

    static bool IsLayerLocked(int index) {
        if( index < 0 ) {
            return "";
        }
        bool locked = pesGetDocument()->getDataParameter(index).locked;
        //SkDebugf( "Layer Type: %i\n", type );
        return locked;
    }    
    
    static bool ImageToBase64DataURI(sk_sp<SkImage> img, SkString* dst) {
        sk_sp<SkData>pngData = img->encodeToData( SkEncodedImageFormat::kPNG, 100 );

        size_t len = SkBase64::Encode(pngData->data(), pngData->size(), nullptr);

        // The PNG can be almost arbitrarily large. We don't want to fill our logs with enormous URLs.
        // Infra says these can be pretty big, as long as we're only outputting them on failure.
        static const size_t kMaxBase64Length = 1024 * 1024;
        if (len > kMaxBase64Length) {
            dst->printf("Encoded image too large (%u bytes)", static_cast<uint32_t>(len));
            return false;
        }

        dst->resize(len);
        SkBase64::Encode(pngData->data(), pngData->size(), dst->data());
        dst->prepend("data:image/png;base64,");
        return true;
    }
    static void ExportPNG(std::string strFileName){
        PES5_ExportPNGFile(strFileName.c_str());
    }
    
    static std::string GetLayerImageString(int index){
        const auto img = PES5_GetPesImageThumbnail(index);
        if (img.get() == nullptr) {
            return "";
        }
        SkString s;
        ImageToBase64DataURI( img, &s );
        return std::string( s.c_str() );
    }
    static std::string GetSVGPartTypeImageString( int layerIndex, std::string parttype ){
        const auto img = PES5_GetSVGPartTypeImage(layerIndex, parttype);
        if (img == nullptr) {
            return "";
        }
        SkString s;
        ImageToBase64DataURI( img, &s );
        return std::string( s.c_str() );
    }    
    
    static void SelectLayer(int index){
        PES5_SelectLayerList(index);
        PES5_SetObjectSelectedIndex(index);
    }

    static void ReplaceWithDefaultFont(){
        PES5_ReplaceWithDefaultFont();
    }

    static void SetZoom( float zoom ) {
        PES5_SetZoomScale( zoom );
    }

    static float GetZoom() {
        return PES5_GetZoomScale();
    }

    static void SetPan( float x, float y ) {
        const pesVec2f pan = pesVec2f(x, y);
        PES5_SetPan(pan);
    }

    static JSArray GetPan() { 
        JSArray props = emscripten::val::array();
        const pesVec2f pan = PES5_GetPan();
        props.call<void>("push", emscripten::val(pan.x));
        props.call<void>("push", emscripten::val(pan.y));
        return props;
    }

    static void UpdateTTFText( int objIndex, int fontIndex ) {
        PES5_UpdateTTFFont( objIndex, fontIndex );
    }
    static void UpdateSVG( int objIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();
        pes->applyFill();
        pes->applyStroke();
        PES5_UpdatePesImage(objIndex);
    }

    static void OpenPatternFillDialog() {
        PES5_OpenPatternFillDialog();
    }
    static void OpenMotifFillDialog() {
        PES5_OpenMotifFillDialog();
    }
    static void OpenMotifStrokeDialog() {
        PES5_OpenMotifStrokeDialog();
    }

    static void SwapColor( int objIndex, int blockIndex, int direction ) { // direction: +1 or -1
        //if((objIndex + direction) >= 0 && (objIndex + direction) < PES5_GetObjectCount() ) {
        if(objIndex >= 0 && objIndex < PES5_GetObjectCount() ) {
            auto pesdata = pesGetDocument()->getDataObject(objIndex);
            auto blocksize = pesdata->fillBlocks.size();
            if ((blockIndex + direction) >= 0 && (blockIndex + direction) < blocksize) {
                if (PES5_ColorBlockSwap(pesdata.get(),
                                        blockIndex,
                                        blockIndex + direction)) {
                    PES5_UpdatePesImage(objIndex);
                }
            }
        }        
    }

    static void SetColor( int objIndex, int blockIndex, int colorIndex ) {
        SkDebugf( "SetColor: %i %i %i\n", objIndex, blockIndex, colorIndex );
        pesGetDocument()->getDataObject(objIndex)
            ->fillBlocks[blockIndex].setColorFromIndex(colorIndex);

        PES5_UpdatePesImage(objIndex);
    }

    static bool hasStitch( int objIndex ) {
        int nf = pesGetDocument()->getDataObject(objIndex)->fillBlocks.size();
        int ns = pesGetDocument()->getDataObject(objIndex)->strokeBlocks.size();
        int n = nf + ns;
        return (n != 0) ? true : false;
    }

    static void RemoveStitch( int objIndex ) {
        pesGetDocument()->getDataObject(objIndex)->fillBlocks.clear();
        pesGetDocument()->getDataObject(objIndex)->strokeBlocks.clear();

        PES5_UpdatePesImage(objIndex);       
        PES5_UpdateGUIRightPanel();          
    }
    static std::string GetPathID( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.path_id;
    }
    static std::string GetPathGroupID( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.group_id;
    }    
    static std::string GetPathFillColor( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        pesColor c = p.getFillColor();
        char sz[32];
        sprintf( sz, "#%02X%02X%02X", c.r, c.g, c.b );
        return std::string( sz );
    }
    static int GetPathFillAngle( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return (int)p.fillAngle;
    }
    static float GetPathFillDensity( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.fillDensity;
    }    
    static float GetPathFillPullCompensate( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.fillPullCompensate;
    }     
    static float GetPathStrokeDensity( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.strokeDensity;
    }    
    static float GetPathStrokePullCompensate( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.strokePullCompensate;
    }     
    static std::string GetPathStrokeColor( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        pesColor c = p.getStrokeColor();
        char sz[32];
        sprintf( sz, "#%02X%02X%02X", c.r, c.g, c.b );
        return std::string( sz );
    }
    static float GetPathStrokeWidth( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        float w = p.getStrokeWidth();
        return w;
    }
    static void SetPathFillColor( int objIndex, int pathIndex, int colorIndex ) {
        pesColor c = pesGetBrotherColor( colorIndex );
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setFillColor( c );
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }
    }
    static void SetPathFillAngle( int objIndex, int pathIndex, int fillAngle ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].fillAngle = fillAngle;
    }
    static void SetPathFillDensity( int objIndex, int pathIndex, float fillDensity ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].fillDensity = fillDensity;
    }        
    static void SetPathFillPullCompensate( int objIndex, int pathIndex, float fillPullCompensate ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].fillPullCompensate = fillPullCompensate;
    }        
    static void SetPathStrokeDensity( int objIndex, int pathIndex, float strokeDensity ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].strokeDensity = strokeDensity;
    }        
    static void SetPathStrokePullCompensate( int objIndex, int pathIndex, float strokePullCompensate ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].strokePullCompensate = strokePullCompensate;
    }        
    static void SetPathGroupFillType( int objIndex, std::string group_id, int fillType, float fillAngle ) {
        SkDebugf( "SetPathGroupFillType: %i %s %i %0.2f\n", objIndex, group_id.c_str(), fillType, fillAngle );
        for( int i=0; i<pesGetDocument()->getDataObject(objIndex)->paths.size(); i++ ) {
            pesPath &path = pesGetDocument()->getDataObject(objIndex)->paths[i];
            if( group_id.length() ) {
                if( pesIsStringStartsWith( path.group_id, group_id ) ) {
                    pesGetDocument()->getDataObject(objIndex)->paths[i].fillType = fillType;
                    pesGetDocument()->getDataObject(objIndex)->paths[i].fillAngle = fillAngle;
                }
            }
        }
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }        
    }
    static void SetPathFillTypeById( int objIndex, std::string path_id, int fillType, float fillAngle ) {
        SkDebugf( "SetPathFillTypeByID: %i %s %i %0.2f\n", objIndex, path_id.c_str(), fillType, fillAngle );
        for( int i=0; i<pesGetDocument()->getDataObject(objIndex)->paths.size(); i++ ) {
            pesPath &path = pesGetDocument()->getDataObject(objIndex)->paths[i];
            if( path_id.length() ) {
                if( path.path_id == path_id ) {
                    pesGetDocument()->getDataObject(objIndex)->paths[i].fillType = fillType;
                    pesGetDocument()->getDataObject(objIndex)->paths[i].fillAngle = fillAngle;
                }
            }
        }
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }         
    }
    static void SetPathGroupRoleType( int objIndex, std::string group_id, int roleType, float roleOffset ) {
        for( int i=0; i<pesGetDocument()->getDataObject(objIndex)->paths.size(); i++ ) {
            pesPath &path = pesGetDocument()->getDataObject(objIndex)->paths[i];
            if( group_id.length() ) {
                if( pesIsStringStartsWith( path.group_id, group_id ) ) {
                    pesGetDocument()->getDataObject(objIndex)->paths[i].roleType = roleType;
                    pesGetDocument()->getDataObject(objIndex)->paths[i].roleOffset = roleOffset;
                }
            }
        }
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }        
    }
    static void SetPathRoleTypeById( int objIndex, std::string path_id, int roleType, float roleOffset ) {
        for( int i=0; i<pesGetDocument()->getDataObject(objIndex)->paths.size(); i++ ) {
            pesPath &path = pesGetDocument()->getDataObject(objIndex)->paths[i];
            if( path_id.length() ) {
                if( path.path_id == path_id ) {
                    pesGetDocument()->getDataObject(objIndex)->paths[i].fillType = roleType;
                    pesGetDocument()->getDataObject(objIndex)->paths[i].roleOffset = roleOffset;
                }
            }
        }
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }         
    }
    static int GetPathFillType( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.fillType;
    }
    static void SetPathFillType( int objIndex, int pathIndex, int fillType ) {
        //SkDebugf( "SetPathFillType: %i %i %i\n", objIndex, pathIndex, fillType );
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].fillType = fillType;
        if( fillType == 0 ) {
            pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setFilled( false );
        }
        else if( fillType == 1 ) {
            pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setFilled( true );
        }
        //pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setFilled( fillType ? true : false );
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyFill();
        }
    }
    static int GetPathFlags( int objIndex, int pathIndex ) {
        return pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].flags;
    }
    static void SetPathFlags( int objIndex, int pathIndex, int flags ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].flags = flags;
    }

    static void SetPathStrokeColor( int objIndex, int pathIndex, int colorIndex ) {
        pesColor c = pesGetBrotherColor( colorIndex );
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setStrokeColor( c );
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyStroke();
        }
    }
    static void SetPathStrokeWidth( int objIndex, int pathIndex, float width ) {
        pesGetDocument()->getDataObject(objIndex)->paths[pathIndex].setStrokeWidth( width );
        if( hasStitch( objIndex ) ) {
            pesGetDocument()->getDataObject(objIndex)->applyStroke();
        }
    }    
    static bool GetPathVisible( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        return p.bVisible;
    }    
    static int32_t GetPathType( int objIndex, int pathIndex ) {
        pesPath p = pesGetDocument()->getDataObject(objIndex)->paths[pathIndex];
        bool iss = p.isStroke();
        bool isf = p.isFill();

        int ret = 0;

        if( iss ) {
            ret |= 1;
        }
        if( isf ) {
            ret |= 2;
        }
        return ret;
    }
    static void ShowPathBound( int objIndex, int pathIndex ) {
        if( objIndex < 0 ) {
            return;
        }
        pesGetDocument()->getDataObject(objIndex)->showPathBoundIndex = pathIndex;
    }
    static void DeletePath( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            pes->paths[pathIndex].clear();
            pes->paths.erase(pes->paths.begin() + pathIndex);

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }        

    }
    static void UniteSimilarPath( int objIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();
        for( int pathIndex = 0; pathIndex < pes->paths.size()-1; pathIndex++ ) {
            pesPath &tpath = pes->paths[pathIndex];
            pesPath &npath = pes->paths[pathIndex+1];
            if( tpath.path_id == npath.path_id &&
                tpath.isFill() == npath.isFill() &&
                tpath.isStroke() == npath.isStroke() &&
                tpath.getStrokeWidth() == npath.getStrokeWidth() &&
                tpath.getStrokeColor().r == npath.getStrokeColor().r &&
                tpath.getStrokeColor().g == npath.getStrokeColor().g &&
                tpath.getStrokeColor().b == npath.getStrokeColor().b &&
                tpath.getFillColor().r == npath.getFillColor().r &&
                tpath.getFillColor().g == npath.getFillColor().g &&
                tpath.getFillColor().b == npath.getFillColor().b
                ) {
                    UniteNextPath( objIndex, pathIndex );
                    pathIndex--;
                }
        }
    }
    static void UniteNextPath( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();
        if (pathIndex >= 0 && pathIndex < pes->paths.size()-1) {
            pesPath &tpath = pes->paths[pathIndex];
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();
            std::string path_id = tpath.path_id;
            std::string group_id = tpath.group_id;

            SkPath path1 = toSk(pes->paths[pathIndex]);
            SkPath path2 = toSk(pes->paths[pathIndex+1]);

            Op(path1, path2, SkPathOp::kUnion_SkPathOp, &path1);
            pesPath path = toPes( path1 );
            path.setStrokeColor( strokeColor );
            path.setFillColor( fillColor );
            path.setStrokeWidth( strokeWidth );
            path.setFilled( isFill );  
            path.path_id = path_id;
            path.group_id = group_id;

            pes->paths[pathIndex].clear();
            pes->paths[pathIndex] = path;     

            pes->paths[pathIndex+1].clear();
            pes->paths.erase(pes->paths.begin() + (pathIndex + 1)); 

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }
    }
    static void EraseUnderPaths( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();
        if (pathIndex >= 1 && pathIndex < pes->paths.size()) {
            SkPath eraserPath = toSk(pes->paths[pathIndex]);
            for( int ip=0; ip<pathIndex; ip++ ) {
                pesPath &tpath = pes->paths[ip];
                if( !tpath.bVisible )
                    continue;
                bool isFill = tpath.isFill();
                bool isStroke = tpath.isStroke();
                float strokeWidth = tpath.getStrokeWidth();
                pesColor strokeColor = tpath.getStrokeColor();
                pesColor fillColor = tpath.getFillColor();
                SkDebugf( "Diff S %i F %i\n", isStroke, isFill );

                SkPath targetPath = toSk(tpath);                
                if( isStroke && !isFill ) {
                    SkPath outlinePath = SkiaPathStroke( targetPath, tpath.getStrokeWidth()/2 * pes->parameter.ppefScaleX );
                    Op(outlinePath, eraserPath, SkPathOp::kDifference_SkPathOp, &targetPath);
                    //Op(targetPath, outlinePath, SkPathOp::kReverseDifference_SkPathOp, &targetPath);

                    pesPath path = toPes( targetPath );
                    path.setStrokeColor( strokeColor );
                    path.setFillColor( strokeColor );
                    path.setStrokeWidth( 0 );
                    path.setFilled( true );
                    pes->paths[ip].clear();
                    pes->paths[ip] = path;                      


                    //pesPath path = toPes( targetPath );
                    //path.setStrokeColor( strokeColor );
                    //path.setFillColor( fillColor );
                    //path.setStrokeWidth( strokeWidth );
                    //path.setFilled( isFill );
                    //pes->paths[ip].clear();
                    //pes->paths[ip] = path;                      

                }
                else {
                    Op(eraserPath, targetPath, SkPathOp::kReverseDifference_SkPathOp, &targetPath);
                pesPath path = toPes( targetPath );
                path.setStrokeColor( strokeColor );
                path.setFillColor( fillColor );
                path.setStrokeWidth( strokeWidth );
                path.setFilled( isFill );

                pes->paths[ip].clear();
                    pes->paths[ip] = path;                      
                }
              
            }

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel(); 
        }
    }
    static void SimplifyPath( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            SkDebugf( "Simplify\n" );
            pesPath &tpath = pes->paths[pathIndex];
            
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();

            vector<pesPath> subpath = tpath.getSubPath();
            SkPath skPath = toSk(tpath);
            
            if(subpath.size()){
                skPath = toSk(subpath[0]);
                /*
                for( int i=0; i<1; i++ ) {
                    pesPath p = subpath[i];
                    SkPath pp = toSk(p);
                    SkDebugf( "Subpath: %i\n", i );
                    Op(skPath, pp, SkPathOp::kDifference_SkPathOp, &skPath);
                }
                */
            }
                        
            Simplify(skPath, &skPath);
            pesPath path = toPes( skPath );

            path.setStrokeColor( strokeColor );
            path.setFillColor( fillColor );
            path.setStrokeWidth( strokeWidth );
            path.setFilled( isFill );

            pes->paths[pathIndex].clear();
            pes->paths[pathIndex] = path;

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }
    }
    static void PathUp( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 1 && pathIndex < pes->paths.size()) {
            std::swap(pes->paths[pathIndex], pes->paths[pathIndex-1]);
            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }        
    }
    static void PathDown( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()-1) {
            std::swap(pes->paths[pathIndex], pes->paths[pathIndex+1]);
            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }        
    }

    static void SetPathVisible( int objIndex, int pathIndex, bool visible ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            pes->paths[pathIndex].bVisible = visible;
        }        
    }
    static SkPath SkiaPathStroke( SkPath &skPath, float value ) {
        SkPath newPath;
        
        SkPaint paint;
        paint.setStyle( SkPaint::Style::kStroke_Style );
        paint.setStrokeWidth( value * 2 );
        paint.setStrokeCap( SkPaint::Cap::kButt_Cap);
        paint.setStrokeJoin( SkPaint::Join::kMiter_Join);
        //paint.getFillPath( skPath, &newPath );
        skpathutils::FillPathWithPaint(skPath, paint, &newPath);
        return newPath;
    }
    static void PathInset( int objIndex, int pathIndex, float value ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            SkDebugf( "Path Inset\n" );
            pesPath &tpath = pes->paths[pathIndex];
            
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();

            SkPath skPath = toSk(tpath);

            // Do inset  
            SkPath outlinePath = SkiaPathStroke( skPath, value ); 
            Simplify(outlinePath, &outlinePath);
            Op(outlinePath, skPath, SkPathOp::kReverseDifference_SkPathOp, &outlinePath);
            //Simplify(outlinePath, &outlinePath);

            pesPath path = toPes( outlinePath );

            path.setStrokeColor( strokeColor );
            path.setFillColor( fillColor );
            path.setStrokeWidth( strokeWidth );
            path.setFilled( isFill );

            pes->paths[pathIndex].clear();
            pes->paths[pathIndex] = path;

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }        
    }

    static void CreateOutlinePath( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            SkDebugf( "Create Outline Path\n" );
            pesPath &tpath = pes->paths[pathIndex];
            
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();

            std::vector<pesPolyline>outlines = tpath.getOutline();
            SkDebugf( "Outline: %i\n", (int)outlines.size() );

            SkPath skPath = toSk(tpath);

            // Do inset  

            pesPath path = toPes( skPath );

            path.setStrokeColor( strokeColor );
            path.setFillColor( fillColor );
            path.setStrokeWidth( strokeWidth );
            path.setFilled( isFill );

            pes->paths[pathIndex].clear();
            pes->paths[pathIndex] = path;

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();  
        }
    }

    static void SeparateSubpath( int objIndex, int pathIndex ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            SkDebugf( "SeparateSubpath\n" );
            pesPath &tpath = pes->paths[pathIndex];
            
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();

            std::vector<pesPath>subpaths = tpath.getSubPath();
            SkDebugf( "Subpath: %i\n", (int)subpaths.size() );

            for( int i=0; i<subpaths.size(); i++ ) {
                pesPath path = subpaths[i];

                path.setStrokeColor( strokeColor );
                path.setFillColor( fillColor );
                path.setStrokeWidth( strokeWidth );
                path.setFilled( isFill );

                pes->paths.insert(pes->paths.begin() + (pathIndex + i + 1), path );
            }

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();  
        }
    }

    static void PathOutset( int objIndex, int pathIndex, float value ) {
        pesData * pes = pesGetDocument()->getDataObject(objIndex).get();

        if (pathIndex >= 0 && pathIndex < pes->paths.size()) {
            SkDebugf( "Path Outset\n" );
            pesPath &tpath = pes->paths[pathIndex];
            
            bool isFill = tpath.isFill();
            bool isStroke = tpath.isStroke();
            float strokeWidth = tpath.getStrokeWidth();
            pesColor strokeColor = tpath.getStrokeColor();
            pesColor fillColor = tpath.getFillColor();

            SkPath skPath = toSk(tpath);

            // Do Outset  
            SkPath outlinePath = SkiaPathStroke( skPath, value ); 
            Simplify(outlinePath, &outlinePath);
            Op(outlinePath, skPath, SkPathOp::kDifference_SkPathOp, &outlinePath);
            //Simplify(outlinePath, &outlinePath);

            pesPath path = toPes( outlinePath );

            path.setStrokeColor( strokeColor );
            path.setFillColor( fillColor );
            path.setStrokeWidth( strokeWidth );
            path.setFilled( isFill );

            pes->paths[pathIndex].clear();
            pes->paths[pathIndex] = path;

            if( hasStitch( objIndex ) ) {
                pes->applyFill();
                pes->applyStroke();
            }
            PES5_UpdatePesImage(objIndex);       
            PES5_UpdateGUIRightPanel();             
        }        
    }

    static void SetLayerVisible( int index, bool visible ) {
        if (index < 0 || index >= pesGetDocument()->getObjectCount()) {
            return;
        }
        pesGetDocument()->setDataParameterVisible(index, visible);
        PES5_UpdatePesImage(index);       
        PES5_UpdateGUIRightPanel();             
    }

    static bool GetLayerVisible(int index) {
        if (index < 0 || index >= pesGetDocument()->getObjectCount()) {
            return false;
        }
        bool visible = pesGetDocument()->getDataParameter(index).visible;
        return visible;
    }

    static void SetLayerLocked( int index, bool locked ) {
        if (index < 0 || index >= pesGetDocument()->getObjectCount()) {
            return;
        }
        auto parameter = pesGetDocument()->getDataParameter(index);
        if (parameter.type == pesData::ObjectType::OBJECT_TYPE_BACKGROUND) {
            return;
        }
        pesGetDocument()->setDataParameterLocked( index, locked );
        PES5_UpdatePesImage(index);       
        PES5_UpdateGUIRightPanel();             
    }

    static bool GetLayerLocked(int index) {
        if (index < 0 || index >= pesGetDocument()->getObjectCount()) {
            return false;
        }
        bool locked = pesGetDocument()->getDataParameter(index).locked;
        return locked;
    }

    static bool ImportDataAs(const std::string& data, const std::string as) {
        return ImportDataAs(data, as, nullptr);
    }

    static bool ImportDataAs(const std::string& data, const std::string as, const std::string options) {
        return PES5_ImportBufferAs(pesBuffer(data.c_str(), data.size()), as, options);
    }

    static Uint8Array ExportDataAs(const std::string as) {
        return ExportDataAs(as, nullptr);
    }

    static Uint8Array ExportDataAs(const std::string as, const std::string options) {
        pesBuffer buff = PES5_ExportBufferAs(as, options);
        return emscripten::val(typed_memory_view(buff.size(), (uint8_t*)buff.getData())).call<Uint8Array>("slice");
    }

    static Uint8Array GetLayerImageData(int layerIndex) {
        const auto img = PES5_GetPesImageThumbnail(layerIndex);
        if (img.get() == nullptr) {
            //return emscripten::val(nullptr);
            return emscripten::val(typed_memory_view(0, (uint8_t*)nullptr)).call<Uint8Array>("slice");
        }
        sk_sp<SkData> pngData = img->encodeToData();
        return emscripten::val(typed_memory_view(pngData->size(), (uint8_t*)pngData->data())).call<Uint8Array>("slice");
    }

    static Uint8Array GetSVGPartTypeImageData(int layerIndex, std::string parttype) {
        const auto img = PES5_GetSVGPartTypeImage(layerIndex, parttype);
        if (img.get() == nullptr) {
            //return emscripten::val(nullptr);
            return emscripten::val(typed_memory_view(0, (uint8_t*)nullptr)).call<Uint8Array>("slice");
        }
        sk_sp<SkData> pngData = img->encodeToData();
        return emscripten::val(typed_memory_view(pngData->size(), (uint8_t*)pngData->data())).call<Uint8Array>("slice");
    }

    static void SetDarkMode(bool isDarkMode) {
        PES5_SetDarkMode(isDarkMode);
    }
    
    //---------------------------------------
    // UndoRedo
    //---------------------------------------
    static bool CanUndo(){
        return PES5_CanUndo();
    }
    
    static bool CanRedo(){
        return PES5_CanRedo();
    }
    
    static void Undo(){
        PES5_Undo();
    }
    
    static void Redo(){
        PES5_Redo();
    }
    
    static void UndoRedoClear(){
        PES5_UndoRedoClear();
    }
    //---------------------------------------
    //---------------------------------------
};

long PES5Template::FLAG_HIDE_LAYER_BOX = PES5_FLAG_HIDE_LAYER_BOX;
long PES5Template::FLAG_HIDE_TITLE_BAR = PES5_FLAG_HIDE_TITLE_BAR;
long PES5Template::FLAG_HIDE_TOOL_BAR = PES5_FLAG_HIDE_TOOL_BAR;
long PES5Template::FLAG_HIDE_TOOL_BOX = PES5_FLAG_HIDE_TOOL_BOX;
long PES5Template::FLAG_HIDE_RULER = PES5_FLAG_HIDE_RULER;
long PES5Template::FLAG_NO_WELCOME = PES5_FLAG_NO_WELCOME;
long PES5Template::FLAG_HIDE_JUMP_STITCH = PES5_FLAG_HIDE_JUMP_STITCH;
long PES5Template::FLAG_RENDER_TEXTURE = PES5_FLAG_RENDER_TEXTURE;

EMSCRIPTEN_BINDINGS(PES5Template) {
    class_<PES5Template>("PES5Template")
        .class_property("FLAG_HIDE_TITLE_BAR", &PES5Template::FLAG_HIDE_TITLE_BAR)
        .class_property("FLAG_HIDE_TOOL_BAR", &PES5Template::FLAG_HIDE_TOOL_BAR)
        .class_property("FLAG_HIDE_TOOL_BOX", &PES5Template::FLAG_HIDE_TOOL_BOX)
        .class_property("FLAG_HIDE_LAYER_BOX", &PES5Template::FLAG_HIDE_LAYER_BOX)
        .class_property("FLAG_HIDE_RULER", &PES5Template::FLAG_HIDE_RULER)
        .class_property("FLAG_NO_WELCOME", &PES5Template::FLAG_NO_WELCOME)
        .class_property("FLAG_HIDE_JUMP_STITCH", &PES5Template::FLAG_HIDE_JUMP_STITCH)
        .class_property("FLAG_RENDER_TEXTURE", &PES5Template::FLAG_RENDER_TEXTURE)
        .class_function("Init", &PES5Template::Init)
        .class_function("UpdateImages", &PES5Template::UpdateImages)
        .class_function("UpdateImage", &PES5Template::UpdateImage)
        .class_function("GetObjectSelectedIndex", &PES5Template::GetObjectSelectedIndex)
        .class_function("SetObjectSelectedIndex", select_overload<void(int)>(&PES5Template::SetObjectSelectedIndex))
        .class_function("SetObjectSelectedIndex", select_overload<void(int, bool)>(&PES5Template::SetObjectSelectedIndex))
        .class_function("GetResourcePath", &PES5Template::GetResourcePath)
        .class_function("SetResourcePath", &PES5Template::SetResourcePath)
        .class_function("NewDocument", &PES5Template::NewDocument)
        .class_function("SetDocumentName", &PES5Template::SetDocumentName)
        .class_function("GetDocumentName", &PES5Template::GetDocumentName)
        .class_function("GetObjectCount", &PES5Template::GetObjectCount)
        .class_function("UpdateGUIRightPanel", &PES5Template::UpdateGUIRightPanel)
        .class_function("UpdatePesImage", &PES5Template::UpdatePesImage)
        .class_function("OpenTracingDialog", &PES5Template::OpenTracingDialog)
        .class_function("LoadPatternAndMotif", &PES5Template::LoadPatternAndMotif)
        .class_function("GetLayerTypeString", &PES5Template::GetLayerTypeString)
        .class_function("GetLayerTextString", &PES5Template::GetLayerTextString)
        .class_function("IsLayerLocked", &PES5Template::IsLayerLocked)
        .class_function("GetLayerImageString", &PES5Template::GetLayerImageString)
        .class_function("SelectLayer", &PES5Template::SelectLayer)
        //.class_function("AllLayerMerge", &PES5Template::AllLayerMerge)
        .class_function("ReplaceWithDefaultFont", &PES5Template::ReplaceWithDefaultFont)
        .class_function("SetZoom", &PES5Template::SetZoom)
        .class_function("GetZoom", &PES5Template::GetZoom)
        .class_function("SetPan", &PES5Template::SetPan)
        .class_function("GetPan", &PES5Template::GetPan)
        .class_function("UpdateTTFText", &PES5Template::UpdateTTFText)
        .class_function("UpdateSVG", &PES5Template::UpdateSVG)
        .class_function("OpenPatternFillDialog", &PES5Template::OpenPatternFillDialog)
        .class_function("OpenMotifFillDialog", &PES5Template::OpenMotifFillDialog)
        .class_function("OpenMotifStrokeDialog", &PES5Template::OpenMotifStrokeDialog)
        .class_function("SwapColor", &PES5Template::SwapColor)
        .class_function("SetColor", &PES5Template::SetColor)
        .class_function("hasStitch", &PES5Template::hasStitch)
        .class_function("RemoveStitch", &PES5Template::RemoveStitch)
        .class_function("GetPathType", &PES5Template::GetPathType)
        .class_function("GetPathVisible", &PES5Template::GetPathVisible)
        .class_function("GetPathFillType", &PES5Template::GetPathFillType)
        .class_function("GetPathFillColor", &PES5Template::GetPathFillColor)
        .class_function("GetPathFillAngle", &PES5Template::GetPathFillAngle)
        .class_function("GetPathFillDensity", &PES5Template::GetPathFillDensity)
        .class_function("GetPathStrokeColor", &PES5Template::GetPathStrokeColor)
        .class_function("GetPathStrokeWidth", &PES5Template::GetPathStrokeWidth)
        .class_function("SetPathStrokeDensity", &PES5Template::SetPathStrokeDensity)
        .class_function("SetPathStrokePullCompensate", &PES5Template::SetPathStrokePullCompensate)
        .class_function("SetPathFillColor", &PES5Template::SetPathFillColor)
        .class_function("SetPathFillAngle", &PES5Template::SetPathFillAngle)
        .class_function("SetPathFillDensity", &PES5Template::SetPathFillDensity)
        .class_function("SetPathFillPullCompensate", &PES5Template::SetPathFillPullCompensate)
        .class_function("SetPathFillType", &PES5Template::SetPathFillType)
        .class_function("SetPathFillTypeById", &PES5Template::SetPathFillTypeById)
        .class_function("SetPathGroupRoleType", &PES5Template::SetPathGroupRoleType)
        .class_function("SetPathRoleTypeById", &PES5Template::SetPathRoleTypeById)
        .class_function("SetPathGroupFillType", &PES5Template::SetPathGroupFillType)
        .class_function("SetPathStrokeColor", &PES5Template::SetPathStrokeColor)
        .class_function("SetPathStrokeWidth", &PES5Template::SetPathStrokeWidth)
        .class_function("ShowPathBound", &PES5Template::ShowPathBound)
        .class_function("DeletePath", &PES5Template::DeletePath)
        .class_function("EraseUnderPaths", &PES5Template::EraseUnderPaths)
        .class_function("PathUp", &PES5Template::PathUp)
        .class_function("PathDown", &PES5Template::PathDown)
        .class_function("UniteNextPath", &PES5Template::UniteNextPath)
        .class_function("UniteSimilarPath", &PES5Template::UniteSimilarPath)
        .class_function("SimplifyPath", &PES5Template::SimplifyPath)
        .class_function("SetPathVisible", &PES5Template::SetPathVisible)
        .class_function("PathInset", &PES5Template::PathInset)
        .class_function("PathOutset", &PES5Template::PathOutset)
        .class_function("CreateOutlinePath", &PES5Template::CreateOutlinePath)
        .class_function("SeparateSubpath", &PES5Template::SeparateSubpath)
        .class_function("GetPathFlags", &PES5Template::GetPathFlags)
        .class_function("ExportPNG", &PES5Template::ExportPNG)
        .class_function("SetPathFlags", &PES5Template::SetPathFlags)
        .class_function("GetPathID", &PES5Template::GetPathID)
        .class_function("GetPathGroupID", &PES5Template::GetPathGroupID)
        .class_function("GetSVGPartTypeImageString", &PES5Template::GetSVGPartTypeImageString)
        .class_function("SetLayerVisible", &PES5Template::SetLayerVisible)
        .class_function("GetLayerVisible", &PES5Template::GetLayerVisible)
        .class_function("SetLayerLocked", &PES5Template::SetLayerLocked)
        .class_function("GetLayerLocked", &PES5Template::GetLayerLocked)
        .class_function("ImportDataAs", select_overload<bool(const string&, const std::string)>(&PES5Template::ImportDataAs))
        .class_function("ImportDataAs", select_overload<bool(const string&, const std::string, const std::string)>(&PES5Template::ImportDataAs))
        .class_function("ExportDataAs", select_overload<Uint8Array(const std::string)>(&PES5Template::ExportDataAs))
        .class_function("ExportDataAs", select_overload<Uint8Array(const std::string, const std::string)>(&PES5Template::ExportDataAs))
        .class_function("GetLayerImageData", &PES5Template::GetLayerImageData)
        .class_function("GetSVGPartTypeImageData", &PES5Template::GetSVGPartTypeImageData)
        .class_function("SetDarkMode", &PES5Template::SetDarkMode)
    
        .class_function("HorizontalFlip", &PES5_HorizontalFlip)
        .class_function("VerticalFlip", &PES5_VerticalFlip)
    
        .class_function("AddPesData", &PES5_AddPesData)

        ////////////////////////////////
        // Multi-Select
        ////////////////////////////////
        //int PES5_MSSelectCount();
        //void PES5_MSClearSelection();
        //void PES5_MSSelectAll();
        //bool PES5_MSIsSelecAll();
        //bool PES5_MSSelect(int idx, bool multiSelect=true);
        //bool PES5_MSDeSelect(int idx);
        //pesRectangle PES5_MSGetBoundingBox();
        //bool PES5_MSIsScalable();
        //void PES5_MSTranslate(float tx, float ty);
        //void PES5_MSScale(float sx, float sy); // Note: scale from left-top
        //void PES5_MSRotate(float degree); // Note: rotate around getBoundingBox().getCenter()
        .class_function("MSSelectCount", &PES5_MSSelectCount)
        .class_function("MSClearSelection", &PES5_MSClearSelection)
        .class_function("MSSelectAll", optional_override([]() -> int {
            PES5_MSSelectAll();
            return PES5_MSSelectCount();
        }))
        .class_function("MSIsSelecAll", &PES5_MSIsSelecAll)
        .class_function("MSIsSelected", &PES5_MSIsSelected)
        .class_function("MSSelect", optional_override([](int idx, bool multiSelect) -> bool {
            return PES5_MSSelect(idx, multiSelect);
        }))
        .class_function("MSSelect", optional_override([](int idx) -> bool {
            return PES5_MSSelect(idx, true);
        }))
        .class_function("MSSelectToggle", optional_override([](int idx) -> bool {
            return PES5_MSSelect(idx, true);
        }))
        .class_function("MSSelectOnly", optional_override([](int idx) -> bool {
            return PES5_MSSelect(idx, false);
        }))
        .class_function("MSDeSelect", &PES5_MSDeSelect)
        .class_function("MSGetBoundingBox", &PES5_MSGetBoundingBox)
        .class_function("MSIsScalable", &PES5_MSIsScalable)
        .class_function("MSTranslate", &PES5_MSTranslate)
        .class_function("MSScale", &PES5_MSScale)
        .class_function("MSRotate", &PES5_MSRotate)
        .class_function("MSRotate2", &PES5_MSRotate2)
        .class_function("MSTranslateToCenter", &PES5_MSTranslateToCenter)
        .class_function("MSPositonScale", &PES5_MSPositonScale)
        .class_function("MSDelete", &PES5_MSDelete)

        //---------------
        // UndoRedo
        //----------------
        .class_function("CanUndo", &PES5Template::CanUndo)
        .class_function("CanRedo", &PES5Template::CanRedo)
        .class_function("Undo", &PES5Template::Undo)
        .class_function("Redo", &PES5Template::Redo)
        .class_function("UndoRedoClear", &PES5Template::UndoRedoClear)
    
        .class_function("ApplyParameter", &PES5_ApplyParameter)
    
    ;
}
