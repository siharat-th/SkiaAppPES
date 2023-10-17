#include "DeclarativeUI.h"
#include "DecUComposer.h"
#include "DecUTestPanel.h"

DecUTestPanel::DecUTestPanel() : DecColumnContainer() {
    size( 360, -1 );
    border(false);
    background(false);
    gap( 0 );
    padding( 10, 10, 10, 10 );
    // Test
    _( Text( "Test" ) );
    _( Row()
        .size( -1, 0 ).gap( 10 ).border( false ).background( false ).margin( 0, 0, 10, 0 )
        // Add Test Widget (100 x 100)
        ._( Button("Add Widget 100x100", 0, [=]( DecButton &btn ) { AddTestWidget(); }))
        // Add Test SVG
        ._( Button("Add Test SVG", 0, [=]( DecButton &btn ) { AddTestSVG(); }))
    );
    _( Text( "SVG" ) );
    _( Row()
        .size( -1, 0 ).gap( 10 ).border( false ).background( false ).margin( 0, 0, 10, 0 )
        // New Document
        ._( Button("New", 0, [=]( DecButton &btn ) { NewDocument(); }))
        // Load SVG from File (Native) or URL (Web)
        ._( Button("Load SVG", 0, [=]( DecButton &btn ) { LoadSVG(); }))
        // Open SVG from File (Native & Web)
        ._( Button("Open SVG", 0, [=]( DecButton &btn ) { OpenSVGFile(); }))
        // Save SVG to File (Native & Web)
        ._( Button("Save SVG", 0, [=]( DecButton &btn ) { SaveSVGFile(); }))
    );
    _( Text( "Export" ) );
    _( Row()
        .size( -1, 0 ).gap( 10 ).border( false ).background( false ).margin( 0, 0, 10, 0 )
        // Export PNG
        ._( Button("PNG", 0, [=]( DecButton &btn ) { ExportPNGFile(); }))
        // Export SVG
        ._( Button("SVG", 0, [=]( DecButton &btn ) { ExportSVGFile(); }))
    );
}

DecUTestPanel::~DecUTestPanel() {

}
 
DecUTestPanel &UTestPanel() {
    DecUTestPanel *w = new DecUTestPanel();
    return *w;    
}

void DecUTestPanel::NewDocument() {
    DecUComposer::getInstance()->NewDocument();
}

void DecUTestPanel::AddTestWidget() {
    DecUComposer::getInstance()->_( Widget().size( 100, 100 ) );
}

void DecUTestPanel::AddTestSVG() {
    DecUComposer::getInstance()->_( SVGDom( "resources/svg/MagicPattern01.svg").size( 200, 150 ).cache(true)
    );
}

void DecUTestPanel::LoadSVG() {
    NewDocument();
    SkDebugf( "LoadSVG\n" );
    DecUComposer::getInstance()->LoadSVG( "resources/Avatar/Example-01.svg" );
}

void DecUTestPanel::OpenSVGFile() {
    NewDocument();
    SkDebugf( "LoadSVG\n" );
    DecUComposer::getInstance()->OpenSVGFile();
}

void DecUTestPanel::ExportPNGFile() {
    SkDebugf( "Export PNG\n" );
    DecUComposer::getInstance()->ExportPNGFile( 0 );
}

void DecUTestPanel::SaveSVGFile() {
    SkDebugf( "Save SVG\n" );
    DecUComposer::getInstance()->SaveSVGFile( 0 );
}

void DecUTestPanel::ExportSVGFile() {
    SkDebugf( "Export SVG\n" );
    DecUComposer::getInstance()->ExportSVGFile( 0 );
}