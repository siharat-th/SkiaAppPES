#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/core/SkSamplingOptions.h"
#include "include/docs/SkPDFDocument.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkStream.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkFontMetrics.h"
#include "SVGEditor.h"
#include "include/svg/SkSVGCanvas.h"
#include "SVGDomEditor2.h"
#include "pesBuffer.hpp"
#include "pesData.hpp"
//#include <src/xml/SkXMLWriter.h>
//#include <src/svg/SkSVGDevice.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <modules/svg/include/SkSVGRect.h>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <modules/svg/include/SkSVGText.h>
//const double PI = M_PI;

const int _paperWidth[] = { 52, 74, 105, 148, 210, 297, 420, 594, 841 };
const int _paperHeight[] = { 74, 105, 148, 210, 297, 420, 594, 841, 1188 };
const char *_paperName[] = { "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", "n/a" };
const int _numPaperType = 9;

DecSVGDomEditor::DecSVGDomEditor( const char *uri ) :
    DecSVGDom( NULL ) {
    _isFocusable = true;
    _border = true;
    _borderWidth = 1;
    _border_color = 0xFF808080;
    _focus_borderWidth = 2;
    _focus_border_color = SK_ColorGREEN;

    _onSelectChanged = NULL;

    //Samsung SA300 Monitor
    //https://downloadcenter.samsung.com/content/UM/201105/20110518114703016/BN59-01127A-08Eng.pdf
    //Pixel Pitch: 0.27675 mm(H) x 0.27675 mm(V)
    //DPI: 25.4/0.27675 = 91.7795844625113
    
    _dpi = 96;
    _bEnableMouseHoverMode = false;
    _bEnableShowAllBoundMode = false;

    _testDraw = false;
    pImageLogo = new DecImage();
    pImageLogo->load( "images/MP-LOGO-C.png" );

    pSVGEditor = NULL;

    pSelectedNode = NULL;
    pHoverNode = NULL;
    pPressedNode = NULL;
    pDragNode = NULL;

    drawFit = true;

    contentCenterOffsetX_72 = 0;
    contentCenterOffsetY_72 = 0;

    setPaperSize( "A1" );

    minPaperWidth_mm = 0;
    minPaperHeight_mm = 0;  

    _draw_hook = NULL;  
    _predraw_hook = NULL;

    _showMinPaperBounds = false;
    _showVisibleContentBounds = false;   
    _showTileGrid = false; 

    _tileW = 0;
    _tileH = 0;
    _numTileX = 0;
    _numTileY = 0;

    _tileTop = 0;
    _tileRight = 0;
    _tileBottom = 0;
    _tileLeft = 0;

    _pdfStream = nullptr;
    _pdfDoc = nullptr;

    _svgViewScale = 1.0;
    _svgViewMoving = false;
    _svgViewPosnX = 0;
    _svgViewPosnY = 0;

    _svgNodeMoving = false;

    _bMoveContentToCenter = true;
    
    size( -1, -1 );
    border(false);
    bg_color(0xFFE0E0E0);

    //pSelectedNodePath = NULL;

    _visibleContentBounds.setEmpty();
    
    if( uri ) {
        load( uri );
    }
}

DecSVGDomEditor::~DecSVGDomEditor() {

}

DecWidget & DecSVGDomEditor::onSelectChanged(std::function<void(DecSVGDomEditor &svgDomEditor)> callback) {
    _onSelectChanged = callback;
    return *this;
}

void DecSVGDomEditor::update() {
    //SkDebugf("DecSVGDomEditor::update: parent-size:[%d, %d]\n",
    //         this->_parent->width(),
    //         this->_parent->height());
    static int snParentHeight = this->_parent->height();
    int nParentHeight = this->_parent->height();
    if (snParentHeight != nParentHeight) {
        snParentHeight = nParentHeight;
        dirty(true, true);
    }
}

