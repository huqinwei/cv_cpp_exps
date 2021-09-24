/*
	*Entrance to tile node segmentation progress.
	*Build quadtree by node segmentation.
	*author:hqw
*/

#include <iostream>
#include <fstream>

//#include "Sample.h"
#include <boost/program_options.hpp>
#include "MeshBase/MeshBase.h"
#include "Simplifier/Simplifier.h"
#include "TileNodeSegmentation.h"

//#define TEST_SIMPLIFIER
// #define TEST_MESHBASE
#define TEST_NODE_SEGMENTATION
//#define LOD_TEST


int func(std::string input_file, std::string output_file);

int main()
{
	bool hello = false;

	hello = true;
	int i;
	bool b;


	std::cout << hello << std::endl;
	return 0;


#ifdef LOD_TEST
	TileNodeSegmentation sol0;
	std::string father_nodes="F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L17_03.obj";
	std::vector<std::string> child_nodes;
	child_nodes.push_back("F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L18_030.obj");
	child_nodes.push_back("F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L18_031.obj");
	child_nodes.push_back("F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L18_032.obj");
	child_nodes.push_back("F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L18_033.obj");

	std::string test_file = "F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L18_032.obj";
	test_file =				"F:/handwork_data/data_out/27_MJSJHY/MJSJHY_L17_00.obj";
	//sol0.test_osg_read(test_file);
	sol0.create_group_pagelod_(father_nodes, child_nodes,0.1f);
	return 0;
#endif


#ifdef TEST_NODE_SEGMENTATION//分割流程
	std::string input_file1 = "F:/handwork_data/data/27_MJSJHY/MJSJHY.obj";
	std::string output_dir = "F:/handwork_data/data_out/27_MJSJHY/";//             F:/handwork_data/data/27_MJSJHY_out/
	TileNodeSegmentation sol;
	sol.main_process(input_file1, output_dir);
	//sol.node_segment_qy();
	return 0;
#endif//分割流程

#ifdef TEST_SIMPLIFIER
	MeshBase* mesh = new MeshBase;

	Simplifier sim;
	std::string input_file = "F:/handwork_data/data_out/27_MJSJHY_/MJSJHY_L18_003.obj";
	std::string output_file = "F:/handwork_data/data_out/27_MJSJHY_/MJSJHY_L18_003_out.obj";
#if 0//实验代码，openmesh处理法线
	func(input_file, output_file);
	sim.simplify_obj(output_file, output_file, 0.6);
#else
	sim.simplify_obj(input_file, output_file, 0.6);
#endif
	//sim.simplify_obj(input_file, output_file, 0.6,mesh);
	return 0;
#endif

#ifdef TEST_MESHBASE
	MeshBase mesh_io;

	//mesh_io.read_obj_file("D:/git_dir/CV_experiment/openmesh_example/data/YYC.obj");
	//D:\git_dir\CV_experiment\openmesh_example\data\my_dota2_cube.OBJ
	mesh_io.read_obj_file("D:/git_dir/CV_experiment/openmesh_example/data/my_dota2_cube_triangle.OBJ");//四边形
	mesh_io.heh_iteration();
	mesh_io.face_vertex_iteration();
	mesh_io.vertex_move(OpenMesh::Vec3f(30, 20, 50));
	mesh_io.write_obj_file("D:/git_dir/CV_experiment/openmesh_example/data/YYC_0826_moved.obj");
	return 0;
#endif
}


int func(std::string input_file, std::string output_file)
//openmesh计算法向量测试代码
{
	DefaultTriMesh mesh;
	// 	mesh.request_vertex_status();
	// 	mesh.request_vertex_colors();
	// 	mesh.request_vertex_texcoords2D();
	// 	mesh.request_halfedge_texcoords2D();
	// 	mesh.request_face_normals();
	// 	mesh.request_face_colors();
	//	mesh.request_face_texture_index();

	mesh.request_vertex_normals();
	if (!mesh.has_vertex_normals())
	{
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
		return 1;
	}
	// 	opt += OpenMesh::IO::Options::VertexTexCoord;
	// 	opt += OpenMesh::IO::Options::FaceColor;
	// 	opt += OpenMesh::IO::Options::ColorAlpha;
	// 	opt += OpenMesh::IO::Options::FaceTexCoord;



	OpenMesh::IO::Options ropt, wopt;
	ropt += OpenMesh::IO::Options::VertexNormal;
	//ropt += OpenMesh::IO::Options::VertexTexCoord;
	wopt += OpenMesh::IO::Options::VertexNormal;
	//wopt += OpenMesh::IO::Options::VertexTexCoord;//todo:写不出来东西
	if (!OpenMesh::IO::read_mesh(mesh, input_file, ropt))
	{
		return 1;
	}
	// If the file did not provide vertex normals, then calculate them
	if (!ropt.check(OpenMesh::IO::Options::VertexNormal))
	{
		mesh.request_face_normals();
		mesh.update_normals();
		//mesh.release_face_normals();
	}
	// 	mesh.release_vertex_normals();

	OpenMesh::IO::write_mesh(mesh, output_file, ropt);
	return 0;
}