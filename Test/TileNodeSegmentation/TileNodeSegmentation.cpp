
#include "TileNodeSegmentation.h"


#include <corecrt_io.h>

#ifndef F_OK
#define F_OK 4
#endif
#define OUTPUT_OSG_FILE//TODO:没多大用，可关闭

void TileNodeSegmentation::main_process(std::string input_file_name, std::string output_dir) {

	int tree_level = 2;
	double level_reserving_ratio = 0.8;

	//TODO:输入参数的读取
	cfg_init();

	//mesh读取
	//m_mesh_base.read_obj_file(input_file_name);

	preProcess_filePath(input_file_name, output_dir);//复制文件，修改输入文件名
	node_segmentation(input_file_name, output_dir, tree_level);
	node_reserving_ratio_computation(level_reserving_ratio);//
	node_reduction_execution();//压缩过程

	//TODO:四叉树对中间过程额外层的兼容
	//todo:3.4.0版本
	//todo:四个接口的问题
	create_lod();//四叉树建立LOD过程



}
std::vector<std::string> GetFileList(const std::string& path) {
	std::vector<std::string> file_list;
	for (auto it = boost::filesystem::directory_iterator(path);
		it != boost::filesystem::directory_iterator(); ++it) {
		if (boost::filesystem::is_regular_file(*it)) {
			const boost::filesystem::path file_path = *it;
			file_list.push_back(file_path.string());
		}
	}
	return file_list;
}
int TileNodeSegmentation::preProcess_filePath(std::string& input_file_name, std::string& output_path) {
	//复制input_file到目标路径，并且修改input_file_name

	std::string input_stem;
	std::string input_ext;
	std::string input_parent_path;
	std::string absolute_path;
	std::string file_path;
	std::filesystem::path input_file(input_file_name);

	if (input_file.has_stem()) {
		input_stem = input_file.stem().string();
	}
	if (input_file.has_extension()) {
		input_ext = input_file.extension().string();
	}
	if (input_file.has_parent_path()) {
		input_parent_path = input_file.parent_path().string();
	}


	if (!boost::filesystem::exists(output_path)) {
		boost::filesystem::create_directories(output_path);
	}

	std::vector<std::string> copy_file_list = GetFileList(input_parent_path);
	for (int copy_i = 0; copy_i < copy_file_list.size(); copy_i++) {
		std::string curr_file_name = copy_file_list[copy_i];
		std::string curr_stem;
		std::string curr_ext;
		std::string curr_parent_path;
		std::filesystem::path curr_file(curr_file_name);

		if (curr_file.has_stem()) {
			curr_stem = curr_file.stem().string();
		}
		if (curr_file.has_extension()) {
			curr_ext = curr_file.extension().string();
		}
		if (curr_file.has_parent_path()) {
			curr_parent_path = curr_file.parent_path().string();
		}

		std::string copy_to_filename = output_path + curr_stem + curr_ext;//copy_to_dir + copy_file_list[copy_i].substr((copy_file_list[copy_i].find_last_of(PATH_SEPARATOR) + 1));
		try {
			boost::filesystem::copy_file(curr_file_name, copy_to_filename, boost::filesystem::copy_option::overwrite_if_exists);
		}
		catch (boost::filesystem::filesystem_error e) {
			std::cout << e.what() << std::endl;
			std::cout << e.path1() << std::endl;
			return -1;
		}
	}
	
	input_file_name = output_path + input_stem + input_ext;

	return 0;
}