void DecSVGDomEditor::adjustWidth(int w) {
    if (_ow == 0 || _ow == -1) {
        // fixed: cache & dirty
        if (fWigetSize.fWidth > 0) {
            // fixed: cache & dirty
            _width = fWigetSize.fWidth;
        } else {
            fWigetSize.fWidth = _width;
        }
        if (w != _width) {
            dirty(true, true);
            _width = w;
            // fixed: cache & dirty
            fWigetSize.fWidth = w;
        }
        if (_width > _maxWidth) {
            _width = _maxWidth;
            // fixed: cache & dirty
            fWigetSize.fWidth = _maxWidth;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecSVGDomEditor::adjustHeight(int h) {
    if (_oh == 0 || _oh == -1) {
        if (fWigetSize.fHeight > 0) {
            // fixed: cache & dirty
            _height = fWigetSize.fHeight;
        } else {
            fWigetSize.fHeight = _height;
        }
        if (h != _height) {
            dirty(true, true);
            _height = h;
            // fixed: cache & dirty
            fWigetSize.fHeight = h;
        }
        if (_height > _maxHeight) {
            _height = _maxHeight;
            // fixed: cache & dirty
            fWigetSize.fHeight = _maxHeight;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}


bool DecSVGDomEditor::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) {
    if (!_isVisible) {
        return false;
    }
    if (!fDom) {
        return false;
    }

    //uint8_t u8mod = (uint8_t)mod;
    //if (u8mod != 0) {
    //    bool a5bits[5] = { 
    //        (bool)(u8mod & 0x01), 
    //        (bool)(u8mod & 0x02), 
    //        (bool)(u8mod & 0x04), 
    //        (bool)(u8mod & 0x08), 
    //        (bool)(u8mod & 0x10)
    //    };
    //    SkDebugf("cc mod: %u%u%u%u%u\n",
    //             a5bits[4],
    //             a5bits[3],
    //             a5bits[2],
    //             a5bits[1],
    //             a5bits[0]);
    //}

    if (skui::InputState::kDown == state) {
        if( ((int)mod & (int)skui::ModifierKey::kCommand) == (int)skui::ModifierKey::kCommand ||
            ((int)mod & (int)skui::ModifierKey::kControl) == (int)skui::ModifierKey::kControl ) {
            if( ((int)mod & (int)skui::ModifierKey::kShift) == (int)skui::ModifierKey::kShift) {
                if (skui::Key::kA == k) {
                    _bEnableShowAllBoundMode = !_bEnableShowAllBoundMode;
                    SkDebugf("Ctrl+Shift+A\n_bEnableShowAllBoundMode:%u\n", _bEnableShowAllBoundMode);
                    dirty(true);
                    return true;
                }
                else if (skui::Key::kS == k) {
                    _bEnableMouseHoverMode = !_bEnableMouseHoverMode;
                    SkDebugf("Ctrl+Shift+S\n_bEnableMouseHoverMode:%u\n", _bEnableMouseHoverMode);
                    
                    // clear hover
                    if (pHoverNode) {
                        pHoverNode = NULL;
                        dirty(true);
                    }

                    if (_bEnableMouseHoverMode) {
                        auto surface = SkSurface::MakeRasterN32Premul(100, 100);
                        SkCanvas* canvas = surface->getCanvas();
                        fDom->computeBoundingBoxs(canvas, _dpi);
                        //SkDebugf("computeBoundingBoxs success\n");
                    }
                    return true;
                } 
                else if (skui::Key::kD == k) {
                    // reset: zoom(scale), pos
                    if (_svgViewScale != 1.0f) {
                        _svgViewScale = 1.0f;
                        _svgViewPosnX = 0;
                        _svgViewPosnY = 0;
                        dirty(true);
                    }
                }
            }
        }
    }
    return false;
}

bool DecSVGDomEditor::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    DecWidget::onMouse(x, y, state, mod);

    static bool bDragTest = false;
    const float dx = x - _svgViewLastMouseX;
    const float dy = y - _svgViewLastMouseY;
    const bool bMove = dx != 0 || dy != 0;

    const bool bModIsCtrlOrCmd =
            (((int)mod & (int)skui::ModifierKey::kCommand) == (int)skui::ModifierKey::kCommand ||
             ((int)mod & (int)skui::ModifierKey::kControl) == (int)skui::ModifierKey::kControl);

    
    if (state == skui::InputState::kDown) {
        bDragTest = true;
        SkSVGNode* pNode = NULL;

        pNode = selectNodeByPosition(x, y, _svgViewScale, pSelectedNode, bModIsCtrlOrCmd);

        if (pNode) {
            if (pPressedNode != pNode) {
                pPressedNode = pNode;
                dirty(true);
            }
        }

        pDragNode = NULL;
    }
    else if( state == skui::InputState::kMove ) {
        if (bDragTest) {
            // threshold for drag start, it's distance ~8px from first pressed.
            if ((dx * dx + dy * dy) < 64)
                return true;
            bDragTest = false;
        }

        // use pPressedNode to consider perform to select or cancle this Node on mouse up.
        if (pPressedNode) {
            // enable draging if keymods is Ctrl|Cmd 
            if (bModIsCtrlOrCmd) {
                _svgNodeMoving = true;
                pDragNode = pPressedNode;
                pSelectedNode = NULL;
            } 
            else {
                _svgViewMoving = true;
            }

            pPressedNode = NULL;
            dirty(true);
        }

        if (pDragNode) {
            // use setAttribute() good way for save/export svg.
            translateNode(dx, dy, _svgViewScale, pDragNode);
            dirty( true );
        }
        else if( _svgViewMoving ) {
            _svgViewPosnX += dx;
            _svgViewPosnY += dy;
            dirty( true );
        }
    }
    else if( state == skui::InputState::kUp ) {
        _svgViewMoving = false;
        _svgNodeMoving = false;
        
        // update bboxs
        if (pDragNode) {
            auto surface = SkSurface::MakeRasterN32Premul(100, 100);
            SkCanvas* canvas = surface->getCanvas();
            fDom->computeBoundingBoxs(canvas, _dpi);
            //SkDebugf("computeBoundingBoxs success\n");

            if (pSelectedNode != pDragNode) {
                pSelectedNode = pDragNode;
                if (_onSelectChanged) {
                    _onSelectChanged(*this);
                }
            }

            dirty(true);
            pDragNode = NULL;
        }

        // update pSelectedNode
        if (pPressedNode) {
            if (bModIsCtrlOrCmd) {
                // SkDebugf("selected: %s\n", pSelectedNode ? pSelectedNode->tagName() : "none");
                if (pPressedNode == pSelectedNode) {
                    // SkDebugf("parent: %s\n", pPressedNode->pParent ? pPressedNode->pParent->tagName() : "none");
                    if (pPressedNode->pParent) {
                        pPressedNode = pPressedNode->pParent;
                        dirty(true);
                    }
                }
            }

            // toggle selected if same
            if (pSelectedNode != pPressedNode) {
                pSelectedNode = pPressedNode;
            } 
            else {
                pSelectedNode = NULL;
            }
            if (_onSelectChanged) {
                _onSelectChanged(*this);
            }
            dirty(true);
            pPressedNode = NULL;
        }
    }

    _svgViewLastMouseX = x;
    _svgViewLastMouseY = y;

    if (bMove) {
        if (_bEnableMouseHoverMode) {
            SkSVGNode* pNode = NULL;
            pNode = selectNodeByPosition(x, y, _svgViewScale, pHoverNode, false);
            if (pHoverNode != pNode) {
                dirty(true);
            }
            pHoverNode = pNode;
        }

        //if (fDom) {
        //    const SkSize szC = fDom->containerSize();
        //    const SkScalar vw = width();
        //    const SkScalar vh = height();
        //    const SkScalar svgw = szC.fWidth * _svgViewScale;
        //    const SkScalar svgh = szC.fHeight * _svgViewScale;
        //    const SkScalar svgx = _svgViewPosnX + (vw - svgw) / 2.0f;
        //    const SkScalar svgy = _svgViewPosnY + (vh - svgh) / 2.0f;
        //
        //    float sx = (x - svgx) / _svgViewScale;
        //    float sy = (y - svgy) / _svgViewScale;
        //    testpoint = {sx, sy};
        //    dirty(true); 
        //}
    }

    return false;
}

SkSVGNode* DecSVGDomEditor::selectNodeByPosition(float x, float y, float scale, SkSVGNode* pSelectedNode, bool bInboundSelectedOnly) {
    SkSVGNode* pNode = NULL;
    if (fDom) {
        const SkSize szC = fDom->containerSize();
        const SkScalar vw = width();
        const SkScalar vh = height();
        const SkScalar svgw = szC.fWidth * scale;
        const SkScalar svgh = szC.fHeight * scale;
        const SkScalar svgx = _svgViewPosnX + (vw - svgw) / 2.0f;
        const SkScalar svgy = _svgViewPosnY + (vh - svgh) / 2.0f;

        //const SkRect rect = SkRect::MakeXYWH(svgx, svgy, svgw, svgh);
        //if (!rect.contains(x, y)) { return NULL; }
        float sx = (x - svgx) / scale;
        float sy = (y - svgy) / scale;
        SkPoint point = {sx, sy};
        if (pSelectedNode) {
            if (pSelectedNode->fBounds.contains(sx, sy)) {
                if (bInboundSelectedOnly) {
                    return pSelectedNode;
                }
            }
        }
        fDom->findNodeByPoint(sx, sy, &pNode);
    }
    return pNode;
}

void DecSVGDomEditor::translateNode(float dx, float dy, float scale, SkSVGNode* pNode) {
    if (fDom) {
        //float dx = (x - _svgViewLastMouseX) / _svgViewScale;
        //float dy = (y - _svgViewLastMouseY) / _svgViewScale;
        dx /= scale;
        dy /= scale;

        // if svg-viewbox isvalid scale dx, dy to viewbox again
        auto& viewbox = fDom->getRoot()->getViewBox();
        if (viewbox.isValid() && !viewbox->isEmpty()) {
            SkSize szC = fDom->containerSize();
            const float ratiow = szC.fWidth  / viewbox->width();
            const float ratioh = szC.fHeight / viewbox->height();

            // aspect ratio
            const float aratio = ratiow < ratioh ? ratiow : ratioh;

            // translate little box
            //const float transx = (viewbw * ratiow - viewbw * aratio) / 2.0f;
            //const float transy = (viewbh * ratioh - viewbh * aratio) / 2.0f;

            dx = dx / aratio;
            dy = dy / aratio;
        }

        SkPoint pt = {dx, dy};
        //std::vector<SkSVGNode*> path2root;
        //SkSVGNode* pRunner = pNode->pParent;
        //while (pRunner != nullptr) {
        //    path2root.push_back(pRunner);
        //    pRunner = pRunner->pParent;
        //}
        //
        //for (auto it = path2root.rbegin(); it != path2root.rend(); it++) {
        //    SkMatrix pm = ((SkSVGTransformableNode*) *it)->fTransform;
        //    pt.fX = (pm.getTranslateX() + pt.fX);
        //    pt.fY = (pm.getTranslateY() + pt.fY);
        //
        //    if (pm.invert(&pm)) {
        //        pt = pm.mapPoint(pt);
        //    } else {
        //        SkDebugf("problem on invert matrix, node name: %s\n", (*it)->tagName());
        //        break;
        //    }
        //}

        if (pNode->pParent) {
            SkMatrix mAbs = pNode->pParent->fTransformAbs;
            pt.fX = (mAbs.getTranslateX() + pt.fX);
            pt.fY = (mAbs.getTranslateY() + pt.fY);
            if (mAbs.invert(&mAbs)) {
                pt = mAbs.mapPoint(pt);
            }
        }

        SkMatrix m = ((SkSVGTransformableNode*) pNode)->fTransform;
        float translate_dx = (m.getTranslateX() + pt.fX);
        float translate_dy = (m.getTranslateY() + pt.fY);
        //SkDebugf("(%g, %g), (%g, %g)\n", dx, dy, pt.fX, pt.fY);

        // transform="translate(700 210)"
        //char buff[100];
        //sprintf(buff, "translate(%.03f %.03f)", m.getTranslateX(), m.getTranslateY());
        //pNode->setAttribute("transform", buff);

        // transform="rotate(-10 50 100) translate(-36 45.5) skewX(40) scale(1 0.5) "
        // transform="translate(700 210) rotate(-30)"

        // kTranslate_Mask: translation SkMatrix
        // kScale_Mask:     scale SkMatrix
        // kAffine_Mask:    skew or rotate SkMatrix

        //auto t = m.getType();

        SkString tstr;
        std::string strAttrValue;

        //if (t & ~(SkMatrix::TypeMask::kTranslate_Mask)) {
            tstr.printf("translate(%g %g)", translate_dx, translate_dy);
            if (!strAttrValue.empty()) strAttrValue += " ";
            strAttrValue += tstr.c_str();
        //}
        //if (t & ~(SkMatrix::TypeMask::kAffine_Mask)) {
            std::string strTransform("transform"); 
            auto stringValue = pNode->attributeNameValueMap.find(strTransform);
            //SkDebugf("old transform: %s\n", stringValue ? stringValue->c_str() : "none");
            if (stringValue) {
                SkSVGAttributeParser parser(stringValue->c_str());
                auto* pParser = &parser;

                SkScalar angle = 0;
                SkScalar cx = 0;
                SkScalar cy = 0;

                auto fnParseRotate = [pParser, &angle, &cx, &cy]() -> bool {
                    const char prefix[] = "rotate";
                    //int i = 0;
                    if (!pParser->gotoMatchString(prefix)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }
                    
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseExpectedStringToken(prefix)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseExpectedStringToken("(")) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseScalarToken(&angle)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }

                    // optional [<cx> <cy>]
                    if (pParser->parseSepToken() &&
                        pParser->parseScalarToken(&cx)) {
                        if (!(pParser->parseSepToken() &&
                                pParser->parseScalarToken(&cy))) {
                            //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                            return false;
                        }
                    }
                    return true;
                };
                
                if (fnParseRotate()) {
                        tstr.printf("rotate(%g %g %g)", angle, cx, cy);
                    if (!strAttrValue.empty()) strAttrValue += " ";
                    strAttrValue += tstr.c_str();
                    SkDebugf("rotate(%g %g %g)\n", angle, cx, cy);
                }


                pParser->setNewString(stringValue->c_str());
                SkScalar sx = 0;
                SkScalar sy = 0;

                auto fnParseScale = [pParser, &sx, &sy]() -> bool {
                    const char prefix[] = "scale";
                    //int i = 0;
                    if (!pParser->gotoMatchString(prefix)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }

                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseExpectedStringToken(prefix)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseExpectedStringToken("(")) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        return false;
                    }
                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseScalarToken(&sx)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        //return false;
                    }

                    pParser->parseWSToken();
                    //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                    if (!pParser->parseScalarToken(&sy)) {
                        //SkDebugf("%d:%s\n", ++i, pParser->fCurPos);
                        //return false;
                    }

                    return true;
                };

                if (fnParseScale()) {
                        tstr.printf("scale(%g %g)", sx, sy);
                    if (!strAttrValue.empty()) strAttrValue += " ";
                    strAttrValue += tstr.c_str();
                    SkDebugf("scale(%g %g)\n", sx, sy);
                }
            }
        //}
        //if (t & ~(SkMatrix::TypeMask::kScale_Mask)) {
        //    //if (m.getScaleX() != 1 || m.getScaleY() != 1) {
        //        tstr.printf("scale(%g %g)", m.getScaleX(), m.getScaleY());
        //        if (!strAttrValue.empty()) strAttrValue += " ";
        //        strAttrValue += tstr.c_str();
        //    //}
        //}

        //SkDebugf("new transform: %s\n", strAttrValue.empty()? "none": strAttrValue.c_str());

        if (!strAttrValue.empty()) {
            pNode->setAttribute("transform", strAttrValue.c_str());
        }

        // update bboxs once on mouse up
        //auto surface = SkSurface::MakeRasterN32Premul(100, 100);
        //SkCanvas* canvas = surface->getCanvas();`
        //fDom->computeBoundingBoxs(canvas, _dpi);
        //SkDebugf("computeBoundingBoxs success\n");
    }
}

