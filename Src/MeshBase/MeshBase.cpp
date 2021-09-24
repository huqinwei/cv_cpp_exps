#include "MeshBase/MeshBase.h"

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
MeshBase::MeshBase() {
	wopt_ += OpenMesh::IO::Options::VertexTexCoord;
	wopt_ += OpenMesh::IO::Options::FaceColor;
	wopt_ += OpenMesh::IO::Options::ColorAlpha;
	wopt_ += OpenMesh::IO::Options::VertexNormal;
	wopt_ += OpenMesh::IO::Options::FaceTexCoord;

	ropt_ += OpenMesh::IO::Options::VertexTexCoord;
	ropt_ += OpenMesh::IO::Options::FaceColor;
	ropt_ += OpenMesh::IO::Options::ColorAlpha;
	ropt_ += OpenMesh::IO::Options::VertexNormal;
	ropt_ += OpenMesh::IO::Options::FaceTexCoord;
	m_mesh.request_vertex_status();
	m_mesh.request_vertex_normals();
	m_mesh.request_vertex_colors();
	m_mesh.request_vertex_texcoords2D();
	m_mesh.request_halfedge_texcoords2D();
	m_mesh.request_face_normals();
	m_mesh.request_face_colors();
	m_mesh.request_face_texture_index();
	m_mesh.request_face_texture_index();

};
int MeshBase::recompute_face_normal() 
{
	if (!ropt_.check(OpenMesh::IO::Options::VertexNormal))
	{
		// we need face normals to update the vertex normals
		m_mesh.request_face_normals();
		// let the mesh update the normals
		m_mesh.update_normals();
		// dispose the face normals, as we don't need them anymore
		m_mesh.release_face_normals();
	}
	return 0;
};

int MeshBase::read_obj_file(std::string file_name) {
	auto reader = new OpenMesh::IO::_OBJReader_;
	OpenMesh::IO::Options opt;
	OpenMesh::IO::ImporterT<DefaultTriMesh> bi(m_mesh);
	reader->read(file_name.c_str(), bi, ropt_);
	//materials_ = reader->get_MaterialList();//"GZ_MJSIHY_MainBake"             static_cast<MaterialList>


	//遍历FACE HANDLE
	if (0)
	{
		OpenMesh::FaceHandle fh, fh2, fh3;
		//m_mesh.get_property_handle(fh, "");
		m_mesh.set_texture_index(fh, 0);
		m_mesh.set_texture_index(fh2, 1);
		m_mesh.set_texture_index(fh2, 1);
		auto a = m_mesh.texture_index(fh);
		auto b = m_mesh.texture_index(fh2);
		auto c = m_mesh.texture_index(fh3);
	}

	OpenMesh::MPropHandleT< std::map< int, std::string > > property;

	if (m_mesh.get_property_handle(property, "TextureMapping"))
	{
		auto hand = m_mesh.get_property_handle(property, "TextureMapping");
		auto res = m_mesh.property(property);//有些文件会报错
		for (auto data : res) {
			texture_filename.push_back(data.second);
		}

		auto face_ret = m_mesh.n_faces();
	}
	else
	{
		std::cout << "############no textureMapping!!!!!!!!!!" << std::endl;

	}


	return 0;
}