int TileNodeSegmentation::node_segmentation(std::string input_file_name, std::string output_dir, int segment_level) {

	m_total_lod_level = segment_level;
	m_total_lod_level_with_geode = m_total_lod_level + 1;
	quad_tree_node_data root_node_data;
	root_node_data.file_name_obj = input_file_name;
	m_tree->createRoot(root_node_data);

	TreeNode<quad_tree_node_data>* rootNodePtr = m_tree->getRootPtr();

	if (nullptr == rootNodePtr || m_total_lod_level_with_geode <= 0) {
		return -1;
	}

	std::queue<std::pair<TreeNode<quad_tree_node_data>*, int>> node_to_process;
	node_to_process.push(std::make_pair(rootNodePtr, m_total_lod_level_with_geode));

	while (!node_to_process.empty()) {
		auto curr_data = node_to_process.front();
		TreeNode<quad_tree_node_data>* curr_node = curr_data.first;
		int level = curr_data.second;
		node_to_process.pop();
		//std::string pathExport = "F:/handwork_data/data_out/27_MJSJHY/";//todo:暴露接口，或者在树的节点内储存信息

		Segmenter segmenter;
		std::unordered_map<std::string, std::vector<double>> res = segmenter.segmentMeshWithSpatialBBox_new(curr_node->data.file_name_obj, output_dir);//上一次的文件名
		
		for (auto it = res.begin(); it != res.end(); it++) {
			//for (int i = 0; i < res.size(); ++i) {
			quad_tree_node_data new_node;
			new_node.file_name_obj = it->first;

			TreeNode<quad_tree_node_data>* node_inserted = curr_node->addChild(new_node);
			node_inserted->data.level = curr_node->data.level + 1;//更新层数
			//node_inserted->data.current_reserving_ratio = new_reserving_ratio;//更新压缩比、、TODO:单独一个循环执行
			if (level > 1)
				node_to_process.push(std::make_pair(node_inserted, level - 1));
		}

	}
	return 0;
	/*
	* bak
	auto segment = [](std::string& str) {
		std::vector<std::string> res;
		for (int i = 0; i < 4; ++i) {
			res.push_back(str + std::to_string(i));
		}
		return res;
	};
	std::vector<std::string> res = segment(curr_node->data);
	*/
}

int TileNodeSegmentation::node_reserving_ratio_computation(double level_reserving_ratio) {
	TreeNode<quad_tree_node_data>* rootNodePtr = m_tree->getRootPtr();
	if (nullptr == rootNodePtr) {
		return -1;
	}

	std::queue<TreeNode<quad_tree_node_data>*> node_to_process;
	node_to_process.push(rootNodePtr);

	while (!node_to_process.empty()) {
		auto curr_data = node_to_process.front();
		TreeNode<quad_tree_node_data>* curr_node = curr_data;
		curr_node->data.current_reserving_ratio=pow(level_reserving_ratio, m_total_lod_level_with_geode -curr_node->data.level);//赋值
		node_to_process.pop();

		std::vector< TreeNode<quad_tree_node_data>* > res = curr_node->getChild();
		for (auto it = res.begin(); it != res.end(); it++) {
			node_to_process.push(*it);
		}
	}
	return 0;
}
int TileNodeSegmentation::create_lod() {
	TreeNode<quad_tree_node_data>* rootNodePtr = m_tree->getRootPtr();
	if (nullptr == rootNodePtr) {
		return -1;
	}

	std::queue<TreeNode<quad_tree_node_data>*> node_to_process;
	node_to_process.push(rootNodePtr);

	while (!node_to_process.empty()) {
		auto curr_data = node_to_process.front();
		TreeNode<quad_tree_node_data>* curr_node = curr_data;

		//拿出数据，建立LOD
		std::string father_obj_file = curr_node->data.file_name_obj;
		std::vector<std::string> child_obj_files;// = curr_node->getChild();

		std::vector< TreeNode<quad_tree_node_data>* > res = curr_node->getChild();
		bool is_leaf = false;//假设同层同结构
		for (auto it = res.begin(); it != res.end(); it++) {
			std::vector<std::string> child_obj_file;// = curr_node->getChild();
			child_obj_files.push_back((*it)->data.file_name_obj);
			node_to_process.push(*it);//入队
			is_leaf = (*it)->data.level >= m_total_lod_level_with_geode;
		}
		create_group_pagelod_2(father_obj_file, child_obj_files, is_leaf,1.0);
		//create_group_pagelod_(father_obj_file,child_obj_files,1.0);

		node_to_process.pop();

	}
	return 0;
}
int TileNodeSegmentation::node_reduction_execution() {
	TreeNode<quad_tree_node_data>* rootNodePtr = m_tree->getRootPtr();
	if (nullptr == rootNodePtr) {
		return -1;
	}

	std::queue<TreeNode<quad_tree_node_data>*> node_to_process;
	node_to_process.push(rootNodePtr);

	while (!node_to_process.empty()) {
		auto curr_data = node_to_process.front();
		TreeNode<quad_tree_node_data>* curr_node = curr_data;
		Simplifier sim;
		sim.simplify_obj(curr_node->data.file_name_obj, curr_node->data.file_name_obj, curr_node->data.current_reserving_ratio);
		node_to_process.pop();

		std::vector< TreeNode<quad_tree_node_data>* > res = curr_node->getChild();
		for (auto it = res.begin(); it != res.end(); it++) {
			node_to_process.push(*it);
		}
	}
	return 0;
}
void TileNodeSegmentation::cfg_init() {
	m_mesh_base = new MeshBase;
	new GenericTree<quad_tree_node_data>();
	m_tree = new QuadTree;
}

