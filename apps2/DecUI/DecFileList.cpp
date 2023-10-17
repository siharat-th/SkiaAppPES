#include <iostream>
#include <filesystem>
#include "DecFileList.h"

namespace fs = std::filesystem;

DecFileList::DecFileList(const char *dir) {
    _( List()
        .ref( (DecWidget **)&_pListFile ) 
        .size( -1, -1 )
        .tag( 993 )
    );

    //raise a fast-fail exception and termintae the process, if dir not exists. 
    //if( dir ) {
    //    _cwd = std::filesystem::path( dir );
    //} else {
    //    _cwd = std::filesystem::current_path();
    //}

    // default
    _cwd = std::filesystem::current_path();

    if (dir) {
        auto pathdir = std::filesystem::path(dir);
        if (std::filesystem::exists(pathdir)) {
            _cwd = pathdir;
        } else {
            // try to create
            if (std::filesystem::create_directories(pathdir)) {
                _cwd = pathdir;
            }
        }
    }

    refresh();
}

DecFileList::~DecFileList() {
}

void DecFileList::refresh() {
    std::string path = _cwd.string();

    _filelist.clear();

    _pListFile->clear();

    for (const auto & entry : fs::directory_iterator(path)) {
        fs::path _path = entry.path();
        fs::file_status _status = entry.symlink_status();

        std::string _filename = _path.filename().string();
        if( _filename[0] != '.' ) {
            if( fs::is_regular_file( _status ) ) {
                _filelist.push_back( _filename );
            }
        }
    }    
    sort(_filelist.begin(), _filelist.end());    
    for( int i=0; i<_filelist.size(); i++ ) {
        std::string f = _filelist[i];
        _pListFile->_( ListItem( f.c_str() ) );
    }        
}

DecFileList &FileList(const char *dir) {
    DecFileList *w = new DecFileList( dir );
    return *w;        
}

void DecFileList::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_LIST_SELECT_CHANGED ) {
        if (sender) {
            DecList* item = (DecList*)sender;
            if (sender->tag() == 993) {
                int fileIndex = (int)item->value();
                std::string filename = _filelist[fileIndex];

                fullpath = _cwd;
                fullpath.append(filename);
                //SkDebugf("%s\n", fullpath.string().c_str());
                if (_parent) {
                    _parent->bubbleAction(ACTION_FILELIST_SELECT_CHANGED, this);
                }
                if (_onValueChanged) {
                    _onValueChanged(*this);
                }
            }
        }
    }
}