template< class MeshT >
bool MeshBase::writeMesh(std::ostream& _out, std::string _filename, MeshT& _mesh) {

	unsigned int i, nV, idx;
	OpenMesh::Vec3f v, n;
	OpenMesh::Vec2f t(0.0f, 0.0f);
	//typename MeshT::VertexHandle vh;
	OpenMesh::VertexHandle vh;
	bool useMaterial = false;
	OpenMesh::Vec4f c;

	bool optionFaceColors = true;
	bool optionVertexNormals = true;
	bool optionVertexTexCoords = true;
	bool optionTextures = false;
	bool optionCopyTextures = false;
	bool optionCreateTexFolder = false;
	//create material file if needed


	std::string base_name;
	std::string absolute_path;
	std::string file_path;
	std::filesystem::path path(_filename);

	if (path.has_stem()) {
		base_name = path.stem().string();
	}

	if (path.has_parent_path()) {
		file_path = path.parent_path().string();
	}



	std::string matFile = absolute_path + base_name + ".mtl";

	if (optionFaceColors || optionTextures) {
		useMaterial = writeMaterial(matFile, _mesh);
	}

	// Header
	_out << "# " << _mesh.n_vertices() << " vertices, ";
	_out << _mesh.n_faces() << " faces" << '\n';

	// Material file
	if (useMaterial && optionFaceColors)
		_out << "mtllib " << base_name << ".mtl" << '\n';

	// Store indices of vertices in a map such that they can easily be referenced for face definitions later on
	std::map<typename OpenMesh::VertexHandle, int> vtMapV;

	int cf = 1;
	// vertex data (point, normals, texcoords)
	for (i = 0, nV = _mesh.n_vertices(); i < nV; ++i)
	{
		vh = typename OpenMesh::VertexHandle(i);
		v = _mesh.point(vh);
		n = _mesh.normal(vh);

		if (_mesh.has_vertex_texcoords2D() && !_mesh.has_halfedge_texcoords2D())
			t = _mesh.texcoord2D(vh);

		// Write out vertex coordinates
		_out << "v " << v[0] << " " << v[1] << " " << v[2] << '\n';

		// Write out vertex coordinates
		if (optionVertexNormals)
			_out << "vn " << n[0] << " " << n[1] << " " << n[2] << '\n';

		// Write out vertex texture coordinates
		if (optionVertexTexCoords && _mesh.has_vertex_texcoords2D() && !_mesh.has_halfedge_texcoords2D()) {
			_out << "vt " << t[0] << " " << t[1] << '\n';
			vtMapV.insert(std::pair<typename MeshT::VertexHandle, int>(vh, cf));
			cf++;
		}
	}

	typename MeshT::FaceVertexIter fv_it;
	typename MeshT::FaceHalfedgeIter fh_it;
	typename MeshT::FaceIter f_it;

	// Store indices of vertex coordinate (in obj-file) in map such that the corresponding halfedge can easily be found later on
	std::map<typename MeshT::HalfedgeHandle, int> vtMap;

	// If m_mesh has halfedge tex coords, write them out instead of vertex texcoords
	if (optionVertexTexCoords && _mesh.has_halfedge_texcoords2D()) {
		int count = 1;
		for (f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it) {
			for (fh_it = _mesh.fh_iter(*f_it); fh_it.is_valid(); ++fh_it) {
				typename MeshT::TexCoord2D t = _mesh.texcoord2D(*fh_it);
				_out << "vt " << t[0] << " " << t[1] << '\n';
				vtMap.insert(std::pair<typename MeshT::HalfedgeHandle, int>(*fh_it, count));
				count++;
			}
		}
	}

	Material lastMat;

	// we do not want to write seperators if we only write vertex indices
	bool vertexOnly = !(optionVertexTexCoords && _mesh.has_halfedge_texcoords2D())
		&& !(optionVertexTexCoords && !_mesh.has_halfedge_texcoords2D() && _mesh.has_vertex_texcoords2D())
		&& !(optionVertexNormals);

	for (f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it) {

		if (useMaterial && optionFaceColors) {

			Material& material = getMaterial(_mesh, *f_it);
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// If we are ina a new material block, specify in the file which material to use
			if (lastMat != material) {
				_out << "g " << material << '\n';
				_out << "usemtl " << material << '\n';
				lastMat = material;
			}
		}

		_out << "f";

		// Write out face information
		for (fh_it = _mesh.fh_iter(*f_it); fh_it.is_valid(); ++fh_it) {

			// Write vertex index
			idx = _mesh.to_vertex_handle(*fh_it).idx() + 1;
			_out << " " << idx;

			if (!vertexOnly) {

				// Write separator
				_out << "/";

				if (optionVertexTexCoords) {
					// Write vertex texture coordinate index
					if (optionVertexTexCoords && _mesh.has_halfedge_texcoords2D()) {
						// Refer to halfedge texture coordinates
						typename std::map<typename MeshT::HalfedgeHandle, int>::iterator it = vtMap.find(*fh_it);
						if (it != vtMap.end())
							_out << (*it).second;
					}
					else if (optionVertexTexCoords && !_mesh.has_halfedge_texcoords2D() && _mesh.has_vertex_texcoords2D()) {
						// Refer to vertex texture coordinates
						typename std::map<typename MeshT::VertexHandle, int>::iterator it = vtMapV.find(_mesh.to_vertex_handle(*fh_it));
						if (it != vtMapV.end())
							_out << (*it).second;
					}
				}

				// Write vertex normal index
				if (optionVertexNormals) {
					// Write separator
					_out << "/";

					_out << idx;
				}
			}
		}

		_out << '\n';
	}

	// Copy texture files (if demanded)
	if (optionCopyTextures) {
		// Only test existence of folder once
		// (for multiple textures)
		/*
		bool testedOnce = false;
		for (MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {
			Material& mat = (*it).second;

			if (!mat.has_Texture()) continue;

			QImage img(mat.map_Kd().c_str());
			QFileInfo img_f(mat.map_Kd().c_str());

			if (img.isNull()) {
				// Something happened wrong
				std::cerr << "An error occurred when trying to copy a texture file." << std::endl;
				continue;
			}
			else {
				if (optionCreateTexFolder) {
					// Create folder
					QDir dir(fi.absolutePath());
					if (!testedOnce && dir.exists(fi.absolutePath() + QDir::separator() + fi.baseName() + "_textures")) {
						std::cerr << "The specified target folder already contains a subfolder called textures. Skipping!" << std::endl;
						continue;
					}
					else {
						dir.mkdir(fi.baseName() + "_textures");
						img.save(fi.absolutePath() + QDir::separator() + fi.baseName() + "_textures" + QDir::separator() + img_f.fileName());
						testedOnce = true;
					}

				}
				else {

					//img.save(absolute_path + "/" + img_f.fileName());
				}
			}
		}*/

	}

	materials_.clear();
	texIndexFileMap_.clear();
	textureIndexPropFetched_ = false;

	return true;
}