int TileNodeSegmentation::test_osg_read(const std::string& file_name) {
	osg::ref_ptr<osgDB::ReaderWriter::Options> read_options = new osgDB::ReaderWriter::Options;
	read_options->setOptionString("noRotation");

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(file_name, read_options);//todo:废代码太多，这个没必要读取两次  child_osg_node ==  node
	osg::Geode* geode = node->asGroup()->getChild(0)->asGeode();
	//改变geode名字
	geode->setName("");
	//geode->getOrCreateStateSet();
	osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();
	geometry->setDataVariance(osg::Object::STATIC);
	osg::StateSet* stateset = geometry->getStateSet()->asStateSet();
	stateset->setDataVariance(osg::Object::STATIC);
	osg::StateAttribute* Material = stateset->getAttribute(osg::StateAttribute::Type::MATERIAL, 0)->asStateAttribute();
	//改变材质名字
	std::string aa = Material->getName();
	Material->setName("");
	osg::StateAttribute* texture2D = stateset->getTextureAttribute(0, osg::StateAttribute::Type::TEXTURE)->asStateAttribute();
	osg::Image* osg_image = texture2D->asTexture()->getImage(0);
	return 0;

}
int TileNodeSegmentation::create_group_pagelod_(const std::string& father_node, const std::vector<std::string>& child_nodes, double rangelist_ratio) {
	//输入：目标输出：父节点osgb名，输入：合并子节点obj
	//输出：L17.osgb。输入：L18.obj。输入链接L18.osgb
	//预处理获得：
	//输入obj名，链接osgb名，输出osgb名
	if (child_nodes.size()<1)
		return -1;
	std::filesystem::path father_obj_name(father_node);
	std::string father_stem;
	std::string father_parent_path;
	if (father_obj_name.has_stem()) {
		father_stem = father_obj_name.stem().string();
	}

	if (father_obj_name.has_parent_path()) {
		father_parent_path = father_obj_name.parent_path().string();
	}
	father_parent_path += "/";




	osg::ref_ptr<osg::Group> group = new osg::Group();
	osg::ref_ptr<osgDB::ReaderWriter::Options> read_options = new osgDB::ReaderWriter::Options;
	read_options->setOptionString("noRotation");
	for (int index = 0; index < child_nodes.size(); index++) {
		//更改
		std::string child_obj= child_nodes[index];
		std::filesystem::path child_obj_name(child_obj);
		std::string child_stem;
		std::string child_parent_path;

		if (child_obj_name.has_stem()) {
			child_stem = child_obj_name.stem().string();
		}

		if (child_obj_name.has_parent_path()) {
			child_parent_path = child_obj_name.parent_path().string();
		}
		child_parent_path += "/";


		//node本节点
		if (access(child_obj.c_str(), F_OK) == 0) {
		}
		else {
			std::cout << "no file" << child_obj << std::endl;
			continue;
		}
		osg::ref_ptr<osg::PagedLOD> lod = new osg::PagedLOD();
		lod->setDatabasePath("");
		//child_node孩子节点
		//std::string child_osgb = child_parent_path + child_stem + ".osgb";//孩子节点
		std::string child_osgb = child_stem + ".osgb";//孩子节点

		osg::ref_ptr<osg::Node> child_osg_node = osgDB::readNodeFile(child_obj, read_options);
		auto bs = child_osg_node->getBound();
		auto c = bs.center();
		auto r = bs.radius();
		lod->setCenter(c);
		lod->setRadius(r);
		lod->setRangeMode(osg::LOD::RangeMode::PIXEL_SIZE_ON_SCREEN);
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(child_obj, read_options);//todo:废代码太多，这个没必要读取两次  child_osg_node ==  node
		osg::Geode* geode = node->asGroup()->getChild(0)->asGeode();
		//改变geode名字
		geode->setName("");
		//geode->getOrCreateStateSet();
		osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();
		geometry->setDataVariance(osg::Object::STATIC);
		osg::StateSet* stateset = geometry->getStateSet()->asStateSet();
		osg::StateSet* stateset2 = geometry->getStateSet();
		stateset->setDataVariance(osg::Object::STATIC);
		osg::StateAttribute* Material = stateset->getAttribute(osg::StateAttribute::Type::MATERIAL, 0)->asStateAttribute();
		//改变材质名字
		std::string aa = Material->getName();
		Material->setName("");
		osg::StateAttribute* texture2D = stateset->getTextureAttribute(0, osg::StateAttribute::Type::TEXTURE)->asStateAttribute();
		osg::Image* osg_image = texture2D->asTexture()->getImage(0);
		//改变纹理名字
		const std::string image_file_name = osg_image->getFileName();
		const std::string new_image_file_name = osgDB::getSimpleFileName(image_file_name);
		osg_image->setFileName(new_image_file_name);
		lod->addChild(geode);
		std::string relativeFilePath = child_stem + ".osgb";  //相对路径,相对于保存的路径//孩子节点的osgb，相对路径
		lod->setFileName(0, "");
		lod->setFileName(1, relativeFilePath);


		float range_val = r / rangelist_ratio;
		lod->setRange(0, 0, range_val);																							//第一层不可见
		lod->setRange(1, range_val, FLT_MAX);
		group->addChild(lod);
	}

	DelNormalsGeometryVisitor dngv;
	group->accept(dngv);

	osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
	options->setOptionString("Compressor=zlib");          // 设置压缩
	options->setOptionString("WriteImageHint=IncludeFile");
	std::string outputLodFile2 = father_parent_path+father_stem + ".osgb";
	osgDB::writeNodeFile(*group, outputLodFile2, options);
	//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OUTPUT_OSG_FILE
	std::string outputLodFile = father_parent_path + father_stem + ".osg";
	osgDB::writeNodeFile(*group, outputLodFile);
#endif
	return 0;
}