bool DecSVGDomEditor::onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey modifier) {
    if (((int)modifier & (int)skui::ModifierKey::kCommand) == (int)skui::ModifierKey::kCommand ||
        ((int)modifier & (int)skui::ModifierKey::kControl) == (int)skui::ModifierKey::kControl) {
#ifdef SK_BUILD_FOR_MAC
        if (delta < 0) {
#else
        if (delta > 0) {
#endif
            if (_svgViewScale < 10.0f) {
                _svgViewScale += 0.1f;
                if (_svgViewScale > 10.0f) {
                    _svgViewScale = 10.0f;
                }
                dirty(true);            
            }
            return true;
#ifdef SK_BUILD_FOR_MAC            
        } else if (delta > 0) {
#else
        } else if (delta < 0) {
#endif
            if (_svgViewScale > 0.1f) {
                _svgViewScale -= 0.1f;
                if (_svgViewScale < 0.1f) {
                    _svgViewScale = 0.1f;
                }
                dirty(true);
            }
            return true;
        }
    }
    return false;
}

bool DecSVGDomEditor::isNodeVisible( SkSVGNode *node ) {
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    if( visibility.isValue() ) {
        vType = visibility->type();
    } 
    return vType == SkSVGVisibility::Type::kVisible ? true : false;
}

void DecSVGDomEditor::setVisibleContentBounds() {
    if( !fDom ) {
        _visibleContentBounds.setEmpty();
        return;
    }
    SkRect rect;
    rect.setEmpty();

    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    SkSVGLengthContext       lctx(szC);
    SkSVGPresentationContext pctx;
    auto surface = SkSurface::MakeRasterN32Premul(100, 100);
    SkCanvas *canvas = surface->getCanvas();
    SkSVGRenderContext       rctx = SkSVGRenderContext(
                                        canvas, 
                                        fDom->fFontMgr, 
                                        fDom->fResourceProvider, 
                                        fDom->fIDMapper, 
                                        lctx, 
                                        pctx,
                                        {nullptr, nullptr});  

    root = fDom->getRoot();
    if( root->hasChildren() ) {
        int numChildren = (int)root->fChildren.size();
        for( int i=0; i<numChildren; i++ ) {
            sk_sp<SkSVGNode> node = root->fChildren.at( i );
            SkSVGNode *pNode = node.get();
            bool visible = isNodeVisible( pNode );
            if( visible ) {
                SkRect bounds = node->objectBoundingBox( rctx ); 
                rect.join( bounds ); 
            }
        }
    }
    
    if( rect.width() > szR.fWidth || 
        rect.height() > szR.fHeight ) {
        //SkDebugf( "Over size\n" );
        _visibleContentBounds = SkRect::MakeXYWH( 0, 0, szR.fWidth, szR.fHeight );
    }        
    else {
        _visibleContentBounds = rect.makeOutset( _svgDPIX/10, _svgDPIY/10 );
    }    

    float centerContentX = (drawPaperW_DPI / _svgScaleX - _visibleContentBounds.width()) / 2;
    float centerContentY = (drawPaperH_DPI / _svgScaleY - _visibleContentBounds.height()) / 2; 
    contentCenterOffsetY_72 = centerContentY - _visibleContentBounds.fTop;
    contentCenterOffsetX_72 = centerContentX - _visibleContentBounds.fLeft;   

    calculateTile(); 
}

DecSVGDomEditor &DecSVGDomEditor::drawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook ) {
    _draw_hook = hook;
    return *this;
}

DecSVGDomEditor &DecSVGDomEditor::preDrawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook ) {
    _predraw_hook = hook;
    return *this;
}


