#ifndef __DEC_FILEDIALOGBOX_H__
#define __DEC_FILEDIALOGBOX_H__

#include "DecModal.h"
#include <filesystem>

class DecList;

class DecFileDialogBox : public DecModal {
public:    
    DecFileDialogBox();
    virtual ~DecFileDialogBox() override;

    virtual std::string type() override { return "MessageBox"; }

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    void onOK();
    void updateLists();

    void getFile( std::function<void(DecFileDialogBox &dlg, const char *filepath)> callback );
protected:
    std::function<void(DecFileDialogBox &dlg, const char *filepath)> _callback;

    DecList *_pListDir;
    DecList *_pListFile;
    DecText *_pTextPath;
    std::filesystem::path _cwd;
    std::filesystem::path fullpath;

    std::vector<std::string> _dirlist;
    std::vector<std::string> _filelist;    
};

DecFileDialogBox &FileDialogBox();

#endif //__DEC_FILEDIALOGBOX_H__