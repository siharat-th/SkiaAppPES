//
//  guiMesh.hpp
//  VG2EMB
//
//  Created by Siharat on 6/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiMesh_hpp
#define guiMesh_hpp

#include <stdio.h>
#include "SDL_gui.h"
#include "tesselator.h"
#include "ofVec3f.h"

typedef TESSindex guiIndexType;

// 2d triangle mesh design for sdl2_gfx
#define vectype ofVec2f
class guiMesh{
    
public:
    
    guiMesh();
    ~guiMesh();
    
    void addVertex(const vectype& v);
    void addVertices(const vector<vectype>& verts);
    void addVertices(const vectype* verts, std::size_t amt);
    void clearVertices();
    std::size_t getNumVertices() const;
    vectype* getVerticesPointer();
    vectype getVertex(guiIndexType i) const;
    const vector<vectype> & getVertices() const;
    bool haveVertsChanged();
    bool hasVertices() const;
    
    void append(const guiMesh & mesh);
    void mergeDuplicateVertices();
    
    vectype getCentroid() const;
    
    void setupIndicesAuto();
    
    
    void addIndex(guiIndexType i);
    void addIndices(const vector<guiIndexType>& inds);
    void addIndices(const guiIndexType* inds, std::size_t amt);
    void clearIndices();
    guiIndexType getIndex(guiIndexType i) const;
    std::size_t getNumIndices() const;
    guiIndexType* getIndexPointer();
    const vector<guiIndexType> & getIndices() const;
    bool haveIndicesChanged();
    bool hasIndices() const;
    void addTriangle(guiIndexType index1, guiIndexType index2, guiIndexType index3);
    
    void clear();
    
    // draw point cloud.
    void drawVertices(int dx=0, int dy=0, SDL_Color c=cBlack) const;
    
    // draw the mesh as lines.
    void drawWireframe(int dx=0, int dy=0, SDL_Color c=cGrey) const;
    
    // fill the mesh as triangles
    void draw(int dx=0, int dy=0, SDL_Color c=cDarkGrey) const;
    
private:
    vector<vectype> vertices;
    vector<guiIndexType> indices;
    
    bool bVertsChanged, bIndicesChanged;
};

#endif /* guiMesh_hpp */