int TileNodeSegmentation::create_group_pagelod_2(const std::string& father_node, const std::vector<std::string>& child_nodes, bool is_leaf, double rangelist_ratio) {
	//输入：目标输出：父节点osgb名，输入：合并子节点obj
	//输出：L17.osgb。输入：L18.obj。输入链接L18.osgb
	//预处理获得：
	//输入obj名，链接osgb名，输出osgb名
	bool has_child = true;//如果有孩子，lod，如果没有，geode
	if (child_nodes.size() < 1)
		return -1;
	osg::ref_ptr<osg::Group> group = nullptr;
	if (child_nodes.size() > 1) {
		group = new osg::Group();
	}

	std::filesystem::path father_obj_name(father_node);
	std::string father_stem;
	std::string father_parent_path;
	if (father_obj_name.has_stem()) {
		father_stem = father_obj_name.stem().string();
	}

	if (father_obj_name.has_parent_path()) {
		father_parent_path = father_obj_name.parent_path().string();
	}
	father_parent_path += "/";



	osg::ref_ptr<osgDB::ReaderWriter::Options> read_options = new osgDB::ReaderWriter::Options;
	read_options->setOptionString("noRotation");
	osg::ref_ptr<osg::PagedLOD> lod;
	for (int index = 0; index < child_nodes.size(); index++) {
		//更改
		std::string child_obj = child_nodes[index];
		std::filesystem::path child_obj_name(child_obj);
		std::string child_stem;
		std::string child_parent_path;

		if (child_obj_name.has_stem()) {
			child_stem = child_obj_name.stem().string();
		}

		if (child_obj_name.has_parent_path()) {
			child_parent_path = child_obj_name.parent_path().string();
		}
		child_parent_path += "/";


		//node本节点
		if (access(child_obj.c_str(), F_OK) == 0) {
		}
		else {
			std::cout << "no file" << child_obj << std::endl;
			continue;
		}
		lod = new osg::PagedLOD();
		lod->setDatabasePath("");
		//child_node孩子节点
		//std::string child_osgb = child_parent_path + child_stem + ".osgb";//孩子节点
		std::string child_osgb = child_stem + ".osgb";//孩子节点

		osg::ref_ptr<osg::Node> child_osg_node = osgDB::readNodeFile(child_obj, read_options);
		//osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(child_obj, read_options);//todo:废代码太多，这个没必要读取两次  child_osg_node ==  node
		auto bs = child_osg_node->getBound();
		auto c = bs.center();
		auto r = bs.radius();
		lod->setCenter(c);
		lod->setRadius(r);
		lod->setRangeMode(osg::LOD::RangeMode::PIXEL_SIZE_ON_SCREEN);
		osg::Geode* geode = child_osg_node->asGroup()->getChild(0)->asGeode();
		//改变geode名字
		geode->setName("");
		//geode->getOrCreateStateSet();
		osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();
		geometry->setDataVariance(osg::Object::STATIC);
		osg::StateSet* stateset = geometry->getStateSet()->asStateSet();
		osg::StateSet* stateset2 = geometry->getStateSet();
		stateset->setDataVariance(osg::Object::STATIC);
		osg::StateAttribute* Material = stateset->getAttribute(osg::StateAttribute::Type::MATERIAL, 0)->asStateAttribute();
		//改变材质名字
		std::string aa = Material->getName();
		Material->setName("");
		osg::StateAttribute* texture2D = stateset->getTextureAttribute(0, osg::StateAttribute::Type::TEXTURE)->asStateAttribute();
		osg::Image* osg_image = texture2D->asTexture()->getImage(0);
		//改变纹理名字
		const std::string image_file_name = osg_image->getFileName();
		const std::string new_image_file_name = osgDB::getSimpleFileName(image_file_name);
		osg_image->setFileName(new_image_file_name);
		lod->addChild(geode);
		std::string relativeFilePath = child_stem + ".osgb";  //相对路径,相对于保存的路径//孩子节点的osgb，相对路径
		lod->setFileName(0, "");
		lod->setFileName(1, relativeFilePath);


		float range_val = r / rangelist_ratio;
		lod->setRange(0, 0, range_val);																							//第一层不可见
		lod->setRange(1, range_val, FLT_MAX);
		if (group) {
			if (!is_leaf)
				group->addChild(lod);
			else
				group->addChild(geode);
		}
		
	}

	auto node_to_save = group?group:lod;
	DelNormalsGeometryVisitor dngv;
	node_to_save->accept(dngv);

	osg::ref_ptr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
	options->setOptionString("Compressor=zlib");          // 设置压缩
	options->setOptionString("WriteImageHint=IncludeFile");
	std::string outputLodFile2 = father_parent_path + father_stem + ".osgb";
	osgDB::writeNodeFile(*node_to_save, outputLodFile2, options);

	//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OUTPUT_OSG_FILE
	std::string outputLodFile = father_parent_path + father_stem + ".osg";
	osgDB::writeNodeFile(*node_to_save, outputLodFile);
#endif
	return 0;
}

