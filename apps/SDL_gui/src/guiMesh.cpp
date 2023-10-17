//
//  guiMesh.cpp
//  VG2EMB
//
//  Created by Siharat on 6/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiMesh.hpp"
#include "SDL2_gfxPrimitives.h"

guiMesh::guiMesh(){
    bVertsChanged = false;
    bIndicesChanged = false;
}

guiMesh::~guiMesh(){
    clear();
}

void guiMesh::clear(){
    if(!vertices.empty()){
        bVertsChanged = true;
        vertices.clear();
    }
    if(!indices.empty()){
        bIndicesChanged = true;
        indices.clear();
    }
}

bool guiMesh::haveVertsChanged(){
    if(bVertsChanged){
        bVertsChanged = false;
        return true;
    }else{
        return false;
    }
}

bool guiMesh::haveIndicesChanged(){
    if(bIndicesChanged){
        bIndicesChanged = false;
        return true;
    }else{
        return false;
    }
}

bool guiMesh::hasVertices() const{
    return !vertices.empty();
}

bool guiMesh::hasIndices() const{
    return !indices.empty();
}

void guiMesh::addVertex(const vectype& v){
    vertices.push_back(v);
    bVertsChanged = true;
}

void guiMesh::addVertices(const vector<vectype>& verts){
    vertices.insert(vertices.end(),verts.begin(),verts.end());
    bVertsChanged = true;
}

void guiMesh::addVertices(const vectype* verts, std::size_t amt){
    vertices.insert(vertices.end(),verts,verts+amt);
    bVertsChanged = true;
}

guiIndexType guiMesh::getIndex(guiIndexType i) const{
    return indices[i];
}

void guiMesh::addIndex(guiIndexType i){
    indices.push_back(i);
    bIndicesChanged = true;
}

void guiMesh::addIndices(const vector<guiIndexType>& inds){
    indices.insert(indices.end(),inds.begin(),inds.end());
    bIndicesChanged = true;
}

void guiMesh::addIndices(const guiIndexType* inds, std::size_t amt){
    indices.insert(indices.end(),inds,inds+amt);
    bIndicesChanged = true;
}

void guiMesh::addTriangle(guiIndexType index1, guiIndexType index2, guiIndexType index3) {
    addIndex(index1);
    addIndex(index2);
    addIndex(index3);
}

vectype guiMesh::getVertex(guiIndexType i) const{
    return vertices[i];
}

std::size_t guiMesh::getNumVertices() const{
    return vertices.size();
}

std::size_t guiMesh::getNumIndices() const{
    return indices.size();
}

vectype* guiMesh::getVerticesPointer(){
    return vertices.data();
}

guiIndexType* guiMesh::getIndexPointer(){
    return indices.data();
}

const vector<vectype> & guiMesh::getVertices() const {
    return vertices;
}

const vector<guiIndexType> & guiMesh::getIndices() const {
    return indices;
}

vectype guiMesh::getCentroid() const {
    if(vertices.size() == 0) {
        GUI_LogWarning("guiMesh") << "getCentroid(): mesh has no vertices, returning ofVec3f(0, 0)";
        return vectype();
    }
    
    vectype sum;
    for(guiIndexType i = 0; i < vertices.size(); i++) {
        sum += vertices[i];
    }
    sum /= (float)vertices.size();
    return sum;
}

void guiMesh::setupIndicesAuto(){
    bIndicesChanged = true;
    indices.resize(vertices.size());
    for(guiIndexType i = 0; i < vertices.size();i++){
        indices[i]=i;
    }
}

void guiMesh::clearVertices(){
    vertices.clear();
    bVertsChanged=true;
}

void guiMesh::clearIndices(){
    indices.clear();
    bIndicesChanged = true;
}

void guiMesh::drawVertices(int dx, int dy, SDL_Color c) const{
    vector<SDL_Point> points;
    for(auto v:vertices){
        points.push_back({(int)round(v.x)+dx, (int)round(v.y)+dy});
    }
    SDL_SetRenderDrawColor(GUI_renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoints(GUI_renderer, points.data(), points.size());
}

void guiMesh::drawWireframe(int dx, int dy, SDL_Color c) const{
    vector<SDL_Point> points;
    for(auto v:vertices){
        points.push_back({(int)round(v.x)+dx, (int)round(v.y)+dy});
    }
    SDL_SetRenderDrawColor(GUI_renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLines(GUI_renderer, points.data(), points.size());
}

void guiMesh::draw(int dx, int dy, SDL_Color c) const{
//    GUI_Log("guiMesh::draw\n");
    if(hasVertices() && hasIndices()){
        for(int i=0; i<(int)getNumIndices(); i+=3){
            auto v0 = getVertex(getIndex(i));
            auto v1 = getVertex(getIndex(i+1));
            auto v2 = getVertex(getIndex(i+2));
            filledTrigonRGBA(GUI_renderer,
                             (Sint16)(round(v0.x)+dx), (Sint16)(round(v0.y)+dy),
							 (Sint16)(round(v1.x)+dx), (Sint16)(round(v1.y)+dy),
							 (Sint16)(round(v2.x)+dx), (Sint16)(round(v2.y)+dy),
                             c.r, c.g, c.b, c.a);

        }
        
    }
//    GUI_Log("guiMesh::draw done\n");
}

void guiMesh::append(const guiMesh & mesh){
//    GUI_Log("guiMesh::append\n");
    guiIndexType prevNumVertices = static_cast<guiIndexType>(vertices.size());
    if(mesh.getNumVertices()){
        vertices.insert(vertices.end(), mesh.getVertices().begin(),mesh.getVertices().end());
    }

    if(mesh.getNumIndices()){
        for(auto index : mesh.getIndices()){
            indices.push_back(index+prevNumVertices);
        }
    }
//    GUI_Log("guiMesh::append done\n");
}
