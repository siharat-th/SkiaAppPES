#ifndef __DECUCOMPOSER_LOADER_H__
#define __DECUCOMPOSER_LOADER_H__

#include "include/docs/SkPDFDocument.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGG.h"
#include "DeclarativeUI.h"

class DecUComposer : public DecStackContainer {
public:    
    DecUComposer();
    virtual ~DecUComposer() override;

    const int EVENT_SVG_CHANGED = 1;
    const int EVENT_SELECT_CHANGED = 2;

    std::vector<DecWidget *>vElements;

    std::map<std::string,SkSVGNode*>mSVGGroupMap;
    
    int selectedElementIndex;

    static DecUComposer *getInstance();

    virtual void init() override;
    virtual void update() override;
    void setDarkMode( bool dark );

    void selectElement( int index );

    virtual void _draw_children(SkCanvas& canvas) override;

    float doc_width;
    float doc_height;

    SkDynamicMemoryWStream * _pdfStream;
    sk_sp<SkDocument> _pdfDoc;

    void startExportPDF( );
    sk_sp<SkData> endExportPDF();    

    void NewDocument();

    void createSVGTree( int index );
    void parseSVGSubTree( int level, SkSVGNode* node );

    void onContentChanged( int event_type, int index );

    // API --------------

    // SVG API
    void LoadSVG( std::string uri, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0 );
    void OpenSVGFile();

    void SaveSVGFile(int index, std::string filename="untitled.svg" );    
    void ExportSVGFile(int index, std::string filename="untitled.svg" );    

    // Export PNG
    void ExportPNGFile(int index, std::string filename="untitled.png" );    

    sk_sp<SkData> GetSVGData( int index );
    sk_sp<SkData> GetSVGRenderData( int index );
    sk_sp<SkData> GetPNGData( int index, int dpi );

    std::string GetGroupList();

    void DisplayPartType( std::string part, std::string type );
};

DecUComposer &UComposer();

#endif //__DECUCOMPOSER_LOADER_H__