void DecSVGDomEditor::setMinPaperSize() {
    if( !fDom ) {
        minPaperSizeName = std::string( "n/a" );
        minPaperWidth_mm = 0;
        minPaperHeight_mm = 0;
        return;
    }

    root = fDom->getRoot();
    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    float w = szR.fWidth/72.0;
    float h = szR.fHeight/72.0;

    int fW = (int)(w * 25.4);
    int fH = (int)(h * 25.4);
    //if( fW > fH ) { // landscape
    //    fW = (int)(h * 25.4);
    //    fH = (int)(w * 25.4);
    //}
    for( int i=0; i<_numPaperType; i++ ) {
        if( _paperWidth[i] >= fW && _paperHeight[i] >= fH ) {
            minPaperSizeName = std::string( _paperName[i] );
            minPaperWidth_mm = _paperWidth[i];
            minPaperHeight_mm = _paperHeight[i];
            SkDebugf( "Min Paper: %s %0.2f %0.2f\n", minPaperSizeName.c_str(), 
                minPaperWidth_mm, minPaperHeight_mm );
            return;
        }
    }
}

void DecSVGDomEditor::setTile( const char *paper, float top, float right, float bottom, float left ) {
    for( int i=0; i<_numPaperType; i++ ) {
        if( !strcmp( _paperName[i], paper ) ) {
            float w = _paperWidth[i] - (right + left); // page size and border in mm
            float h = _paperHeight[i] - (top + bottom);
            _tileW = (w / 25.4 * 72.0);
            _tileH = (h / 25.4 * 72.0);

            _tileTop = top / 25.4 * 72.0;
            _tileRight = right / 25.4 * 72.0;
            _tileBottom = bottom / 25.4 * 72.0;
            _tileLeft = left / 25.4 * 72.0;

            calculateTile();
            break;
        }
    }
}

void DecSVGDomEditor::calculateTile() {
    float nX = _visibleContentBounds.width() / _tileW;
    float nY = _visibleContentBounds.height() / _tileH;

    _numTileX = (int)nX;
    _numTileY = (int)nY;

    if( _numTileX < nX )
        _numTileX++;
    if( _numTileY < nY )
        _numTileY++;

    float _totalTileW = _tileW * _numTileX;
    float _totalTileH = _tileH * _numTileY;

    _tileX = _visibleContentBounds.x() - (_totalTileW - _visibleContentBounds.width()) / 2.0;
    _tileY = _visibleContentBounds.y() - (_totalTileH - _visibleContentBounds.height()) / 2.0;
}

void DecSVGDomEditor::setPaperSize( const char *paper ) {
    for( int i=0; i<_numPaperType; i++ ) {
        if( !strcmp( _paperName[i], paper ) ) {
            paperSizeName = std::string( _paperName[i] );
            paperWidth_mm = _paperWidth[i];
            paperHeight_mm = _paperHeight[i];

            setDrawParameters();
            setMinPaperSize();
            setVisibleContentBounds();                     

            return;
        }
    }
    if( !fDom ) {
        paperWidth_mm = 0;
        paperHeight_mm = 0;
        return;
    }

    root = fDom->getRoot();
    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }
    paperSizeName = std::string(paper);
    paperWidth_mm = szR.fWidth / 72 * 25.4;
    paperHeight_mm = szR.fHeight / 72 * 25.4;

    setDrawParameters();
    setMinPaperSize();
    setVisibleContentBounds();                  
}


void DecSVGDomEditor::selectNode( SkSVGNode *pNode ){
    if( pNode == pSelectedNode ) {
        return;
    }
    dirty();
    pSelectedNode = pNode;
    //if( pSelectedNodePath ) {
    //    delete( pSelectedNodePath );
    //}
    //pSelectedNodePath = NULL;
    //if( pNode ) {
    //    SkSVGTag tag = pSelectedNode->tag();
    //    SkDebugf( "Tag: %i\n", (int) tag );
    //    if( tag == SkSVGTag::kPath || tag == SkSVGTag::kRect || 
    //        tag == SkSVGTag::kPolygon || tag == SkSVGTag::kPolyline ) {
    //        auto surface = SkSurface::MakeRasterN32Premul(100, 100);
    //        SkCanvas *canvas = surface->getCanvas();
    //        SkSize szC = fDom->containerSize(); 
    //        SkSVGLengthContext       lctx(szC);
    //        SkSVGPresentationContext pctx;
    //        SkSVGRenderContext       rctx = SkSVGRenderContext(
    //                                            canvas, 
    //                                            fDom->fFontMgr, 
    //                                            fDom->fResourceProvider, 
    //                                            fDom->fIDMapper, 
    //                                            lctx, 
    //                                            pctx,
    //                                            {nullptr, nullptr});
    //
    //        SkPath path = pNode->asPath(rctx);
    //        pSelectedNodePath = new SkPath( path );
    //        SkDebugf( "Set Selected Path\n" );
    //    }
    //}
}

DecSVGDomEditor &SVGDomEditor(const char *uri ) {
    DecSVGDomEditor *w = new DecSVGDomEditor(uri);
    return *w;    
}

void DecSVGDomEditor::calcSVGScale() {
    // fix fContainerSize with dpi=96
    fDom->setContainerSize(fDom->getRoot()->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), _dpi)));

    DecSVGDom::calcSVGScale();
}

void DecSVGDomEditor::onLoad() {
    //SkDebugf( "DecSVGDomEditor::OnLoad\n" );

    // fixed: reset pos, scale
    _bEnableMouseHoverMode = false;
    pSelectedNode = NULL;
    pHoverNode = NULL;
    pPressedNode = NULL;
    pDragNode = NULL;

    _svgViewScale = 1.0;
    _svgViewPosnX = 0;
    _svgViewPosnY = 0;

    root = fDom->getRoot();

    setDrawParameters();
    setMinPaperSize();
    setVisibleContentBounds();
    setTile( "A4", 10, 15, 15, 10 );

    if( pSVGEditor ) {
        pSVGEditor->onLoad();
    }

    // ?? move here, call after all above success for computeBoundingBoxs working correct.
    auto surface = SkSurface::MakeRasterN32Premul(100, 100);
    SkCanvas* canvas = surface->getCanvas();
    fDom->computeBoundingBoxs(canvas, _dpi);
    SkDebugf("computeBoundingBoxs success\n");
}

