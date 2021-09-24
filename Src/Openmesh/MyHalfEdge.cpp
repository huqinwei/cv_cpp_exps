#include "Openmesh/MyHalfEdge.h"




bool check(OpenMesh::IO::BaseExporter& _be, OpenMesh::IO::Options _opt)
{
	if (_opt.check(OpenMesh::IO::Options::VertexNormal) <= _be.has_vertex_normals())
		std::cout << "true" << std::endl;
	if (_opt.check(OpenMesh::IO::Options::VertexTexCoord) <= _be.has_vertex_texcoords())
		std::cout << "true" << std::endl;
	if (_opt.check(OpenMesh::IO::Options::VertexColor) <= _be.has_vertex_colors())
		std::cout << "true" << std::endl;
	if (_opt.check(OpenMesh::IO::Options::FaceNormal) <= _be.has_face_normals())
		std::cout << "true" << std::endl;
	if (_opt.check(OpenMesh::IO::Options::FaceColor) <= _be.has_face_colors())
		std::cout << "true" << std::endl;

	return (_opt.check(OpenMesh::IO::Options::VertexNormal) <= _be.has_vertex_normals())
		&& (_opt.check(OpenMesh::IO::Options::VertexTexCoord) <= _be.has_vertex_texcoords())
		&& (_opt.check(OpenMesh::IO::Options::VertexColor) <= _be.has_vertex_colors())
		&& (_opt.check(OpenMesh::IO::Options::FaceNormal) <= _be.has_face_normals())
		&& (_opt.check(OpenMesh::IO::Options::FaceColor) <= _be.has_face_colors());
}



int MyTriMesh_custom_IO() {

    MyTriMesh mesh;
    std::string output_path = "D:/git_dir/CV_experiment/openmesh_example/data/MJSJHY_write_with_texcoord_0817.obj";
    std::string output_path_2 = "D:/git_dir/CV_experiment/openmesh_example/data/MJSJHY_write_with_texcoord_0817";
    std::string output_path_3 = "D:/git_dir/CV_experiment/openmesh_example/data/MJSJHY_write_with_texcoord_0817_.obj";

    std::string input_path = "D:/git_dir/CV_experiment/openmesh_example/data/MJSJHY.obj";

    //ÆÀ¹ÀÃüÁîÐÐ
    OpenMesh::IO::Options ropt, wopt;
	wopt += IO::Options::VertexTexCoord;
	wopt += IO::Options::FaceColor;
	wopt += IO::Options::ColorAlpha;
	wopt += IO::Options::VertexNormal;
    
    //wopt += IO::Options::VertexColor;

    ropt += IO::Options::VertexTexCoord;
	ropt += IO::Options::FaceColor;
	ropt += IO::Options::ColorAlpha;
	ropt += IO::Options::VertexNormal;
    //ropt += IO::Options::VertexColor;

    mesh.request_vertex_status();
    mesh.request_vertex_texcoords2D();
    mesh.request_halfedge_texcoords2D();
    mesh.request_face_colors();
    mesh.request_vertex_normals();
    

    ///////////////////////////////////////


    //
    std::fstream in(input_path.c_str(), std::ios_base::in);
    auto reader = new OpenMesh::IO::_OBJReader_;
    OpenMesh::IO::Options opt;
	OpenMesh::IO::ImporterT<MyTriMesh> bi(mesh);
	//reader->read(in, bi, ropt);//
	reader->read(input_path.c_str(), bi, ropt);
    //typedef std::map<std::string, Material> MaterialList;
    //MaterialList  ml = static_cast<MaterialList>(reader->get_MaterialList());//"GZ_MJSIHY_MainBake"
    in.close();


    OpenMesh::MPropHandleT< std::map< int, std::string > > property;

    auto hand = mesh.get_property_handle(property, "TextureMapping");
    auto res = mesh.property(property);


    auto face_ret = mesh.n_faces();


    std::fstream out(output_path.c_str(), std::ios_base::out);

    OpenMesh::IO::ExporterT<MyTriMesh> be(mesh);
    auto writer = new OpenMesh::IO::_OBJWriter_;

    //bool check_ret = check(be, wopt);


    bool write_res = writer->write(output_path, be, wopt);

    out.close();



    return 0;
   


}
