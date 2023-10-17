#include "DeclarativeUI.h"
#include "DecFileDialog.h"
#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

DecFileDialogBox::DecFileDialogBox() {
    _callback = nullptr;

    size( 480, 320 );
    border( false );
    round( 5 );
    _( Column()
        .padding( 10, 10, 10, 10 )
        .border(false)
        .gap(5)
        ._( Text( "Path: ").ref( (DecWidget **)&_pTextPath ) )
        ._( Row()
            .size( -1, -1 )
            .border(false)
            .gap( 10 )
            ._(List()
                .ref( (DecWidget **)&_pListDir )
                .size( 160, -1 )
                .round( 5 )
                .padding( 0, 0, 0, 0 )
                .border( true )
                .border_color( 0xFF808080 )
                .tag( 991 )
            )
            ._(List()
                .ref( (DecWidget **)&_pListFile )
                .size( -1, -1 )
                .round( 5 )
                .padding( 0, 0, 0, 0 )
                .border( true )
                .border_color( 0xFF808080 )
                .tag( 992 )
            )
        )
        ._( Row() 
            .size( -1, 0 )
            .border( false )
            .background( false )
            .margin( 10, 0, 10, 0  )
            .gap( 10 )

            ._(Spacer().size( -1, 0 ))
            ._(Button("OK", 0, [=](DecButton &btn) {
                onOK();
            }).size( 100, 0 ))
            ._(Button("Cancel", 0, [=](DecButton &btn) {
                hide();
            }).size( 100, 0 ))            
            ._(Spacer().size( -1, 0 ))
        )
    );

    _cwd = std::filesystem::current_path();
    updateLists();
}

void DecFileDialogBox::updateLists() {
    _pTextPath->setText( _cwd.string().c_str() );
    //SkDebugf( "Path: %s\n", _cwd.string().c_str() );

    std::string path = _cwd.string();

    _dirlist.clear();
    _filelist.clear();

    _pListDir->clear();
    _pListFile->clear();

    _dirlist.push_back( ".." );
    //_pListFile->_( ListItem( ".." ) );

    for (const auto & entry : fs::directory_iterator(path)) {
        fs::path _path = entry.path();
        fs::file_status _status = entry.symlink_status();

        std::string _filename = _path.filename().string();
        if( _filename[0] != '.' ) {
            if( fs::is_directory( _status ) ) {
                _dirlist.push_back( _filename );
            }
            else if( fs::is_regular_file( _status ) ) {
                _filelist.push_back( _filename );
            }
        }
    }
    sort(_dirlist.begin(), _dirlist.end());
    sort(_filelist.begin(), _filelist.end());
    for( int i=0; i<_dirlist.size(); i++ ) {
        std::string f = _dirlist[i];
        //SkDebugf( "Dir: %s\n", f.c_str() );
        _pListDir->_( ListItem( f.c_str() ) );
    }
    for( int i=0; i<_filelist.size(); i++ ) {
        std::string f = _filelist[i];
        //SkDebugf( "File %s\n", f.c_str() );
        _pListFile->_( ListItem( f.c_str() ) );
    }    
}

DecFileDialogBox::~DecFileDialogBox() {

}

void DecFileDialogBox::onOK() {
    fs::file_status _status = fs::status(fullpath);
    if( fs::is_directory( _status ) ) {
        SkDebugf( "Directory\n" );
    }
    else if( fs::is_regular_file( _status ) ) {
        // fixed: call fullpath.c_str() will be returns a string type as same as using in path,  
        // in this case, windows always return const wchar_t*
        // windows std::filesystem::path:
        // -using value_type  = wchar_t;
        // -using string_type = _STD wstring;
        // Workarounds:
        // -every platform should be convert path to std::string by path::string()

        SkDebugf( "File: %s\n", fullpath.string().c_str() );
        if( _callback ) {
            _callback(*this, fullpath.string().c_str());
        }
        hide();
    }    
}

void DecFileDialogBox::getFile( std::function<void( DecFileDialogBox &dlg, const char *filepath )> callback ) {
    _callback = callback;

    show();
}


DecFileDialogBox &FileDialogBox() {
    DecFileDialogBox *w = new DecFileDialogBox();
    return *w;   
}

void DecFileDialogBox::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_LIST_SELECT_CHANGED ) {
        DecList *item = (DecList *)sender;
        if( sender->tag() == 991 ) {
            int dirIndex = (int)item->value();
            std::string dirname = _dirlist[dirIndex];
            if( dirname == ".." ) {
                _cwd = _cwd.parent_path();
            }
            else {
                _cwd.append(dirname);
            }

            fullpath = _cwd;
            updateLists();
        }
        else if( sender->tag() == 992 ) {
            int fileIndex = (int)item->value();
            if (fileIndex < 0) {
                return;
            }
            std::string filename = _filelist[fileIndex];

            fullpath = _cwd;
            fullpath.append( filename );
            SkDebugf( "%s\n", fullpath.string().c_str() );
            _pTextPath->setText( fullpath.string().c_str() );
        }
    }
}