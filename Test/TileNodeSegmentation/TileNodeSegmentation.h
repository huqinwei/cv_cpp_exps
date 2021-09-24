#ifndef _NODE_SEGMENTATION_
#define _NODE_SEGMENTATION_

#include "MeshBase/GenericTree.h"
#include "MeshBase/Segmenter.h"
#include "MeshBase/MeshBase.h"
#include "Simplifier/Simplifier.h"
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <osg/Group>
#include <osg/Notify>
#include <osg/Geometry>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/PagedLOD>

//#include <osgViewer/Viewer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>

#include <osgUtil/Optimizer>
//#include "Util.h"

class TileNodeSegmentation {
public:
	TileNodeSegmentation() {}

public:

	//void node_segment_qy();

	void cfg_init();

	//对外接口
	void main_process(std::string input_dir, std::string output_dir);
	//预处理，复制文件等
	int preProcess_filePath(std::string& input_file_name, std::string& output_path);
	//切割保存四叉树
	int node_segmentation(std::string input_file_name, std::string pathEoutput_dirxport, int segment_level);
	//四叉树压缩比例计算
	int node_reserving_ratio_computation(double level_reserving_ratio);
	//四叉树压缩执行
	int node_reduction_execution();
	////todo:一个遍历树节点赋值rangelist_ratio的接口//double rangelist_ratio = 1.0;
	int rangelist_ratio_computation();

	//遍历、建立LOD
	int create_lod();
	//单元接口
	//旧参数列表：std::string filename, std::int32_t flag, std::vector <std::vector<std::string>> farther_child, std::vector<float_t> rangelist_ratio, std::int32_t merge_flag, bool hand_work_mode, float transmit_range_val, int check_flag
	//需要未卜先知，设计尾部的链接
	//原版LOD过程问题：只有一个L17.osgb，要用4个L17的obj
	int create_group_pagelod_(const std::string& father_node, const std::vector<std::string>& child_nodes, double rangelist_ratio);
	int create_group_pagelod_2(const std::string& father_node, const std::vector<std::string>& child_nodes,bool is_leaf, double rangelist_ratio);
	int test_osg_read(const std::string& file_name);

	void cut_node();

	//std::unordered_map<std::string, std::vector<double>> segmentMeshWithSpatialBBox(const std::string& fileImport, const std::string& pathExport, const segParamSpatial& param);


private:

	//MeshBase* m_mesh;
	QuadTree* m_tree;
	//GenericTree<tree_meta_node>tree m_tree;
	double m_leaf_resolution = 0.1;//叶子节点分辨率
	int m_total_lod_level;				//层级//real_level//逻辑层数:设置2，则最终产出0、1、2三层
	int m_total_lod_level_with_geode;	//fake_level=real_level+1//中间过程依赖
	
	std::vector<double> rangelist_ratio;
	MeshBase* m_mesh_base;
	//Segmentator* m_segmenter;
	//m_segmenter->segmentMeshWithSpatialBBox();//这个接口的文件名参数可以取消，改为从mesh拿，但是过程有些用到的逻辑需要拆解出来


};






class DelNormalsGeometryVisitor : public osg::NodeVisitor
{
public:

	DelNormalsGeometryVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {
	}

	virtual void apply(osg::Geode& geode)
	{
		for (unsigned int ii = 0; ii < geode.getNumDrawables(); ++ii)
		{
			osg::ref_ptr< osg::Geometry > geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(ii));
			if (geometry.valid())
			{
				osg::ref_ptr< osg::Vec3Array > newnormals = NULL;
				geometry->setNormalArray(newnormals.get(), osg::Array::BIND_UNDEFINED);
			}
		}
	}

	virtual void apply(osg::Node& node)
	{
		traverse(node);
	}
};






#endif