void DecSVGDomEditor::renderTiledPDF(const char *patternName, std::function<void(SkCanvas &canvas, int w, int h )>hook) {
    float _tileX_DPI = _tileX * _svgScaleX;
    float _tileY_DPI = _tileY * _svgScaleY;
    float _tileW_DPI = _tileW * _svgScaleX;
    float _tileH_DPI = _tileH * _svgScaleY;

    float scale = 1.0 / _svgScaleX;

    float a4W = 210.0 / 25.4 * 72.0;
    float a4H = 297.0 / 25.4 * 72.0;

    for( int y=0; y<_numTileY; y++ ) {
        for( int x=0; x<_numTileX; x++ ) {
            SkCanvas* canvas = _pdfDoc->beginPage(a4W, a4H);

            canvas->clear( SK_ColorWHITE );

            canvas->save();
            SkRect rect = SkRect::MakeXYWH( _tileLeft, _tileTop, _tileW, _tileH );
            canvas->clipRect( rect );

            canvas->scale( scale, scale );
            canvas->translate( 
                (_tileLeft * _svgScaleX)-(_tileX_DPI + (x * _tileW_DPI)), 
                (_tileTop * _svgScaleY)-(_tileY_DPI + (y * _tileH_DPI)) );

            if( hook ) {
                hook( *canvas, a4W, a4H );
            }
            fDom->render( canvas );
            canvas->restore();

            SkPaint paint;
            SkRect rectGlueR = SkRect::MakeXYWH( _tileLeft + _tileW, _tileTop, 5.0 / 25.4 * 72, _tileH );
            paint.setColor( 0xFFE0E0E0 );
            paint.setStyle( SkPaint::Style::kFill_Style );
            if( x < _numTileX-1 ) {
                canvas->drawRect( rectGlueR, paint );
            }

            SkRect rectGlueB = SkRect::MakeXYWH( _tileLeft, _tileTop + _tileH, _tileW, 5.0 / 25.4 * 72 );
            if( y < _numTileY-1 ) {
                canvas->drawRect( rectGlueB, paint );
            }

            SkRect rectGlueC = SkRect::MakeXYWH( _tileLeft + _tileW, _tileTop + _tileH, 
                5.0 / 25.4 * 72, 5.0 / 25.4 * 72 );
            if( x < _numTileX-1 && y < _numTileY-1 ) {
                canvas->drawRect( rectGlueC, paint );
            }

            paint.setColor( SK_ColorGRAY );
            paint.setStrokeWidth( 0.5 * scale );
            paint.setStyle( SkPaint::Style::kStroke_Style );
            canvas->drawRect(  rect, paint );

            paint.setColor( SK_ColorBLACK );
            paint.setStrokeWidth( 1.5 * scale );
            paint.setStyle( SkPaint::Style::kStroke_Style );

            // Left cut line
            if( x > 0 ) {
                if( y == 0 ) {
                    canvas->drawLine( _tileLeft, 0, _tileLeft, a4H, paint );
                }
                else {
                    canvas->drawLine( _tileLeft, _tileTop, _tileLeft, a4H, paint );
                }
            }

            // Top cut line
            if( y > 0 ) {
                if( x == 0 ) {
                    canvas->drawLine( 0, _tileTop, a4W, _tileTop, paint );
                }
                else {
                    canvas->drawLine( _tileLeft, _tileTop, a4W, _tileTop, paint );
                }
            }      

            SkFont font;
            font.setTypeface(_font->typeface(FONT_REGULAR));
            font.setSubpixel(true);
            font.setSize(9);       

            paint.setAntiAlias(true);
            paint.setColor(SK_ColorBLACK);
            paint.setStyle( SkPaint::Style::kFill_Style );

            SkFontMetrics metrics;
            font.getMetrics(&metrics);

            std::string text = "ติดกาวด้านนี้";
            auto blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);

            int ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

            // Glue Text Right
            if( x < _numTileX-1 ) {
                canvas->save();
                canvas->translate( _tileLeft + _tileW - metrics.fAscent, _tileTop + (_tileH - ww) / 2.0 );
                canvas->rotate( -90 );
                canvas->drawTextBlob(blob, 0, 0, paint);               
                canvas->restore();
            }

            // Glue Glue Bottom
            if( y < _numTileY-1 ) {
                canvas->drawTextBlob(blob, 
                    _tileLeft + (_tileW - ww) / 2, 
                    _tileTop + _tileH - metrics.fAscent, paint);               
            }

            text = "ตัดตามเส้นนี้";
            blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);

            ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect );                    

            // Glue Text Right
            if( x > 0 ) {
                canvas->save();
                canvas->translate( _tileLeft - metrics.fDescent, _tileTop + (_tileH - ww) / 2.0 );
                canvas->rotate( -90 );
                canvas->drawTextBlob(blob, 0, 0, paint);               
                canvas->restore();
            }

            // Cut Glue Bottom
            if( y > 0 ) {
                canvas->drawTextBlob(blob, 
                    _tileLeft + (_tileW - ww) / 2, 
                    _tileTop - metrics.fDescent, paint);               
            }

            char sz[256];
            sprintf( sz, "%s แผ่นที่ %i / %i", patternName, y * _numTileX + x + 1, _numTileX * _numTileY );

            // Title (Product ID, Size, Sheet ID, Page#)
            blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
            canvas->drawTextBlob(blob, 
                _tileLeft + (72 / 20), 
                _tileTop - metrics.fAscent + (72 / 40), 
                paint); 

            // Assembly Grid
            if( _numTileX > 1 || _numTileY > 1 ) {
                if( x == 0 && y == 0 ) {
                    canvas->save();
                    float nn = 25.0;
                    float grid_scale = nn / _tileW_DPI;
                    canvas->translate( _tileLeft + (72 / 20), _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) );
                    canvas->scale( grid_scale, grid_scale );
                    canvas->translate( 0-_tileX_DPI, 0-_tileY_DPI );
                    fDom->render( canvas );
                    canvas->restore();

                    paint.setColor( SK_ColorDKGRAY );
                    paint.setStrokeWidth( 0.5 * scale );
                    for( int yy = 0; yy<_numTileY; yy++ ) {
                        for( int xx = 0; xx<_numTileX; xx++ ) {
                            SkRect rect = SkRect::MakeXYWH( 
                                _tileLeft + (72 / 20) + (xx * nn),
                                _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) + (yy * nn * _tileH / _tileW),
                                nn, nn * _tileH / _tileW
                            );
                            if( x == xx && y == yy ) {
                                paint.setStyle( SkPaint::Style::kStroke_Style );
                                paint.setStrokeWidth( 2 * scale );
                                canvas->drawRect( rect, paint );
                            }
                            else {
                                paint.setStrokeWidth( 0.5 * scale );
                                paint.setStyle( SkPaint::Style::kStroke_Style );
                                canvas->drawRect( rect, paint );
                            }
                        }
                    } 
                }            
                else {
                    paint.setColor( SK_ColorDKGRAY );
                    paint.setStrokeWidth( 0.5 * scale );
                    for( int yy = 0; yy<_numTileY; yy++ ) {
                        for( int xx = 0; xx<_numTileX; xx++ ) {
                            int nn = 8;
                            SkRect rect = SkRect::MakeXYWH( 
                                _tileLeft + (72 / 20) + (xx * nn),
                                _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) + (yy * nn),
                                6, 6
                            );
                            if( x == xx && y == yy ) {
                                paint.setStyle( SkPaint::Style::kFill_Style );
                                canvas->drawRect( rect, paint );
                            }
                            paint.setStyle( SkPaint::Style::kStroke_Style );
                            canvas->drawRect( rect, paint );
                        }
                    } 
                }
            }

            // Test Box
            if( x == 0 && y == 0 ) {
                int tbx = _tileLeft + _tileW - 72 - (72 / 20);
                int tby = _tileTop + (72 / 20);
                int cm = 72.0 / 2.54;

                paint.setStyle( SkPaint::Style::kStroke_Style );

                rect = SkRect::MakeXYWH( tbx, tby, 72, 72 );
                canvas->drawRect(rect, paint);
                rect = SkRect::MakeXYWH( tbx + (72.0 - cm)/2, tby + (72.0 - cm)/2, cm, cm );
                canvas->drawRect(rect, paint);

                font.setSize( 7 );
                font.getMetrics(&metrics);

                paint.setStyle( SkPaint::Style::kFill_Style );

                text = "ตรวจสอบขนาด";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby+72-metrics.fDescent, paint);  

                text = "1 นิ้ว";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby-metrics.fAscent, paint);  

                text = "1 ซ.ม.";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby+36-(metrics.fDescent+metrics.fAscent)/2, paint); 
            }

 

            // Watermark
            rect = SkRect::MakeXYWH( 0, 0, pImageLogo->image->width(), pImageLogo->image->height() );
            canvas->save();
            float sc = 6.0 * 72.0 / (float)pImageLogo->image->width();
            canvas->translate( _tileLeft + _tileW / 2, _tileTop + _tileH / 2 );
            canvas->scale( sc, sc );
            canvas->rotate( -45 );
            canvas->translate( 0-pImageLogo->image->width()/2, 0-pImageLogo->image->height()/2);
            
            paint.setColorFilter(SkColorFilters::Blend(0x30FFFFFF, SkBlendMode::kModulate));
            canvas->drawImageRect( pImageLogo->image, rect, SkSamplingOptions(), &paint );
            canvas->restore();

  
            _pdfDoc->endPage();
        }
    }
}

void DecSVGDomEditor::renderPDF( const char *patternName, bool tiled, std::function<void(SkCanvas &canvas, int w, int h)>hook ) {
    if (_pdfDoc.get() == nullptr) 
        return;

    if (tiled) {
        return renderTiledPDF( patternName, hook );
    }

    float dpi = 72;

    // SVG size at SVG DPI 
    SkSize szC = fDom->containerSize();   

    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    // PDF size at 72 DPI
    float exportW = szR.fWidth;
    float exportH = szR.fHeight;

    float scale = std::min(exportW/szC.fWidth, exportH/szC.fHeight );

    float dw = 0;
    float dh = 0;    

    SkCanvas* canvas = _pdfDoc->beginPage(exportW, exportH);

    if (canvas == nullptr) 
        return;

    canvas->clear( SK_ColorWHITE );
    canvas->scale( scale, scale );

    char sz[256];
    sprintf( sz, "%s", patternName );

    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize(12);       

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorBLACK);
    paint.setStyle( SkPaint::Style::kFill_Style );

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    // Title (Product ID, Size, Sheet ID, Page#)
    auto blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
    canvas->drawTextBlob(blob, 
        72/2, 
        72/2 + 10, 
        paint); 

    canvas->translate( dw, dh );
    fDom->render(canvas);
    if (hook) {
        hook( *canvas, exportW, exportH );
    }

    _pdfDoc->endPage();
}

