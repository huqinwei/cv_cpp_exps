#ifndef _MESH_BASE_H_
#define _MESH_BASE_H_

#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <filesystem>
#include <limits>
///////////////////////////////////////////////////////////////////////////////////////
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Tools/Utils/getopt.h>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/IO/reader/OBJReader.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/Mesh/PolyMeshT.hh>
#include <OpenMesh/Core/Mesh/TriMeshT.hh>
#include <Constants.h>

#include "Material.h"
///////////////////////////////////////////////////////////////////////////////////////
struct TriTraits : public OpenMesh::DefaultTraits
{
	/// Use double precision points
	typedef OpenMesh::Vec3d Point;
	/// Use double precision Normals
	typedef OpenMesh::Vec3d Normal;

	/// Use RGBA Color
	typedef OpenMesh::Vec4f Color;
};
typedef AI3D_API OpenMesh::TriMesh_ArrayKernelT<TriTraits> DefaultTriMesh;


class AI3D_API MeshBase {
public:
	//typedef  OpenMesh::IO::Material  Material;
	typedef std::map<std::string, Material> MaterialList;

	MeshBase();
	//int MyTriMesh_custom_IO_polymesh();

	//	//修改点云坐标
	void vertex_move(OpenMesh::Vec3f& point_add);

	//访问面片和点云
	void face_vertex_iteration(); 
	void heh_iteration(); 
	//https://mcoder.cc/2019/07/06/half_edge_data_structure/

	int read_obj_file(std::string file_name);
	int recompute_face_normal();
	int write_obj_file(std::string file_name);

	std::string textureIndexPropertyName_;
	bool    textureIndexPropFetched_;
	std::map<int, std::string> texIndexFileMap_;
	DefaultTriMesh m_mesh;
	void get_bbox(OpenMesh::Vec3d &ptmin, OpenMesh::Vec3d& ptmax);

	std::vector<std::string>& get_texture_filename() {		return texture_filename;	}

private:

	template< class MeshT >
	bool writeMesh(std::ostream& _out, std::string _filename, MeshT& _mesh);

	template< class MeshT >
	bool writeMaterial(std::string _filename, MeshT& _mesh);

	template< class MeshT >
	Material& getMaterial(MeshT& _mesh, const OpenMesh::FaceHandle& _fh);

	template< class MeshT >
	Material& test(Material& mt);

	OpenMesh::IO::Options ropt_, wopt_;

	std::vector<std::string> texture_filename; //供查询



private:
	MaterialList materials_;//供writer使用，目前专用
};

//字符串分割函数
inline std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
inline void get_file_name_parts(const std::string& filename, std::string& absolute_path, std::string& base_name) {
	std::vector<std::string>  res_vec2 = split(filename, "/");
	std::string res2 = res_vec2[res_vec2.size() - 1];
	std::vector<std::string>  res_vec3 = split(res2, ".");
	base_name = res_vec3[0];
	absolute_path = filename.substr(0, filename.size() - res2.size());
}


#endif//_MESH_BASE_H_