#ifndef _FINDBOUNDARY_H
#define _FINDBOUNDARY_H
#include<iostream>
#include<string>
#include<vector>
#include <set>
#include<algorithm>

template<class MeshT>
bool FindBoundary(MeshT* _mesh, const std::string& head_filename, std::vector<typename MeshT::VertexHandle>& head, const std::string& tail_filename, std::vector<typename MeshT::VertexHandle>& tail, 
    int overlap, const std::string& edgefile, const std::string& objfile, std::vector<std::vector<typename MeshT::VertexHandle>>& merge_boundary);
#endif