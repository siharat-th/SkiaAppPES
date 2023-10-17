#include "DeclarativeUI.h"
#include "DecMessageBox.h"

DecMessageBox::DecMessageBox() {
    _pTextMessage = NULL;

    size( 360, 160 );
    _( Column()
        .border(false)
        ._( Stack()
            .size( -1, -1 )
            .border( false )
            .background( false )
            .margin( 20, 0, 0, 0 )
            ._( Text("Message" ) 
                .ref( (DecWidget **)&_pTextMessage )
                .align( ALIGN_CENTER )
            )
        )
        ._( Row() 
            .size( -1, 0 )
            .border( false )
            .background( false )
            .margin( 10, 0, 20, 0  )
            .gap( 10 )

            ._(Spacer().size( -1, 0 ))
            ._(Button("OK", 0, [=](DecButton &btn) {
                onOK();
            }).size( 100, 0 ))
            ._(Spacer().size( -1, 0 ))
        )
    );
}

DecMessageBox::~DecMessageBox() {

}

DecMessageBox &MessageBox() {
    DecMessageBox *w = new DecMessageBox();
    return *w;   
}

void DecMessageBox::onOK() {
    hide();
}

DecMessageBox &DecMessageBox::message( const char *msg ) {
    _pTextMessage->setText( msg );
    return *this;
}