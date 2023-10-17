#include "DeclarativeUI.h"
#include "DecTemplate.h"

DecTemplate::DecTemplate() {
    size( -1, -1 );
    border( false );
    background( false );
    _( Stack()
        ._( Widget()
            .size( -1, -1 )
        )
        ._( Text( "Hello World" )
            .ref( (DecWidget **)&pTextInfo )
            .margin( 10, 10, 10, 12 ) 
        )
    );
}

DecTemplate::~DecTemplate(){

} 

DecTemplate &Template() {
    DecTemplate *w = new DecTemplate();
    return *w;
}

void DecTemplate::init() {
    SkDebugf( "init\n" );
    char sz[256];
    sprintf( sz, "Screen: %i %i", width(), height() );
    pTextInfo->text( sz );
}

void DecTemplate::update() {
    char sz[256];
    sprintf( sz, "Screen: %i %i", width(), height() );
    pTextInfo->text( sz );
}