void DecSVGDomEditor::startExportPDF( ) {
    _pdfStream = new SkDynamicMemoryWStream();
    if (_pdfStream == nullptr) 
        return;
    _pdfDoc = SkPDF::MakeDocument(_pdfStream);
}

sk_sp<SkData> DecSVGDomEditor::endExportPDF() {
    if (_pdfDoc.get() == nullptr) 
        return nullptr;

    _pdfDoc->close();
    sk_sp<SkData> data = _pdfStream->detachAsData();

    return data;
}

/*
    if (data.get() == nullptr) 
        return;

#ifdef __EMSCRIPTEN__
    std::filesystem::path _path = std::filesystem::path(_pdfFilename.c_str());
    std::string strFilename = _path.filename();
    if (strFilename.empty()) return;

    EM_ASM({
        var pbytes = $0;
        var byteslen = $1;
        var dataUint8Array = CanvasKit['decUtility']['getBytes'](pbytes, byteslen);
        var blob = new Blob([dataUint8Array]);
        var a = document.createElement("a");
        document.body.appendChild(a);
        a.href = window.URL.createObjectURL(blob);
        a.download = UTF8ToString($2);
        a.onclick = function() {
            setTimeout(
                    function() {
                        window.URL.revokeObjectURL(a.href);
                        document.body.removeChild(a);
                    },
                    1500);
        };
        a.click();
    }, data->bytes(), data->size(), strFilename.c_str());
#else
    std::filesystem::path _path = std::filesystem::path(_pdfFilename.c_str());
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(_pdfFilename.c_str());
    file.write(data->bytes(), data->size());
    delete _pdfStream;
    _pdfStream = nullptr;
#endif
}


sk_sp<SkData> DecSVGDomEditor::getSVGData() {
    if (!fDom) {
        return nullptr;
    }
    SkDebugf( "GetSVGData\n" );
    const sk_sp<SkSVGSVG> svg = fDom->fRoot;

    svg->setAttribute("xmlns",          "http://www.w3.org/2000/svg");
    svg->setAttribute("xmlns:xlink",    "http://www.w3.org/1999/xlink");
    
    // default -1: single line, no indent
    std::string str = svg->toString(0);

    //SkDebugf("%s\n", str.c_str());
    
    sk_sp<SkData> data = SkData::MakeWithCopy(str.c_str(), str.length());
    return data;
}


void DecSVGDomEditor::saveSVG(const char* filename) {
    sk_sp<SkData> data = getSVGData();

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);
    file.write(data->bytes(), data->size());
}
*/
/*
sk_sp<SkData> DecSVGDomEditor::getSVGRenderData() {
    if (!fDom) {
        return nullptr;
    }
    SkDebugf( "GetSVGRenderData\n" );

    // SkSize containerSize = fDom->containerSize();
    //SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 95.997395));
    SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 96));
    SkDebugf("containerSize:[%.03f,%.03f]\n",
             containerSize.width(),
             containerSize.height());
    SkRect bounds = SkRect::MakeSize(containerSize);
    SkDynamicMemoryWStream writer;
    { // block for SkXMLStreamWriter(inside SkSVGCanvas obj) flush() on destrutor
        auto pSelectedNode_Temp = pSelectedNode;
        pSelectedNode = NULL;

        std::unique_ptr<SkCanvas> canvas = SkSVGCanvas::Make(bounds, &writer);
        SkCanvas* pcanvas = canvas.get();

        // this->_draw_content(*pcanvas);
        fDom->render(pcanvas);

        pSelectedNode = pSelectedNode_Temp;
    }
    
    sk_sp<SkData> data = writer.detachAsData();
    return data;
}


void DecSVGDomEditor::exportSVG(const char* filename) {
    sk_sp<SkData> data = getSVGRenderData();

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);
    file.write(data->bytes(), data->size());
    delete _pdfStream;
    _pdfStream = nullptr;   
}
*/
/*
sk_sp<SkData> DecSVGDomEditor::getPNGData( float dpi ) {
    SkPixmap pixmap = getPixmap( dpi );

    SkDynamicMemoryWStream buf;
    bool success = SkPngEncoder::Encode(&buf, pixmap, SkPngEncoder::Options());
    return success ? buf.detachAsData()  : nullptr;    
}
*/

pesBuffer DecSVGDomEditor::getPNGBuffer(float dpi) {
    pesBuffer buff;

    sk_sp<SkData> data = getPNGData( dpi );

    if(data && data->size()){
        buff.set((const char*)data->data(), data->size());
    }
    return buff;
}

pesBuffer DecSVGDomEditor::getPDFBuffer() {
    pesBuffer buff;

    sk_sp<SkData> data = getPDFData();

    if(data && data->size()){
        buff.set((const char*)data->data(), data->size());
    }
    return buff;
}

pesBuffer DecSVGDomEditor::getSVGBuffer() {
    pesBuffer buff;

    sk_sp<SkData> data = getSVGData();

    if(data && data->size()){
        buff.set((const char*)data->data(), data->size());
    }
    return buff;
}

pesBuffer DecSVGDomEditor::getSVGRenderBuffer() {
    pesBuffer buff;

    sk_sp<SkData> data = getSVGRenderData();

    if(data && data->size()){
        buff.set((const char*)data->data(), data->size());
    }
    return buff;
}

/*
SkPixmap DecSVGDomEditor::getPixmap( float dpi, std::function<void(SkCanvas &canvas)>hook ) {
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));

    //float exportW = szR.fWidth * dpi / 72.0;
    //float exportH = szR.fHeight * dpi / 72.0;

    float exportW = _visibleContentBounds.width() * dpi / 72.0;
    float exportH = _visibleContentBounds.height() * dpi / 72.0;

    SkSize sz = fDom->containerSize();

    auto surface = SkSurface::MakeRasterN32Premul(exportW, exportH);

    //float scale = std::min(exportW/sz.fWidth, exportH/sz.fHeight );
    float scale = std::min( exportW/(_visibleContentBounds.width() * _svgScaleX), exportH/(_visibleContentBounds.height() * _svgScaleY) );

    //float dw = ((exportW - sz.fWidth * scale)/2) / scale;
    //float dh = ((exportH - sz.fHeight * scale)/2) / scale;
    float dw = 0;
    float dh = 0;

    SkCanvas *canvas = surface->getCanvas();
    canvas->scale( scale, scale );
    canvas->translate( 0-(_visibleContentBounds.left() * _svgScaleX), 0-(_visibleContentBounds.top() * _svgScaleY) );
    fDom->render( canvas );
    if( hook ) {
        hook( *canvas );
    }    

    SkPixmap pixmap;
    surface->peekPixels(&pixmap);
    return pixmap;
}


void DecSVGDomEditor::exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook  ) {
    SkPixmap pixmap = getPixmap( dpi, hook );

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream out(filename);
    if (!out.isValid()) {
        SkDebugf( "Could not open export file: error on create file.\n" );
        return;
    }

    // Use default encoding options.
    SkPngEncoder::Options png_options;

    if (!SkPngEncoder::Encode(&out, pixmap, png_options)) {
        SkDebugf( "PNG encoding failed.\n" );
        return;
    }    
}
*/
sk_sp<SkData> DecSVGDomEditor::getPDFData() {
    startExportPDF();
    renderPDF( "", false );
    sk_sp<SkData>data = endExportPDF();  
    return data; 
}

