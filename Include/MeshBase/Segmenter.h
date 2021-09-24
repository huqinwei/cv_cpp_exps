//##########################################################################
//#                      mesh segmentiopn with BBox                        #
//#						  tag=1725 date=2021-05-19                         #
//##########################################################################

#ifndef _SEGMENTER_H_
#define _SEGMENTER_H_

//local
//#include "../../libs/MVS/Common.h"

#include "MeshBase.h"
#include "GenericTree.h"

// sys
#include<cmath>
#include<iostream>
#include<unordered_map>


#include <filesystem>


#define ZERO_TOLERANCE	(1e-7)
//TODO:各个结构体的整理

/*======================== struct ========================*/

struct MeshCutterParams
{
	double epsilon;
	bool generateOutsideMesh;
	unsigned char planeOrthoDim;
	double planeCoord;
	//for box intersection
	OpenMesh::Vec3d bbMin, bbMax;
	//for the reprojection of triangle features
	bool trackOrigIndexes;
	std::vector<unsigned> origTriIndexesMapInside;		// [new triangle index] =  old triangle index
	std::vector<unsigned> origTriIndexesMapOutside;		// ...


	// vertices
	std::vector<OpenMesh::Vec3d> insideVertices;
	std::vector<OpenMesh::Vec3d> outsideVertices;
	// texture 
	std::vector<OpenMesh::Vec2d> mappingTexCorrds;
	// normal
	std::vector<OpenMesh::Vec3d> mappingNormals;

	// face
	std::vector<OpenMesh::Vec3i> insideMesh;
	std::vector<OpenMesh::Vec3i> outsideMesh;
	// texture id
	std::vector<OpenMesh::Vec3i> mappingFaceTexId;
	// normal id
	std::vector<OpenMesh::Vec3i> mappingFaceNormId;




	MeshCutterParams()
		: generateOutsideMesh(false)
		, planeOrthoDim(0)
		, planeCoord(0)
		, bbMin(0, 0, 0)
		, bbMax(0, 0, 0)
		, trackOrigIndexes(false)
		, epsilon(1.1920928955078125e-07)
	{}

	std::vector<OpenMesh::Vec3i> getSegmentFaces() { return (generateOutsideMesh ? outsideMesh : insideMesh); }
	std::vector<OpenMesh::Vec3d> getSegmentVertices() { return (generateOutsideMesh ? outsideVertices : insideVertices); }
	unsigned getOrigTriIndexes(unsigned int id) { return (generateOutsideMesh ? origTriIndexesMapOutside[id] : origTriIndexesMapInside[id]); }

};

struct InsideOutsideIndexes
{
	InsideOutsideIndexes() : insideIndex(0), outsideIndex(0) {}
	InsideOutsideIndexes(unsigned inside, unsigned outside) : insideIndex(inside), outsideIndex(outside) {}
	unsigned insideIndex;
	unsigned outsideIndex;
};
static std::map< uint64_t, InsideOutsideIndexes > s_edgePoint;

struct segParamSpatial
{
	std::string subTag = "";		// cubes string for saving
	OpenMesh::Vec3i crts = OpenMesh::Vec3i(1, 1, 1);  // cols, rows and tens
	OpenMesh::Vec3d bBoxPtMin;
	OpenMesh::Vec3d bBoxPtMax;
	segParamSpatial(const std::string& str, const OpenMesh::Vec3i& p, const OpenMesh::Vec3d& pMin, const OpenMesh::Vec3d& pMax)
		:subTag(str)
		, crts(p)
		, bBoxPtMin(pMin)
		, bBoxPtMax(pMax)
	{}
};

struct Face
{
	OpenMesh::Vec3i verticesId;
	OpenMesh::Vec3i normalsId;
	OpenMesh::Vec3i texCoordsId;
};

struct Group
{
	std::string usemtl = "";
	std::vector<Face> faces;
};

struct OBJData {