template< class MeshT >
bool MeshBase::writeMaterial(std::string mat_file_name, MeshT& _mesh)
{
	bool optionColorAlpha = false;
	bool optionTextures = true;
	bool optionCopyTextures = true;
	bool optionCreateTexFolder = false;

	std::fstream matStream(mat_file_name.c_str(), std::ios_base::out);

	if (!matStream) {

		std::cerr << "writeMaterial : cannot not open file " << mat_file_name.c_str() << std::endl;;
		return false;
	}

	// \TODO Implement setting of all colors (diffuse, ambient and specular)
	// There's only diffuse colors so far
	OpenMesh::Vec4f c;
	materials_.clear();

	//iterate over faces
	typename MeshT::FaceIter f_it;
	typename MeshT::FaceIter f_end = _mesh.faces_end();

	// Prepare materials ( getMaterial handles a list that is set up by this call)
	for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it) {
		getMaterial(_mesh, *f_it);
	}

	//write the materials
	for (MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {
		Material& mat = (*it).second;
		matStream << "newmtl " << mat << '\n';
		matStream << "Ka 0.5000 0.5000 0.5000" << '\n';
		//ACG::Vec3f c = mat.Kd();
		OpenMesh::Vec3f c = mat.Kd();
		//matStream << "Kd " << c[0] / 255 << " " << c[1] / 255 << " " << c[2] / 255 << '\n';
		matStream << "Kd " << c[0] << " " << c[1] << " " << c[2] << '\n';
		if (optionColorAlpha) {
			matStream << "Tr " << mat.Tr() << '\n';
		}
		matStream << "illum 1" << '\n';

		// Write out texture info
		if (optionTextures && mat.has_Texture()) {
			if (optionCopyTextures) {
				// Use file path in target folder (relative)
				std::string absolute_mat_path;
				std::string base_mat_name;
				std::filesystem::path path(mat_file_name);

				if (path.has_stem()) {
					base_mat_name = path.stem().string();
				}

				if (path.has_parent_path()) {
					absolute_mat_path = path.parent_path().string();
				}


				if (optionCreateTexFolder) {
					matStream << "map_Kd " << base_mat_name << "_textures" << "/"
						<< mat_file_name << '\n';
				}
				else {
					matStream << "map_Kd " << mat.map_Kd() << '\n';
				}
			}
			else {
				// Use original file path
				matStream << "map_Kd " << mat.map_Kd() << '\n';
			}
		}

		matStream << '\n';
	}

	matStream.close();

	return true;
}