void DecSVGDomEditor::exportPDF( const char *filename ) {
    sk_sp<SkData> data = getPDFData();

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);
    file.write(data->bytes(), data->size());
    delete _pdfStream;
    _pdfStream = nullptr;    
}

void DecSVGDomEditor::setDrawParametersFit() {
    SkSize szC = fDom->containerSize();   

    drawPaperW_DPI = szC.fWidth;
    drawPaperH_DPI = szC.fHeight;      

    drawScale_DPI = std::min( width()/drawPaperW_DPI, height()/drawPaperH_DPI ) * 0.9;
    drawCenterOffsetX_DPI = ((width() - drawPaperW_DPI * drawScale_DPI)/2) / drawScale_DPI;
    drawCenterOffsetY_DPI = ((height() - drawPaperH_DPI * drawScale_DPI)/2) / drawScale_DPI;
}

void DecSVGDomEditor::setDrawParametersPaper() {
    SkSize szC = fDom->containerSize();   

    drawPaperW_DPI = ((paperWidth_mm / 25.4)) * _svgDPIX;
    drawPaperH_DPI = ((paperHeight_mm / 25.4)) * _svgDPIY;    

    drawScale_DPI = std::min( (width()-20)/drawPaperW_DPI, (height()-20)/drawPaperH_DPI );

    drawCenterOffsetX_DPI = (((width() - drawPaperW_DPI * drawScale_DPI)/2) / drawScale_DPI);
    drawCenterOffsetY_DPI = (((height() - drawPaperH_DPI * drawScale_DPI)/2) / drawScale_DPI);
}

void DecSVGDomEditor::setDrawParameters() {
    if( !fDom ) 
        return;
    if( drawFit ) {
        setDrawParametersFit( );
    }
    else {
        setDrawParametersPaper( );
    }
}

//void DecSVGDomEditor::_draw_content(SkCanvas &canvas) {
//    if (!_isVisible) {
//        return;
//    }
//    if (_testDraw) {
//        DecWidget::_draw_content(canvas);
//        return;
//    }
//    if( !fDom ) {
//        return;
//    }
//
//    /*
//    // about matrix.mapRect
//    {
//        canvas.save();
//        SkCanvas* cv = &canvas;
//        SkPaint paint;
//        paint.setAntiAlias(true);
//        SkMatrix matrix;
//        SkRect bounds = {10, 10, 150 + 10, 150 + 10};
//        SkRect translateBounds, rotatedBounds;
//        paint.setColor(0x80404040);
//        cv->drawRect(bounds, paint);
//        matrix.setTranslate(150 / 2, 150 / 2);
//        matrix.mapRect(&translateBounds, bounds);
//        paint.setColor(0x8000FF00);
//        cv->drawRect(translateBounds, paint);
//        matrix.setRotate(10, 150 + 10, 150 + 10);
//        matrix.mapRect(&rotatedBounds, translateBounds);
//        paint.setColor(0x400000FF);
//        cv->drawRect(rotatedBounds, paint);
//        cv->concat(matrix);
//        paint.setColor(0x80FF0000);
//        cv->drawRect(translateBounds, paint);
//        canvas.restore();
//    }
//    */
//
//    setDrawParameters();
//
//    SkSize szC = fDom->containerSize();  
//
//    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
//    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
//        szR.fWidth = szC.fWidth;
//        szR.fHeight = szC.fHeight;
//    }
//
//    //SkDebugf("wh:%.03f, %.03f\n", szC.fWidth, szC.fHeight);
//    //SkDebugf("wh:%.03f, %.03f\n", szR.fWidth, szR.fHeight);
//         
//    canvas.save();
//    canvas.translate( _svgViewPosnX, _svgViewPosnY );
//    canvas.scale( _svgViewScale, _svgViewScale );
//    canvas.scale( drawScale_DPI, drawScale_DPI );
//    canvas.translate( drawCenterOffsetX_DPI, drawCenterOffsetY_DPI );    
//
//    if( _showTileGrid ) {
//        canvas.save();
//        canvas.scale( _svgScaleX, _svgScaleY );
//        canvas.translate( 0+contentCenterOffsetX_72, 0+contentCenterOffsetY_72 );
//        
//        SkPaint paint;
//        paint.setColor( 0xFFFFFFFF );
//        paint.setAntiAlias( true );
//        paint.setStrokeWidth( 1 / drawScale_DPI );
//        paint.setStyle( SkPaint::Style::kFill_Style );   
//
//        SkRect rect = SkRect::MakeXYWH( 
//            _tileX, 
//            _tileY, 
//            _tileW * _numTileX, 
//            _tileH * _numTileY );
//        canvas.drawRect( rect, paint );  
//
//        canvas.restore();
//    }
//    else {
//        // Draw Paper
//        SkRect rect = SkRect::MakeXYWH( 0, 0, drawPaperW_DPI, drawPaperH_DPI );
//        SkPaint paintBG;
//        paintBG.setColor( 0xFFFFFFFF );
//        paintBG.setStyle( SkPaint::Style::kFill_Style );    
//        canvas.drawRect( rect, paintBG );    
//        paintBG.setColor( 0xFF000000 );
//        paintBG.setStyle( SkPaint::Style::kStroke_Style );            
//        canvas.drawRect( rect, paintBG );   
//        canvas.clipRect( rect, false );
//    }
//
//    // Draw SVG at center of paper
//    canvas.save();
//    if( _bMoveContentToCenter ) {
//        canvas.translate( 0+contentCenterOffsetX_72*_svgScaleX, 0+contentCenterOffsetY_72*_svgScaleY );
//    }
//    fDom->render(&canvas);
//    canvas.restore();
//
//    SkSVGLengthContext       lctx(szC);
//    SkSVGPresentationContext pctx;
//    SkSVGRenderContext       rctx = SkSVGRenderContext(
//                                        &canvas, 
//                                        fDom->fFontMgr, 
//                                        fDom->fResourceProvider, 
//                                        fDom->fIDMapper, 
//                                        lctx, 
//                                        pctx,
//                                        {nullptr, nullptr});
//
//    if( _predraw_hook ) {
//        _predraw_hook( this, canvas );
//    }
//
//    // All overlay draw at 72 DPI (Document Space)
//    canvas.save();
//    canvas.scale( _svgScaleX, _svgScaleY );
//
//    // Minimal paper bounds
//    if( _showMinPaperBounds ) {
//        float minPW = (minPaperWidth_mm / 25.4) * 72.0;
//        float minPH = (minPaperHeight_mm / 25.4) * 72.0;
//        float minPX = (drawPaperW_DPI / _svgScaleX - minPW) / 2;
//        float minPY = (drawPaperH_DPI / _svgScaleY- minPH) / 2;
//        SkRect rect = SkRect::MakeXYWH( minPX, minPY, minPW, minPH );
//        SkPaint paintBG;
//
//        paintBG.setColor( 0xFF00FF00 );
//        paintBG.setStrokeWidth( 1.0 / drawScale_DPI );
//        paintBG.setStyle( SkPaint::Style::kStroke_Style );    
//        canvas.drawRect( rect, paintBG );     
//    }
//
//    // All object bounds ar draw at 72 DPI (center translate)
//    // Selection to center space    
//    canvas.translate( 0+contentCenterOffsetX_72, 0+contentCenterOffsetY_72 );
//    if( pSelectedNode ) {
//        SkRect rect = pSelectedNode->objectBoundingBox(rctx);
//
//        SkDebugf(">>>> xywh:%.03f, %.03f, %.03f, %.03f: <%s>\n",
//                 rect.fLeft,
//                 rect.fTop,
//                 rect.width(),
//                 rect.height(),
//                 pSelectedNode->tagName());
//
//        // line slope 0, 1 can draw rectangle bounding box.
//        // if (!rect.isEmpty())
//        if (rect.width() || rect.height()) {
//            const SkSVGNode* pRoot = fDom->fRoot.get();
//            SkSVGNode* pRunner = pSelectedNode;
//            while (pRoot != pRunner) {
//                pRunner = pRunner->pParent;
//                if (!pRunner) {
//                    break;
//                }
//
//                pRunner->mapToParent(&rect);
//
//                SkDebugf(" map xywh:%.03f, %.03f, %.03f, %.03f: <%s>\n",
//                         rect.fLeft,
//                         rect.fTop,
//                         rect.width(),
//                         rect.height(),
//                         pRunner->tagName());
//            }
//
//            SkPaint paint;
//            paint.setAntiAlias(true);
//            paint.setStyle(SkPaint::Style::kStroke_Style);
//
//            //paint.setColor(0x80FF0000);
//            //paint.setStrokeWidth(8.0 / drawScale_DPI);
//            //canvas.drawRect(rect, paint);
//
//            // TODO: svg units width = cm, height = cm, ui must represent in cm, not 100% always
//            // fix: if 100% always, map rect scale to svg.viewbox then svg.container-size and ui.size
//            // solve: rect02.svg, circle01.svg, line01.svg, polygon01.svg, polyline01.svg, ellipse01.svg ...
//            auto& viewbox = fDom->fRoot->getViewBox();
//
//            // fix: svg no attr viewBox
//            // solve: AA-tommek_Car.svg, AAA-svg2009.svg
//            if (viewbox.isValid() && !viewbox->isEmpty()) {
//                const float viewbw = viewbox->width();
//                const float viewbh = viewbox->height();
//                const float ratiow = szC.fWidth  / viewbw;
//                const float ratioh = szC.fHeight / viewbh;
//
//                // old: without aspect ratio
//                //rect.setLTRB(rect.fLeft   * ratiow / _svgScaleX,
//                //             rect.fTop    * ratioh / _svgScaleY,
//                //             rect.fRight  * ratiow / _svgScaleX,
//                //             rect.fBottom * ratioh / _svgScaleY);
//
//                // fix: aspect ratio
//                const float aratio = ratiow < ratioh ? ratiow : ratioh;
//                const float transx = (viewbw * ratiow - viewbw * aratio) / 2.0f;
//                const float transy = (viewbh * ratioh - viewbh * aratio) / 2.0f;
//                
//                rect.setXYWH(rect.fLeft    * aratio + transx,
//                             rect.fTop     * aratio + transy,
//                             rect.width()  * aratio,
//                             rect.height() * aratio);
//                
//                rect.setXYWH(rect.fLeft    / _svgScaleX,
//                             rect.fTop     / _svgScaleY,
//                             rect.width()  / _svgScaleX,
//                             rect.height() / _svgScaleY);
//            }
//
//            SkDebugf("UI xywh:%.03f, %.03f, %.03f, %.03f: <%s>\n\n",
//                     rect.fLeft,
//                     rect.fTop,
//                     rect.width(),
//                     rect.height(),
//                     pSelectedNode->tagName());
//
//            paint.setColor(0x8000A0FF);
//            paint.setStrokeWidth(8.0f / drawScale_DPI);
//            canvas.drawRect(rect, paint);
//        }
//    }       
//
//    if( _showVisibleContentBounds ) {
//        SkPaint paint;
//        paint.setColor( 0x40FF0000 );
//        paint.setAntiAlias( true );
//        paint.setStrokeWidth( 1 / drawScale_DPI );
//        paint.setStyle( SkPaint::Style::kStroke_Style );    
//
//        SkRect rect = SkRect::MakeXYWH( 
//            _visibleContentBounds.x(), 
//            _visibleContentBounds.y(), 
//            _visibleContentBounds.width(), _visibleContentBounds.height() );
//        canvas.drawRect( rect, paint );
//    }
//
//    if( _showTileGrid ) {
//        SkPaint paint;
//        paint.setColor( 0x400000FF );
//        paint.setAntiAlias( true );
//        paint.setStrokeWidth( 1 / drawScale_DPI );
//        paint.setStyle( SkPaint::Style::kStroke_Style );   
//
//        SkRect rect = SkRect::MakeXYWH( 
//            _tileX, 
//            _tileY, 
//            _tileW * _numTileX, 
//            _tileH * _numTileY );
//        canvas.drawRect( rect, paint );
//        for( int i=0; i<_numTileX; i++ ) {
//            float xx = _tileX + (i * _tileW);
//            canvas.drawLine( xx, _tileY, xx, _tileY + _tileH * _numTileY, paint );
//        }
//        for( int i=0; i<_numTileY; i++ ) {
//            float yy = _tileY + (i * _tileH);
//            canvas.drawLine( _tileX, yy, _tileX + _tileW * _numTileX, yy, paint );
//        }
//    }
//
//    canvas.restore();
//
//    if( _draw_hook ) {
//        _draw_hook( this, canvas );
//    }
//
//
//    canvas.restore();
//
//    /*
//    
//    char str[64];
//    sprintf( str, "win size: %i %i", width(), height() );
//    SkPaint paint;
//    paint.setStyle( SkPaint::Style::kFill_Style );
//    paint.setColor( 0xFF000000 );
//    SkFont font;
//
//    sk_sp<SkTypeface> typeface = _font->typeface(0);
//    
//    font.setTypeface(typeface);
//    font.setSubpixel(true);
//    font.setSize(16);
//
//    canvas.drawSimpleText( str, strlen(str), SkTextEncoding::kUTF8, 200, 200, font, paint );
//    //canvas.drawRect( SkRect::MakeXYWH( 100, 100, 200, 200), paint );    
//
//    paint.setColor( 0xFFFF0000 );
//    paint.setStyle( SkPaint::Style::kStroke_Style );
//    paint.setAntiAlias( true );
//
//    canvas.drawRect( SkRect::MakeXYWH( 10, 10, width()-20,height()-20 ), paint) ;
//
//    canvas.drawLine( SkPoint::Make(0, 0), SkPoint::Make(width(), height()), paint );
//    canvas.drawLine( SkPoint::Make(width(), 0), SkPoint::Make(0, height()), paint );        
//    */
//}

