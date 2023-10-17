//
//  PESUndoRedoCommand.hpp
//  PES5Template
//
//  Created by SIHARAT THAMMAYA on 10/10/2566 BE.
//

#ifndef PESUndoRedoCommand_hpp
#define PESUndoRedoCommand_hpp

#include <stdio.h>
#include <string>
#include "modules/pes/include/pesEngine.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkFont.h"
#include "include/core/SkData.h"
#include <set>

class PesPathEdit;
class PesStitchEdit;

// Interface Class
class Command {
public:
    Command(std::string _name);
    virtual ~Command() {
    }

    std::function<void()> execute;
    std::function<void()> unExecute;

    std::string name;
    std::string description;
};

class UndoRedo {
public:
    UndoRedo();
    ~UndoRedo();
    
    void pushCommand(Command* command);
    void pushCommand(std::shared_ptr<Command> command);
    
    void redo();
    void undo();
    
    bool canRedo();
    bool canUndo();
    
    std::string getRedoName();
    std::string getUndoName();
    
    void clear();
    
private:
    std::vector<std::shared_ptr<Command>> _undoCommands;
    std::vector<std::shared_ptr<Command>> _redoCommands;
};

// Implement Class
class TranslateCommand : public Command {
public:
    TranslateCommand(std::set<int> indices, float tx, float ty);
};

class ScaleCommand : public Command {
public:
    ScaleCommand(std::set<int> indices, float sx, float sy, pesVec2f origin);
};

class RotateCommand : public Command {
public:
    RotateCommand(std::set<int> indices, float degree, pesVec2f pivot);
};

class DuplicateCommand : public Command {
public:
    DuplicateCommand(int index);
};

class DeleteCommand : public Command {
public:
    DeleteCommand(int index);
    DeleteCommand(std::set<int> indices);
};

class SendBackwardCommand : public Command {
public:
    SendBackwardCommand(int index);
};

class BringForwardCommand : public Command {
public:
    BringForwardCommand(int index);
};

class MergeCommand : public Command {
public:
    MergeCommand(int index);
};

class BreakCommand : public Command {
public:
    BreakCommand(int index);
};

class VertitalFlipCommand : public Command {
public:
    VertitalFlipCommand(int index);
};

class HorizontalFlipCommand : public Command {
public:
    HorizontalFlipCommand(int index);
};

class ColorBlockSwapCommand : public Command {
public:
    ColorBlockSwapCommand(pesData * pes, int index1, int index2);
};

class ImportCommand : public Command {
public:
    ImportCommand(pesData * data);
};

class ChangeParameterCommand : public Command {
public:
    ChangeParameterCommand(int index, pesData::Parameter oldParam, pesData::Parameter newParam, std::vector<pesPath> oldPaths, std::vector<pesPath> newPaths);
};

// PathEdit
class PathEditMoveCommand : public Command {
public:
    PathEditMoveCommand(PesPathEdit * pathEdit, std::vector<pesPath> oldPaths, std::vector<pesPath> newPaths);
};

// StitchEdit
class StitchEditMoveCommand : public Command {
public:
    StitchEditMoveCommand(PesStitchEdit * stitchEdit, pesVec2f oldPoint, pesVec2f newPoint, pesVec2f * selectedNode);
    StitchEditMoveCommand(PesStitchEdit * stitchEdit, pesVec2f oldPoint, pesVec2f newPoint, pesStitchBlock *selectedBlock, int selectedIndex);
};

class StitchEditInsertCommand : public Command {
public:
    StitchEditInsertCommand(PesStitchEdit * stitchEdit);
};

class StitchEditRemoveCommand : public Command {
public:
    StitchEditRemoveCommand(PesStitchEdit * stitchEdit);
};


class StitchEditStitchBlockCommand : public Command {
public:
    StitchEditStitchBlockCommand(PesStitchEdit * stitchEdit, pesStitchBlockList oldFillBlocks, pesStitchBlockList oldStrokeBlocks, pesStitchBlockList fillBlocks, pesStitchBlockList strokeBlocks);
};

#endif /* PESUndoRedoCommand_hpp */