template< class MeshT >
Material& MeshBase::getMaterial(MeshT& _mesh, const OpenMesh::FaceHandle& _fh)
{
	// check options
	bool optionColorAlpha = false;
	// 	if (!OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
	// 		optionColorAlpha = saveAlpha_->isChecked();
		// \TODO Fetch options from ini states if dialog box is not available

	OpenMesh::Vec4f c = _mesh.color(_fh);

	// First off, try to fetch texture index of current face/object...
	if (!textureIndexPropFetched_) {
		//emit textureIndexPropertyName(_objId, textureIndexPropertyName_);
		textureIndexPropFetched_ = true;
	}

	int texIndex = -1;
	//
	OpenMesh::FPropHandleT< int > texture_index_property;
	if (_mesh.get_property_handle(texture_index_property, "OriginalTexIndexMapping")) {//textureIndexPropertyName_.toStdString()
		texIndex = _mesh.property(texture_index_property, _fh);
	}
	else if (_mesh.get_property_handle(texture_index_property, "f:textureindex")) {
		texIndex = _mesh.property(texture_index_property, _fh);
	}
	else if (_mesh.has_face_texture_index()) {
		texIndex = _mesh.texture_index(_fh);
	}
	else {
		//std::string texName;
		//emit getCurrentTexture(_objId, texName);
// 		if (texName != "NONE")
// 			emit textureIndex(texName, _objId, texIndex);
	}

	std::string filename;
	bool hasTexture = false;

	if (texIndex != -1) {

		// Search for texture index in local map
		std::map<int, std::string>::iterator it = texIndexFileMap_.find(texIndex);

		if (it != texIndexFileMap_.end()) {
			// We already know this file
			filename = (*it).second;
			hasTexture = true;
		}
		else {
			// A new texture file has been found
			std::string texName;
			//TODO:这个过程是反向查找，给定一个index，查找到一个材质名
			//emit textureName(_objId, texIndex, texName);
			int tex_index = _mesh.texture_index(_fh);

			OpenMesh::MPropHandleT< std::map< int, std::string > > property;
			auto map_tex = _mesh.get_property_handle(property, "TextureMapping");
			std::map< int, std::string >  res = _mesh.property(property);
			texName = res[tex_index];//texName本该是“代号”而不是filename，但是可能用处也不大，直接转文件名

			if (texName != "NOT_FOUND" && texName != "") {
				//emit textureFilename(_objId, texName, file_base_name);
				filename = texName;//
				// => Add to local map
				texIndexFileMap_.insert(std::pair<int, std::string>(texIndex, filename));
				hasTexture = true;
			}
		}
	}

	for (MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {

		// No texture has been found
		if (!hasTexture) {
			// ... just look for diffuse color in materials list
			if (((*it).second).Kd() == OpenMesh::Vec3f(c[0], c[1], c[2]) &&
				((optionColorAlpha && ((*it).second).Tr() == c[3]) || !optionColorAlpha))
				return (*it).second;
		}
		else {
			// Texture has been found, look for both, matching texture and color
			std::string mKd(((*it).second).map_Kd());
			if ((((*it).second).Kd() == OpenMesh::Vec3f(c[0], c[1], c[2]) &&
				((optionColorAlpha && ((*it).second).Tr() == c[3]) || !optionColorAlpha)) &&
				(filename == mKd && ((*it).second).map_Kd_index() == texIndex))
				return (*it).second;
		}
	}

	// If not found, add new material(s)
	Material mat;
	// Set diffuse color
	mat.set_Kd(c[0], c[1], c[2]);
	// Add transparency if available
	if (optionColorAlpha) mat.set_Tr(c[3]);
	mat.material_number(materials_.size());
	// Set texture info
	if (hasTexture)
		mat.set_map_Kd(filename, texIndex);

	materials_.insert(std::make_pair("Material"+std::to_string(mat.material_number()), mat));
	MaterialList::iterator it = materials_.end();
	--it;
	return (*it).second;
}


template< class MeshT >
Material& MeshBase::test(Material& mt) {


	Material mat;
	return mat;
}

using namespace std;

void MeshBase::face_vertex_iteration()
{
	for (int i = 0; i < 10; ++i) {
		auto face_h = m_mesh.face_handle(i);
		cout << "face index:" << face_h.idx() << endl;;
		OpenMesh::PolyConnectivity::FaceVertexIter fv_i;
		for (fv_i = m_mesh.fv_iter(face_h); fv_i != m_mesh.fv_end(face_h); fv_i++) {
			auto a = *fv_i;
			auto point = m_mesh.point(a);
			cout << "\tfv_i: " << a.idx();
			cout << "\txyz: " << point[0] << "," << point[1] << "," << point[2] << endl;
		}
	}


}
void MeshBase::heh_iteration()
{
	DefaultTriMesh::VertexIter v_it = m_mesh.vertices_begin();


// 	DefaultTriMesh::HalfedgeHandle heh, heh_init;
// 	heh = heh_init = m_mesh.halfedge_handle(v_it);
// 	heh = m_mesh.next_halfedge_handle(heh);
// 	while (heh != heh_init) {
// 		heh = m_mesh.next_halfedge_handle(heh);
// 		if (heh.is_valid()) {
// 			std::cout << m_mesh.edge_handle(heh).idx() << std::endl;
// 		}
// 	}


	DefaultTriMesh::HalfedgeIter he_it = m_mesh.halfedges_begin();
	DefaultTriMesh::HalfedgeIter he_it_end = m_mesh.halfedges_end();
	DefaultTriMesh::HalfedgeHandle heh;
	//std::cout << "count:"<< m_mesh.halfedges_end() - m_mesh.halfedges_begin() << std::endl;
	while (he_it != he_it_end) {
		heh = m_mesh.next_halfedge_handle(he_it);
		if (heh.is_valid()) {
			std::cout << m_mesh.edge_handle(heh).idx() << std::endl;
		}
		he_it++;
	}
}






void MeshBase::vertex_move(OpenMesh::Vec3f& point_add)
{
	DefaultTriMesh::VertexIter          v_it, v_end(m_mesh.vertices_end());
	DefaultTriMesh::VertexVertexIter    vv_it;
	for (v_it = m_mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		auto p_temp = m_mesh.point(*v_it);
		p_temp += point_add;
		m_mesh.set_point(*v_it, p_temp);
	}
}


int MeshBase::write_obj_file(std::string file_name) {
	OpenMesh::IO::ExporterT<DefaultTriMesh> be(m_mesh);
	std::fstream objStream(file_name.c_str(), std::ios_base::out);
	writeMesh(objStream, file_name, m_mesh);

	return 0;
}


void MeshBase::get_bbox(OpenMesh::Vec3d& ptmin, OpenMesh::Vec3d& ptmax) {
	ptmin[0] = ptmin[1] = ptmin[2] = std::numeric_limits<double>::max();
	ptmax[0] = ptmax[1] = ptmax[2] = -std::numeric_limits<double>::max();

	//遍历所有面
	for (DefaultTriMesh::FaceIter f_it = m_mesh.faces_begin(); f_it != m_mesh.faces_end(); ++f_it)
	{
		DefaultTriMesh::FaceHandle fh = *f_it;
		DefaultTriMesh::FaceVertexIter fvi;
		fvi = m_mesh.fv_begin(fh);

		while (fvi != m_mesh.fv_end(fh)) {//遍历每个面的顶点
			auto vh = *fvi;//获取数据
			if (vh.is_valid()) {
				auto v = m_mesh.point(vh);
				ptmin[0] = std::min((double)ptmin[0], (double)v[0]);
				ptmin[1] = std::min((double)ptmin[1], (double)v[1]);
				ptmin[2] = std::min((double)ptmin[2], (double)v[2]);
				ptmax[0] = std::max((double)ptmax[0], (double)v[0]);
				ptmax[1] = std::max((double)ptmax[1], (double)v[1]);
				ptmax[2] = std::max((double)ptmax[2], (double)v[2]);
			}
			else {
				std::cout << "vh   not  valid " << std::endl;
			}
			fvi++;
		}
	}
}