#ifndef _MERGE_H_
#define _MERGE_H_
#include <fstream>
#include<Windows.h>

#include <Constants.h>
#include"MeshBase/MeshBase.h"
#include<iostream>
#include<string>
#include<vector>
#include <set>
#include<algorithm>
#include<stack>

typedef DefaultTriMesh TriMesh;
//extern "C" void AI3D_API hi();

// class AI3D_API Sample
// {
// public:
// 	Sample();
// 	virtual ~Sample();
// 	void hi();
// };
class Merge
{
public:
	Merge(MeshBase *mesh):m_mesh(mesh){}
	~Merge() 
	{
		if (m_mesh != NULL)
			delete m_mesh;
	}
	template<class MeshT>
	bool FindBoundary(MeshT* _mesh, const std::string& head_filename, std::vector<typename MeshT::VertexHandle>& head, const std::string& tail_filename, std::vector<typename MeshT::VertexHandle>& tail,
		int overlap, const std::string& edgefile, const std::string& objfile, std::vector<std::vector<typename MeshT::VertexHandle>>& merge_boundary);
	void add_face(TriMesh& m, std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_0,std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_1,
		std::vector<TriMesh::VertexHandle>& head_0, std::vector<TriMesh::VertexHandle>& tail_0, std::vector<TriMesh::VertexHandle>& head_1, std::vector<TriMesh::VertexHandle>& tail_1, const double& move_dist, const std::string& dst_obj);
private:
	MeshBase* m_mesh;
	 
};


#endif // _SAMPLE_H_