	bool hasTexture;
	bool hasNormal;
	std::string fileName;
	std::string mtlLib;
	std::vector < OpenMesh::Vec3d > vertices;
	std::vector<OpenMesh::Vec3d> normals;
	std::vector<OpenMesh::Vec2d> texCoords;

	std::vector <Group> faceGroups;
	OBJData() { clear(); }

	void clear() {
		hasTexture = false;
		hasNormal = false;
		fileName = "";
		mtlLib = "";
		vertices.clear(); vertices.shrink_to_fit();
		normals.clear(); normals.shrink_to_fit();
		texCoords.clear(); texCoords.shrink_to_fit();
		faceGroups.clear(); faceGroups.shrink_to_fit();
	}

	unsigned int getGroupId(unsigned int triId) {
		unsigned int spliTag = 0;
		for (int i = 0; i < faceGroups.size(); i++) {
			spliTag += faceGroups[i].faces.size();
			if (spliTag > triId) {
				return i;
			}
		}
		return 0;
	}
	//判断是否为空
	bool isEmpty()
	{
		return (vertices.size() == 0);
	}
};

class AI3D_API Segmenter {

	/** \param  path            mesh path
		\param  bBoxMinOffset   Offset distance of lower left corner
		\param  bBoxMaxOffset   Offset distance of upper right corner
		\param  bBoxMin			box min point
		\param  bBoxMax			box max point
		\return  true if mesh cut successfully
	**/
	bool segmentMeshWithBBox(const std::string& pathImport, const OpenMesh::Vec3d& bBoxMinOffset, const OpenMesh::Vec3d& bBoxMaxOffset, OpenMesh::Vec3d& bBoxMin, OpenMesh::Vec3d& bBoxMax, MeshBase& mesh_base, const std::string& pathExport = "", const std::string& mtlName = "");
	bool segmentMeshWithBBox(std::vector<OpenMesh::Vec3i>& origFaces, std::vector<OpenMesh::Vec3d>& origVertices, MeshCutterParams& ioParams);

	int mappingSegmentMeshNormal(OBJData& origMesh, const std::vector<OpenMesh::Vec3i>& origFaces, MeshCutterParams& ioParams);

	/** \param  fileImport      file loading path
		\param  pathExport      file export dir
		\param  param		    parameter settings
		\return  unordered_map  mapping the path mesh saved and bBox dimensions
	**/
	std::unordered_map<std::string, std::vector<double>> segmentMeshWithSpatialBBox(const std::string& fileImport, const std::string& pathExport, const segParamSpatial& param, MeshBase& mesh_base);
public:
	std::unordered_map<std::string, std::vector<double>> segmentMeshWithSpatialBBox_new(std::string input_obj_file, std::string pathExport);

	//LoadObj
	bool LoadObj(const std::string& fileName);

	void get_bbox(OpenMesh::Vec3d& ptmin, OpenMesh::Vec3d& ptmax);
	bool LoadObj_from_mesh_base(const std::string& fileName, OBJData& mesh,MeshBase& mesh_base);

	
	//SaveObj
	bool SaveObj(const std::string& fileName, const OBJData& mesh, unsigned precision = 6);


	// get bounding box
	std::vector<OpenMesh::Vec3d> GetBoundingBox(const OBJData& meshRead);
	//get .mtl file_base_name
	bool get_mtl_filename(const std::string& inputmtldir, std::vector<std::string>& mtlfilename);

	int mappingSegmentMeshTexture(OBJData& origMesh, const std::vector<OpenMesh::Vec3i>& origFaces, MeshCutterParams& ioParams);
	void computeInterpolationWeights(const OpenMesh::Vec3d* A, const OpenMesh::Vec3d* B, \
		const OpenMesh::Vec3d* C, const OpenMesh::Vec3d* P, OpenMesh::Vec3d& weights);

private:
	OBJData m_mesh;//临时变量，至少是当前
public:
	void segment_entrance();
};

#endif //_SEGMENTER_H_