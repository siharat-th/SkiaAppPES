#ifndef __DEC_FILELIST_CONTAINER_H__
#define __DEC_FILELIST_CONTAINER_H__

#include "DecList.h"
#include "DecContainer.h"

class DecFileList : public DecContainer {
public:    
    DecFileList(const char *dir=NULL);
    virtual ~DecFileList() override;

    virtual std::string type() override { return "FileList"; }

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    int count() { return _pListFile->count(); }
    void select_fileItem( int i ) { _pListFile->select_listItem( i ); }

    void refresh();    
    std::filesystem::path fullpath;

    std::filesystem::path _cwd;
    std::vector<std::string> _filelist;    
protected:
    DecList *_pListFile;

};

DecFileList &FileList(const char *dir=NULL);

#endif //__DEC_FILELIST_CONTAINER_H__