void DecSVGDomEditor::_draw_content(SkCanvas& canvas) {
    if (!_isVisible) {
        return;
    }
    if (_testDraw) {
        DecWidget::_draw_content(canvas);
        return;
    }
    if (!fDom) {
        return;
    }
    
    SkSize szC = fDom->containerSize();

    canvas.save();
    canvas.translate(_svgViewPosnX + (width() - szC.width() * _svgViewScale) / 2.0f,
                     _svgViewPosnY + (height() - szC.height() * _svgViewScale) / 2.0f);
    canvas.scale(_svgViewScale, _svgViewScale);

    // Draw white paper
    SkRect rect = SkRect::MakeXYWH(0, 0, szC.width(), szC.height());
    SkPaint paintBG;
    paintBG.setColor(0xFFFFFFFF);
    paintBG.setStyle(SkPaint::Style::kFill_Style);
    canvas.drawRect(rect, paintBG);

    // Draw SVG
    canvas.save();
    fDom->render(&canvas, _dpi);
    canvas.restore();

    // Draw BBox
    auto fnDrawBBox = [&](SkSVGNode* pNode, bool bFill, bool bStroke) {
        if (pNode && (bFill || bStroke)) {
            //SkRect rect;
            //
            //SkSVGTransformableNode *pTNode = (SkSVGTransformableNode *)pNode;
            //SkMatrix m = pTNode->fTransform;
            //m.mapRect( &rect, pNode->fBounds, SkApplyPerspectiveClip::kYes );

            SkRect rect = pNode->fBounds;
            if (rect.width() || rect.height()) {
                SkPaint paint;
                paint.setAntiAlias(false);
                if (bFill) {
                    paint.setColor(0x8000A0FF);
                    paint.setStyle(SkPaint::Style::kFill_Style);
                    canvas.drawRect(rect, paint);
                }

                if (bStroke) {
                    paint.setColor(0xC000A0FF);
                    rect.outset(0.5f, 0.5f);
                    paint.setStyle(SkPaint::Style::kStroke_Style);
                    paint.setStrokeWidth(1.0f);
                    canvas.drawRect(rect, paint);
                }
            }
        }
    };

    fnDrawBBox(pSelectedNode, true, true);
    
    fnDrawBBox(pHoverNode, false, true);

    fnDrawBBox(pPressedNode, true, false);

    canvas.restore();
}