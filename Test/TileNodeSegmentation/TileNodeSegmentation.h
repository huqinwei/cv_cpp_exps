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

	//����ӿ�
	void main_process(std::string input_dir, std::string output_dir);
	//Ԥ���������ļ���
	int preProcess_filePath(std::string& input_file_name, std::string& output_path);
	//�и���Ĳ���
	int node_segmentation(std::string input_file_name, std::string pathEoutput_dirxport, int segment_level);
	//�Ĳ���ѹ����������
	int node_reserving_ratio_computation(double level_reserving_ratio);
	//�Ĳ���ѹ��ִ��
	int node_reduction_execution();
	////todo:һ���������ڵ㸳ֵrangelist_ratio�Ľӿ�//double rangelist_ratio = 1.0;
	int rangelist_ratio_computation();

	//����������LOD
	int create_lod();
	//��Ԫ�ӿ�
	//�ɲ����б�std::string filename, std::int32_t flag, std::vector <std::vector<std::string>> farther_child, std::vector<float_t> rangelist_ratio, std::int32_t merge_flag, bool hand_work_mode, float transmit_range_val, int check_flag
	//��Ҫδ����֪�����β��������
	//ԭ��LOD�������⣺ֻ��һ��L17.osgb��Ҫ��4��L17��obj
	int create_group_pagelod_(const std::string& father_node, const std::vector<std::string>& child_nodes, double rangelist_ratio);
	int create_group_pagelod_2(const std::string& father_node, const std::vector<std::string>& child_nodes,bool is_leaf, double rangelist_ratio);
	int test_osg_read(const std::string& file_name);

	void cut_node();

	//std::unordered_map<std::string, std::vector<double>> segmentMeshWithSpatialBBox(const std::string& fileImport, const std::string& pathExport, const segParamSpatial& param);


private:

	//MeshBase* m_mesh;
	QuadTree* m_tree;
	//GenericTree<tree_meta_node>tree m_tree;
	double m_leaf_resolution = 0.1;//Ҷ�ӽڵ�ֱ���
	int m_total_lod_level;				//�㼶//real_level//�߼�����:����2�������ղ���0��1��2����
	int m_total_lod_level_with_geode;	//fake_level=real_level+1//�м��������
	
	std::vector<double> rangelist_ratio;
	MeshBase* m_mesh_base;
	//Segmentator* m_segmenter;
	//m_segmenter->segmentMeshWithSpatialBBox();//����ӿڵ��ļ�����������ȡ������Ϊ��mesh�ã����ǹ�����Щ�õ����߼���Ҫ������


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