/*
void TileNodeSegmentation::node_segment_qy() {
	std::string file_read = "F:/handwork_data/data/27_MJSJHY/MJSJHY.obj";
	std::string path_save = "F:/handwork_data/data_out/27_MJSJHY/";
	std::string subTag = "MJSJHY";

	int col = 1, row = 1, tens = 1;//分一叉树四叉树

	bool is_quadtree = false;
	if (is_quadtree) {
		col = 2;
		row = 2;//分一叉树四叉树

	}
	OpenMesh::Vec3i crts(col, row, tens);


	//TODO:待实现
	OpenMesh::Vec3d ptmin; // GetBoundingBox(meshData)[0];
	OpenMesh::Vec3d ptmax;// = GetBoundingBox(meshData)[1];

	std::unordered_map<std::string, std::vector<double>> result_name;

	segParamSpatial param(subTag, crts, OpenMesh::Vec3d(ptmin[0], ptmin[1], ptmin[2]), OpenMesh::Vec3d(ptmax[0], ptmax[1], ptmax[2]));
	result_name = segmentMeshWithSpatialBBox(file_read, path_save, param);

	return result_name;

}
*/
/*
std::unordered_map<std::string, std::vector<double>>  TileNodeSegmentation::segmentMeshWithSpatialBBox(const std::string& fileImport, const std::string& pathExport, const segParamSpatial& param) {

	// pre check
	std::unordered_map<std::string, std::vector<double>> result_name;
	if (param.crts[0] * param.crts[1] * param.crts[2] < 1) {
		return result_name;
	}

	if (!boost::filesystem::exists(fileImport))
		return result_name;

	// variables 
	double xDimension = (param.bBoxPtMax[0] - param.bBoxPtMin[0]) / param.crts[0];
	double yDimension = (param.bBoxPtMax[1] - param.bBoxPtMin[1]) / param.crts[1];
	double zDimension = (param.bBoxPtMax[2] - param.bBoxPtMin[2]) / param.crts[2];

	const double overlpReserved = 0.00f;
	const OpenMesh::Vec3d ptMinShiftReaerved = OpenMesh::Vec3d(-overlpReserved, -overlpReserved, -overlpReserved - 0.00001f);
	const OpenMesh::Vec3d ptMaxShiftReaerved = OpenMesh::Vec3d(overlpReserved, overlpReserved, overlpReserved + 0.00001f);

	std::string temp = fileImport;
	std::string file_base_name = "MJSJHY";//TODO：完整obj提取base name    Util::getFileName(temp)
	std::string file_ext_name = ".obj";//Util::getFileExt(temp).ToLower()

	temp = "F:/handwork_data/data/27_MJSJHY/";// Util::getFilePath(temp);
	//Util::ensureFolderSlash(temp);
	std::string path = temp;

	const int nCubes = param.crts[0] * param.crts[1] * param.crts[2];

	// 1.  genereate cube 
	for (int i = 0; i < nCubes; i++)
	{
		
		std::string strId = "";
		if (nCubes > 1) {
			strId = std::to_string(i);
		}

		int idx = i % (param.crts[0]);
		int idy = i / (param.crts[0]) % (param.crts[0]);
		int idz = i / (param.crts[0] * param.crts[1]);

		if (zDimension == 0 && idz > 0) {

			std::cout << "!!! empty cube in pos: [ " << idx << "  " << idy << "  " << idz << " ]" << std::endl;
			continue;
		}


		// calculate current cube border offset corresponding to the orignal mesh
		double x = 1.0f * xDimension * idx + ptMinShiftReaerved[0];
		double y = 1.0f * yDimension * idy + ptMinShiftReaerved[1];
		double z = 1.0f * zDimension * idz + ptMinShiftReaerved[2];
		OpenMesh::Vec3d bBoxMinOffset(x, y, z);

		x = 1.0f * xDimension * (idx - param.crts[0] + 1) + ptMaxShiftReaerved[0];
		y = 1.0f * yDimension * (idy - param.crts[1] + 1) + ptMaxShiftReaerved[1];
		z = 1.0f * zDimension * (idz - param.crts[2] + 1) + ptMaxShiftReaerved[2];
		OpenMesh::Vec3d bBoxMaxOffset(x, y, z);

		if (bBoxMinOffset[2] == bBoxMaxOffset[2]) {
			bBoxMinOffset[2] -= 0.0001f;
			bBoxMaxOffset[2] += 0.0001f;
		}

		std::string save_name = pathExport + param.subTag + strId + ".obj";

		printf("cube name: %s  cube position: [ %d %d %d ]  ", save_name.c_str(), idx, idy, idz);

		OpenMesh::Vec3d bBoxMin, bBoxMax;
		std::string mtlSharedName = "seg_shared.mtl";
		std::string mtlSharedPath = pathExport + mtlSharedName;
		if (segmentMeshWithBBox(fileImport, bBoxMinOffset, bBoxMaxOffset, bBoxMin, bBoxMax, save_name, mtlSharedName)) {
			std::vector<double> w_h_t = { bBoxMax[0] - bBoxMin[0],bBoxMax[1] - bBoxMin[1],bBoxMax[2] - bBoxMin[2] };
			result_name.insert(std::pair<std::string, std::vector<double>>(save_name, w_h_t));
			printf(" ok \n");
		}
		else {
			printf(" empty \n");
			continue;
		}

		// 2. create shared mtl,textures
		{
			// mtl
			if (!boost::filesystem::exists(mtlSharedPath)) {
				std::string src = path + file_base_name + ".mtl";

				boost::filesystem::copy_file(src, mtlSharedPath);
				std::string texturesDir = "tempTextures";
				std::set<std::string> textures = updateMtlRelativePath(mtlSharedPath, texturesDir);


				// textures
				for (auto filename : textures) {

					std::string src = path + filename;
					std::string dst = pathExport + texturesDir + "/" + filename;

					boost::filesystem::create_directory(pathExport + texturesDir);
					boost::filesystem::copy_file(src, dst);
				}

			}
		}
	}

	return result_name;
}
*/
/*
bool TileNodeSegmentation::segmentMeshWithBBox(const std::string& fileImport, const OpenMesh::Vec3d& bBoxMinOffset, const OpenMesh::Vec3d& bBoxMaxOffset, OpenMesh::Vec3d& bBoxMin, OpenMesh::Vec3d& bBoxMax, const std::string& fileExport, const std::string& mtlName) {


	// obj path valid?
	if (!boost::filesystem::exists(fileImport.c_str()))
		return false;

	// mesh path
	std::string pathRead = fileImport;
	std::string pathSave = fileImport;//  boost::filesystem::path(path).parent_path().string() + "/" + boost::filesystem::basename(path) + "_segment" + ".obj";
	if (!fileExport.empty()) {
		pathSave = fileExport;
	}


	////////////////////////////////////////////////////////////////
	//TODO:考虑一下类的关系，直接从mesh类拿数据

	//mappingSegmentMeshTexture是否有必要，又是否足够正确

	//先确定下边到底有几步，再确定哪一步是必须的，mtl映射如果不要~！

	// 1. load mesh
	OBJData meshRead;
	if (!LoadObj(pathRead, meshRead))
		return false;

	// get face
	std::vector<Point3i> faces;
	for (auto group : meshRead.faceGroups) {

		for (auto f : group.faces) {
			faces.push_back(Point3i(f.verticesId[0], f.verticesId[1], f.verticesId[2]));
		}
	}

	// get bounding box
	Point3d bbBoxMin = meshRead.vertices[0];
	Point3d bbBoxMax = bbBoxMin;
	for (auto val : meshRead.vertices) {

		bbBoxMin[0] = std::min(bbBoxMin[0], val[0]);
		bbBoxMin[1] = std::min(bbBoxMin[1], val[1]);
		bbBoxMin[2] = std::min(bbBoxMin[2], val[2]);
		bbBoxMax[0] = std::max(bbBoxMax[0], val[0]);
		bbBoxMax[1] = std::max(bbBoxMax[1], val[1]);
		bbBoxMax[2] = std::max(bbBoxMax[2], val[2]);
	}


	MeshCutterParams ioParams;
	ioParams.bbMin = bbBoxMin + bBoxMinOffset;
	ioParams.bbMax = bbBoxMax + bBoxMaxOffset;

	// 2. cut mesh
	ioParams.trackOrigIndexes = true;
	// mesh segmention
	if (!segmentMeshWithBBox(faces, meshRead.vertices, ioParams))
		return false;

	// 3.1 mapping texture
	if (meshRead.hasTexture) {
		mappingSegmentMeshTexture(meshRead, faces, ioParams);
	}

	// 3.2 mapping normal
	if (meshRead.hasNormal) {
		mappingSegmentMeshNormal(meshRead, faces, ioParams);
	}

	// 4. save mesh data
	OBJData meshSave;
	meshSave.mtlLib = meshRead.mtlLib;
	if (!mtlName.empty()) {
		meshSave.mtlLib = mtlName;
	}

	meshSave.vertices = ioParams.getSegmentVertices();
	meshSave.faceGroups.resize(meshRead.faceGroups.size());
	meshSave.hasTexture = meshRead.hasTexture;
	meshSave.hasNormal = meshRead.hasNormal;
	for (int i = 0; i < meshSave.faceGroups.size(); i++)
		meshSave.faceGroups[i].usemtl = meshRead.faceGroups[i].usemtl;

	// 4.1 unique uv coordinatess
	std::vector<int> offsetVal;
	offsetVal.resize(ioParams.mappingTexCorrds.size());
	if (meshSave.hasTexture) {
		int offset_total = 0;
		std::set<std::string> uvKeys;
		std::map<std::string, int> vecKeys;

		for (int i = 0; i < ioParams.mappingTexCorrds.size(); i++) {
			std::string str = std::to_string(ioParams.mappingTexCorrds[i][0]) + "#" + std::to_string(ioParams.mappingTexCorrds[i][1]);
			if (!uvKeys.insert(str).second) {

				auto it = vecKeys.find(str);
				if (it != vecKeys.end()) {

					offset_total -= 1;
					int pos = it->second;
					offsetVal[i] = pos - i;
				}
			}
			else {
				vecKeys.insert(std::pair<std::string, int>(str, vecKeys.size()));
				meshSave.texCoords.push_back(ioParams.mappingTexCorrds[i]);
				offsetVal[i] = offset_total;
			}
		}
	}


	// 4.2 unique normal
	std::vector<int> normalOffset;
	normalOffset.resize(ioParams.mappingNormals.size());
	if (meshSave.hasNormal) {
		int offset_total = 0;
		std::set<std::string> uvKeys;
		std::map<std::string, int> vecKeys;

		for (int i = 0; i < ioParams.mappingNormals.size(); i++) {
			std::string str = std::to_string(ioParams.mappingNormals[i][0]) + "#" + std::to_string(ioParams.mappingNormals[i][1]) + "#" + std::to_string(ioParams.mappingNormals[i][2]);
			if (!uvKeys.insert(str).second) {

				auto it = vecKeys.find(str);
				if (it != vecKeys.end()) {

					offset_total -= 1;
					int pos = it->second;
					normalOffset[i] = pos - i;
				}
			}
			else {
				vecKeys.insert(std::pair<std::string, int>(str, vecKeys.size()));
				meshSave.normals.push_back(ioParams.mappingNormals[i]);
				normalOffset[i] = offset_total;
			}
		}
	}


	// 4.3  add face group
	faces = ioParams.getSegmentFaces();
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned origFaceId = ioParams.getOrigTriIndexes(i);
		unsigned int groupId = meshRead.getGroupId(origFaceId);

		Face f;
		memset(&f, 0xFF, sizeof(Face));

		//v
		f.verticesId = faces[i];

		// vn
		if (meshSave.hasNormal) {
			const Point3i& normalId = ioParams.mappingFaceNormId[i];
			f.normalsId = Point3i(normalId[0] + normalOffset[normalId[0]], normalId[1] + normalOffset[normalId[1]], normalId[2] + normalOffset[normalId[2]]);
		}

		// vt
		if (meshSave.hasTexture) {

			const Point3i& texId = ioParams.mappingFaceTexId[i];
			f.texCoordsId = Point3i(texId[0] + offsetVal[texId[0]], texId[1] + offsetVal[texId[1]], texId[2] + offsetVal[texId[2]]);
		}

		meshSave.faceGroups[groupId].faces.push_back(f);
	}


	// 5. remove old file
	if (fileExport.empty()) {
		boost::filesystem::remove(pathRead);
	}

	// 6. save mesh data
	SaveObj(pathSave, meshSave);

	// 7. set bBox
	bBoxMin = meshSave.vertices[0];
	bBoxMax = bBoxMin;
	for (auto val : meshSave.vertices) {
		bBoxMin[0] = std::min(bBoxMin[0], val[0]);
		bBoxMin[1] = std::min(bBoxMin[1], val[1]);
		bBoxMin[2] = std::min(bBoxMin[2], val[2]);
		bBoxMax[0] = std::max(bBoxMax[0], val[0]);
		bBoxMax[1] = std::max(bBoxMax[1], val[1]);
		bBoxMax[2] = std::max(bBoxMax[2], val[2]);
	}

	return true;
}
*/