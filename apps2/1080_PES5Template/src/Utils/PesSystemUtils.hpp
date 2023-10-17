//
//  PesSystemUtils.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 19/7/2565 BE.
//

#ifndef PesSystemUtils_hpp
#define PesSystemUtils_hpp

#include <stdio.h>
#include <string>
#include <vector>

class PesFileDialogResult{
    public:
        PesFileDialogResult();
        std::string getName();
        std::string getPath();
    
        std::string filePath; ///< full path to selected file or directory
        std::string fileName; ///< selected file or directory name
        bool bSuccess; ///< true if the dialog action was successful, aka file select not cancel
};

PesFileDialogResult PesSystemSaveDialog(std::string defaultName, std::string messageName);
PesFileDialogResult PesSystemLoadDialog(std::string windowTitle="", bool bFolderSelection = false, std::string defaultPath="",
                                        std::vector<std::string> allowExtensions=std::vector<std::string>());

#endif /* PesSystemUtils_hpp */
