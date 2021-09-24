//##########################################################################
//#                      m_mesh segmentiopn with BBox                        #
//#						  tag=1725 date=2021-05-19                         #
//##########################################################################

#include"MeshBase/Segmenter.h"
#include <boost/filesystem.hpp>
#include <cmath>
#define OBJ_INDEX_OFFSET 1
const double EPSILON = 1e-7;

/*========================  defination ========================*/
//
#define FINDMINMAX(x0,x1,x2,minV,maxV) minV = maxV = x0; if(x1<minV) minV=x1; else if(x1>maxV) maxV=x1;if(x2<minV) minV=x2; else if(x2>maxV) maxV=x2;
// X-tests
#define AXISTEST_X01(a, b, fa, fb) minV = a*v0[1] - b*v0[2]; maxV = a*v2[1] - b*v2[2]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[1] + fb * boxhalfSize[2]; if (minV>rad || maxV<-rad) return 0;
#define AXISTEST_X2(a, b, fa, fb) minV = a*v0[1] - b*v0[2]; maxV = a*v1[1] - b*v1[2]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[1] + fb * boxhalfSize[2]; if (minV>rad || maxV<-rad) return 0;

// Y-tests 
#define AXISTEST_Y02(a, b, fa, fb) minV = -a*v0[0] + b*v0[2]; maxV = -a*v2[0] + b*v2[2]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[0] + fb * boxhalfSize[2]; if (minV>rad || maxV<-rad) return 0;
#define AXISTEST_Y1(a, b, fa, fb) minV = -a*v0[0] + b*v0[2]; maxV = -a*v1[0] + b*v1[2]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[0] + fb * boxhalfSize[2]; if (minV>rad || maxV<-rad) return 0;

// Z-tests
#define AXISTEST_Z12(a, b, fa, fb) minV = a*v1[0] - b*v1[1]; maxV = a*v2[0] - b*v2[1]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[0] + fb * boxhalfSize[1]; if (minV>rad || maxV<-rad) return 0;
#define AXISTEST_Z0(a, b, fa, fb) minV = a*v0[0] - b*v0[1]; maxV = a*v1[0] - b*v1[1]; if (maxV<minV) std::swap(minV,maxV); rad = fa * boxhalfSize[0] + fb * boxhalfSize[1]; if (minV>rad || maxV<-rad) return 0;

const unsigned c_origIndexFlag = 0x80000000; //original index flag (bit 31)
const unsigned c_srcIndexFlag = 0x40000000; //source index flag (bit 30)
const unsigned c_realIndexMask = 0x3FFFFFFF; //original index mask (bit 0 to 29) --> max allowed index = 1073741823 ;)
const unsigned c_defaultArrayGrowth = 1024;


/* * * * * * * * * * * * * * * * * *   Function units  * * * * * * * * * * * * * * * * * * * * */
//
void vsubstract(const OpenMesh::Vec3d p, const OpenMesh::Vec3d q, OpenMesh::Vec3d& r) { r[0] = p[0] - q[0]; r[1] = p[1] - q[1]; r[2] = p[2] - q[2]; }
void vcross(const OpenMesh::Vec3d p, const OpenMesh::Vec3d q, OpenMesh::Vec3d& r) { r[0] = (p[1] * q[2]) - (p[2] * q[1]); r[1] = (p[2] * q[0]) - (p[0] * q[2]); r[2] = (p[0] * q[1]) - (p[1] * q[0]); }
double vdot(const OpenMesh::Vec3d p, const OpenMesh::Vec3d q) { return (p[0] * q[0]) + (p[1] * q[1]) + (p[2] * q[2]); }
double norm2d(const OpenMesh::Vec3d& p)  { return p[0] * p[0] + p[1] * p[1] + p[2] * p[2]; }
double normd(const OpenMesh::Vec3d& p)  { return std::sqrt(norm2d(p)); }

//<add zk date = 2021/06/11>
//得到AABB盒的最小最大顶点坐标
// std::vector<OpenMesh::Vec3d> GetBoundingBox(const OBJData& meshRead)
// {
// 	OpenMesh::Vec3d bbBoxMin = meshRead.vertices[0];
// 	OpenMesh::Vec3d bbBoxMax = bbBoxMin;
// 	for (auto val : meshRead.vertices) {
// 
// 		bbBoxMin[0] = std::min(bbBoxMin[0], val[0]);
// 		bbBoxMin[1] = std::min(bbBoxMin[1], val[1]);
// 		bbBoxMin[2] = std::min(bbBoxMin[2], val[2]);
// 		bbBoxMax[0] = std::max(bbBoxMax[0], val[0]);
// 		bbBoxMax[1] = std::max(bbBoxMax[1], val[1]);
// 		bbBoxMax[2] = std::max(bbBoxMax[2], val[2]);
// 	}
// 	std::vector<OpenMesh::Vec3d> AABB;
// 	AABB.push_back(bbBoxMin);
// 	AABB.push_back(bbBoxMax);
// 	return AABB;
// }
//</add>
bool TriBoxOverlapd(const OpenMesh::Vec3d& boxcenter, const OpenMesh::Vec3d& boxhalfSize, const OpenMesh::Vec3d triverts[3])
{
	/*    use separating axis theorem to test overlap between triangle and box */
	/*    need to test for overlap in these directions: */
	/*    1) the {X,Y,Z}-directions (actually, since we use the AABB of the triangle */
	/*       we do not even need to test these) */
	/*    2) normal of the triangle */
	/*    3) crossproduct(edge from tri, {X,Y,Z}-direction) */
	/*       this gives 3x3=9 more tests */

	/* move everything so that the boxcenter is in (0,0,0) */
	OpenMesh::Vec3d v0;
	OpenMesh::Vec3d v1;
	OpenMesh::Vec3d v2;
	vsubstract(triverts[0], boxcenter, v0);
	vsubstract(triverts[1], boxcenter, v1);
	vsubstract(triverts[2], boxcenter, v2);

	OpenMesh::Vec3d e0;
	vsubstract(v1, v0, e0);      /* compute triangle edge 0 */

	/* Bullet 3: */

	/*  test the 9 tests first (this was faster) */
	double rad = 0.0;
	double fex = 0.0;
	double fey = std::abs(e0[1]);
	double fez = std::abs(e0[2]);

	double minV = 0.0;
	double maxV = 0.0;
	AXISTEST_X01(e0[2], e0[1], fez, fey);
	fex = std::abs(e0[0]); //DGM: not necessary before!
	AXISTEST_Y02(e0[2], e0[0], fez, fex);
	AXISTEST_Z12(e0[1], e0[0], fey, fex);

	OpenMesh::Vec3d e1;
	vsubstract(v2, v1, e1);      /* compute triangle edge 1 */

	//fex = std::abs(e1[0]);
	fey = std::abs(e1[1]);
	fez = std::abs(e1[2]);

	AXISTEST_X01(e1[2], e1[1], fez, fey);
	fex = std::abs(e1[0]); //DGM: not necessary before!
	AXISTEST_Y02(e1[2], e1[0], fez, fex);
	AXISTEST_Z0(e1[1], e1[0], fey, fex);

	OpenMesh::Vec3d e2;
	vsubstract(v0, v2, e2);      /* compute triangle edge 2 */

	//fex = std::abs(e2[0]);
	fey = std::abs(e2[1]);
	fez = std::abs(e2[2]);

	AXISTEST_X2(e2[2], e2[1], fez, fey);
	fex = std::abs(e2[0]); //DGM: not necessary before!
	AXISTEST_Y1(e2[2], e2[0], fez, fex);
	AXISTEST_Z12(e2[1], e2[0], fey, fex);

	/* Bullet 1: */

	/*  first test overlap in the {X,Y,Z}-directions */
	/*  find minV, maxV of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in 0-direction */
	FINDMINMAX(v0[0], v1[0], v2[0], minV, maxV);
	if (minV > boxhalfSize[0] || maxV < -boxhalfSize[0])
		return false;

	/* test in 1-direction */
	FINDMINMAX(v0[1], v1[1], v2[1], minV, maxV);
	if (minV > boxhalfSize[1] || maxV < -boxhalfSize[1])
		return false;

	/* test in 2-direction */
	FINDMINMAX(v0[2], v1[2], v2[2], minV, maxV);
	if (minV > boxhalfSize[2] || maxV < -boxhalfSize[2])
		return false;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*0+d=0 */

	//double normal[3];
	vcross(e0, e1,/*normal*/e2); //DGM: we use 'e2' instead of 'normal' to save heap memory
	{
		//double vmin[3],vmax[3]; //DGM: we use e0 and e1 instead of vmin and vmax
		if (/*normal*/e2[0] > 0)
		{
			/*vmin*/e0[0] = -boxhalfSize[0] - v0[0];
			/*vmax*/e1[0] = boxhalfSize[0] - v0[0];
		}
		else
		{
			/*vmin*/e0[0] = boxhalfSize[0] - v0[0];
			/*vmax*/e1[0] = -boxhalfSize[0] - v0[0];
		}
		if (/*normal*/e2[1] > 0)
		{
			/*vmin*/e0[1] = -boxhalfSize[1] - v0[1];
			/*vmax*/e1[1] = boxhalfSize[1] - v0[1];
		}
		else
		{
			/*vmin*/e0[1] = boxhalfSize[1] - v0[1];
			/*vmax*/e1[1] = -boxhalfSize[1] - v0[1];
		}
		if (/*normal*/e2[2] > 0)
		{
			/*vmin*/e0[2] = -boxhalfSize[2] - v0[2];
			/*vmax*/e1[2] = boxhalfSize[2] - v0[2];
		}
		else
		{
			/*vmin*/e0[2] = boxhalfSize[2] - v0[2];
			/*vmax*/e1[2] = -boxhalfSize[2] - v0[2];
		}

		if (vdot(/*normal*/e2,/*vmin*/e0) > 0
			|| vdot(/*normal*/e2,/*vmax*/e1) < 0)
		{
			return false;
		}
	}

	return true;   /* box and triangle overlaps */
}

bool AddVertex(OpenMesh::Vec3d& P, std::vector<OpenMesh::Vec3d>* vertices, unsigned& index)
{
	assert(vertices);
	//add vertex to the 'vertices' set
	unsigned vertCount = vertices->size();
	if (vertCount == vertices->capacity())
	{

		try
		{
			vertices->reserve(vertCount + c_defaultArrayGrowth);
		}
		catch (const std::exception&)
		{
			//not enough memory!
			return false;
		}
	}
	vertices->push_back(P);
	index = vertCount;
	return true;
}

bool AddTriangle(unsigned iA, unsigned iB, unsigned iC,
	std::vector<OpenMesh::Vec3i>* mesh,
	bool directOrder)
{
	//special case: the m_mesh might not exist (if we skip the 'outside' m_mesh creation)
	//so we accept this eventuallity to simply the code
	if (!mesh)
		return true;

	//now add the triangle
	if (mesh->size() == mesh->capacity()
		&& (mesh->size() + c_defaultArrayGrowth > c_realIndexMask
			))
	{
		try
		{
			mesh->reserve(mesh->size() + c_defaultArrayGrowth);
		}
		catch (const std::exception&)
		{
			//not enough memory (or too many triangles!)
			return false;
		}
	}

	if (directOrder)
		mesh->push_back(OpenMesh::Vec3i(iA, iB, iC));
	else
		mesh->push_back(OpenMesh::Vec3i(iA, iC, iB));

	return true;
}

bool ComputeEdgePoint(const OpenMesh::Vec3d& A, unsigned iA,
	const OpenMesh::Vec3d& B, unsigned iB,
	unsigned& iCoutside, unsigned& iCinside,
	double planeCoord, unsigned char planeDim,
	std::vector<OpenMesh::Vec3d>* outsideVertices, std::vector<OpenMesh::Vec3d>* insideVertices)
{
	assert(outsideVertices || insideVertices);

	//first look if we already know this edge
	uint64_t key = 0;
	{
		unsigned minIndex = iA;
		unsigned maxIndex = iB;
		if (minIndex > maxIndex)
			std::swap(minIndex, maxIndex);
		key = (static_cast<uint64_t>(minIndex) << 32) | static_cast<uint64_t>(maxIndex);
	}

	//if the key (edge) already exists
	if (s_edgePoint.find(key) != s_edgePoint.end())
	{
		const InsideOutsideIndexes& pmi = s_edgePoint[key];
		iCoutside = pmi.outsideIndex;
		iCinside = pmi.insideIndex;
	}
	//otherwise we'll create it
	else
	{
		OpenMesh::Vec3d I = A + (B - A) * (planeCoord - A[planeDim]) / (B[planeDim] - A[planeDim]);

		//add vertex to the inside 'vertices' set
		iCinside = 0;
		if (insideVertices && !AddVertex(I, insideVertices, iCinside))
			return false;
		//add vertex to the outside 'vertices' set
		iCoutside = 0;
		if (outsideVertices && !AddVertex(I, outsideVertices, iCoutside))
			return false;

		s_edgePoint[key] = InsideOutsideIndexes(iCinside, iCoutside);
	}

	return true;
}

bool ImportSourceVertices(std::vector<OpenMesh::Vec3d>* srcVertices,
	std::vector<OpenMesh::Vec3i>* newMesh,
	std::vector<OpenMesh::Vec3d>* newVertices)
{
	assert(srcVertices && newMesh && newVertices);

	unsigned srcVertCount = srcVertices->size();
	unsigned newVertCount = newVertices->size();
	unsigned newTriCount = newMesh->size();

	try
	{
		//first determine the number of source vertices that should be imported
		std::vector<unsigned> newIndexMap;
		newIndexMap.resize(srcVertCount, 0);

		for (unsigned i = 0; i < newTriCount; ++i)
		{
			OpenMesh::Vec3i tsi = newMesh->at(i);
			if (tsi[0] & c_srcIndexFlag)
				newIndexMap[tsi[0] & c_realIndexMask] = 1;
			if (tsi[1] & c_srcIndexFlag)
				newIndexMap[tsi[1] & c_realIndexMask] = 1;
			if (tsi[2] & c_srcIndexFlag)
				newIndexMap[tsi[2] & c_realIndexMask] = 1;
		}

		//count the number of used vertices
		unsigned importedVertCount = 0;
		{
			for (unsigned i = 0; i < srcVertCount; ++i)
				if (newIndexMap[i])
					++importedVertCount;
		}

		if (importedVertCount == 0)
		{
			//nothing to do
			//(shouldn't happen but who knows?)
			return true;
		}

		//reserve the memory to import the source vertices

		try
		{
			newVertices->reserve(newVertices->size() + importedVertCount);
		}
		catch (const std::exception&)
		{
			//not enough memory
			return false;
		}

		//then copy them
		{
			//update the destination indexes by the way
			unsigned lastVertIndex = newVertCount;
			for (unsigned i = 0; i < srcVertCount; ++i)
			{
				if (newIndexMap[i])
				{
					newVertices->push_back(srcVertices->at(i));
					newIndexMap[i] = lastVertIndex++;
				}
			}
		}

		//update the existing indexes
		{
			for (unsigned i = 0; i < newTriCount; ++i)
			{
				OpenMesh::Vec3i& tsi = newMesh->at(i);
				if (tsi[0] & c_srcIndexFlag)
					tsi[0] = newIndexMap[tsi[0] & c_realIndexMask];
				if (tsi[1] & c_srcIndexFlag)
					tsi[1] = newIndexMap[tsi[1] & c_realIndexMask];
				if (tsi[2] & c_srcIndexFlag)
					tsi[2] = newIndexMap[tsi[2] & c_realIndexMask];
			}
		}
	}
	catch (const std::bad_alloc&)
	{
		//not enough memory
		return false;
	}

	newVertices->resize(newVertices->size());

	return true;
}

bool MergeOldTriangles(std::vector<OpenMesh::Vec3i>* origMesh,
	std::vector<OpenMesh::Vec3d>* origVertices,
	std::vector<OpenMesh::Vec3i>* newMesh,
	std::vector<OpenMesh::Vec3d>* newVertices,
	const std::vector<unsigned>& preservedTriangleIndexes,
	std::vector<unsigned>* origTriIndexesMap = nullptr)
{
	assert(origMesh && origVertices && newMesh && newVertices);

	unsigned importedTriCount = static_cast<unsigned>(preservedTriangleIndexes.size());
	unsigned origVertCount = origVertices->size();
	unsigned newVertCount = newVertices->size();
	unsigned newTriCount = newMesh->size();

	try
	{
		//first determine the number of original vertices that should be imported
		std::vector<unsigned> newIndexMap;
		newIndexMap.resize(origVertCount, 0);

		//either for the preserved triangles
		{
			for (unsigned i = 0; i < importedTriCount; ++i)
			{
				unsigned triIndex = preservedTriangleIndexes[i];
				OpenMesh::Vec3i tsi = origMesh->at(triIndex);
				newIndexMap[tsi[0]] = 1;
				newIndexMap[tsi[1]] = 1;
				newIndexMap[tsi[2]] = 1;
			}
		}

		//or by the new triangles
		{
			for (unsigned i = 0; i < newTriCount; ++i)
			{
				OpenMesh::Vec3i tsi = newMesh->at(i);
				if (tsi[0] & c_origIndexFlag)
					newIndexMap[tsi[0] & c_realIndexMask] = 1;
				if (tsi[1] & c_origIndexFlag)
					newIndexMap[tsi[1] & c_realIndexMask] = 1;
				if (tsi[2] & c_origIndexFlag)
					newIndexMap[tsi[2] & c_realIndexMask] = 1;
			}
		}

		//count the number of used vertices
		unsigned importedVertCount = 0;
		{
			for (unsigned i = 0; i < origVertCount; ++i)
				if (newIndexMap[i])
					++importedVertCount;
		}

		if (importedVertCount == 0)
		{
			//nothing to do
			//(shouldn't happen but who knows?)
			return true;
		}

		//reserve the memory to import the original vertices
		try
		{
			newVertices->reserve(newVertices->size() + importedVertCount);
		}
		catch (const std::exception&)
		{
			//not enough memory
			return false;
		}

		//then copy them
		{
			//update the destination indexes by the way
			unsigned lastVertIndex = newVertCount;
			for (unsigned i = 0; i < origVertCount; ++i)
			{
				if (newIndexMap[i])
				{
					newVertices->push_back(origVertices->at(i));
					newIndexMap[i] = lastVertIndex++;
				}
			}
		}

		//update the existing indexes
		{
			for (unsigned i = 0; i < newTriCount; ++i)
			{
				OpenMesh::Vec3i tsi = (*newMesh)[i];
				if (tsi[0] & c_origIndexFlag)
					(*newMesh)[i][0] = newIndexMap[tsi[0] & c_realIndexMask];
				if (tsi[1] & c_origIndexFlag)
					(*newMesh)[i][1] = newIndexMap[tsi[1] & c_realIndexMask];
				if (tsi[2] & c_origIndexFlag)
					(*newMesh)[i][2] = newIndexMap[tsi[2] & c_realIndexMask];
			}
		}

		if (importedTriCount)
		{
			//reserve the memory to import the original triangles
			try
			{
				newMesh->reserve(newMesh->size() + importedTriCount);
			}
			catch (const std::exception&)
			{
				//not enough memory
				return false;
			}

			//then copy them
			{
				assert(!origTriIndexesMap || newMesh->size() == origTriIndexesMap->size());
				for (unsigned i = 0; i < importedTriCount; ++i)
				{
					unsigned triIndex = preservedTriangleIndexes[i];
					OpenMesh::Vec3i tsi = origMesh->at(triIndex);
					newMesh->push_back(OpenMesh::Vec3i(newIndexMap[tsi[0]], newIndexMap[tsi[1]], newIndexMap[tsi[2]]));
					if (origTriIndexesMap)
						origTriIndexesMap->push_back(triIndex);
				}
			}
		}
	}
	catch (const std::bad_alloc&)
	{
		//not enough memory
		return false;
	}

	newMesh->resize(newMesh->size());
	newVertices->resize(newVertices->size());

	assert(!origTriIndexesMap || newMesh->size() == origTriIndexesMap->size());

	return true;
}

// * m_mesh segment
bool Segmenter::segmentMeshWithBBox(std::vector<OpenMesh::Vec3i>& origFaces,std::vector<OpenMesh::Vec3d>& origVertices, MeshCutterParams& ioParams) {

	if (
		origFaces.size() == 0
		|| origVertices.size() < 3
		|| ioParams.bbMin[0] >= ioParams.bbMax[0]
		|| ioParams.bbMin[1] >= ioParams.bbMax[1]
		|| ioParams.bbMin[2] >= ioParams.bbMax[2])
	{
		//invalid input parameters
		return false;
	}

	if (origFaces.size() > 0x3FFFFFFF)
	{
		//too many triangles!
		return false;
	}

	const double& epsilon = ioParams.epsilon;
	const OpenMesh::Vec3d& bbMin = ioParams.bbMin;
	const OpenMesh::Vec3d& bbMax = ioParams.bbMax;

	//indexes of original triangle that are not modified bt copied "as is"
	std::vector<unsigned> preservedTrianglesInside1;	//insde (1)
	std::vector<unsigned> preservedTrianglesInside2;	//insde (2)
	std::vector<unsigned> preservedTrianglesOutside;	//outside

	//inside meshes (swapped for each dimension)
	std::vector<OpenMesh::Vec3d> insideVertices1;
	std::vector<OpenMesh::Vec3i> insideMesh1;
	std::vector<OpenMesh::Vec3d> insideVertices2;
	std::vector<OpenMesh::Vec3i> insideMesh2;

	//outside m_mesh (output)
	std::vector<OpenMesh::Vec3d> outsideVertices;
	std::vector<OpenMesh::Vec3i> outsideMesh;


	//pointers on input and output structures (will change for each dimension)
	std::vector<unsigned>* preservedTrianglesInside = &preservedTrianglesInside1;
	std::vector<unsigned>* formerPreservedTriangles = &preservedTrianglesInside2;
	std::vector<OpenMesh::Vec3d>* insideVertices = &insideVertices1;
	std::vector<OpenMesh::Vec3i>* insideMesh = &insideMesh1;
	std::vector<OpenMesh::Vec3i>* sourceMesh = &origFaces;
	std::vector<OpenMesh::Vec3d>* sourceVertices = &origVertices;

	OpenMesh::Vec3d boxCenter = (ioParams.bbMin + ioParams.bbMax) / 2;
	OpenMesh::Vec3d boxHalfSize = (ioParams.bbMax - ioParams.bbMin) / 2;
	bool error = false;

	//for each triangle
	try
	{
		//for each plane
		for (unsigned d = 0; d < 6; ++d)
		{
			//Extract the 'plane' information corresponding to the input box faces
			//-X,+X,-Y,+Y,-Z,+Z
			unsigned char Z = static_cast<unsigned char>(d / 2);
			double planeCoord = ((d & 1) ? bbMax : bbMin)[Z];
			bool keepBelow = ((d & 1) ? true : false);

			assert(preservedTrianglesInside && formerPreservedTriangles);
			assert(insideVertices && insideMesh);
			assert(sourceVertices && sourceMesh);
			s_edgePoint.clear();

			std::vector<unsigned> origTriIndexesMapInsideBackup;
			if (ioParams.trackOrigIndexes)
			{
				origTriIndexesMapInsideBackup = ioParams.origTriIndexesMapInside;
				ioParams.origTriIndexesMapInside.resize(0);
			}

			//look for original triangles
			//(the first time they only come from the original m_mesh but afterwards
			// they can come from the original m_mesh through the 'preserved' list
			// or from the previous 'inside' m_mesh as we have to test those triangles
			// against the new plane)
			unsigned sourceTriCount = sourceMesh ? sourceMesh->size() : 0; //source: previous/original m_mesh
			unsigned formerPreservedTriCount = static_cast<unsigned>(formerPreservedTriangles->size());
			unsigned triCount = sourceTriCount + formerPreservedTriCount;

			for (unsigned i = 0; i < triCount; ++i)
			{

				bool triangleIsOriginal = false;
				unsigned souceTriIndex = 0;
				OpenMesh::Vec3i tsi;
				if (i < sourceTriCount)
				{
					souceTriIndex = i;
					triangleIsOriginal = (sourceMesh == &origFaces);
					tsi = sourceMesh->at(souceTriIndex);
				}
				else
				{
					souceTriIndex = (*formerPreservedTriangles)[i - sourceTriCount];
					triangleIsOriginal = true;
					tsi = origFaces[souceTriIndex];
				}

				//vertices indexes
				unsigned vertIndexes[3] = { tsi[0], tsi[1], tsi[2] };
				if (triangleIsOriginal)
				{
					//we flag the vertices indexes as referring to the 'original' m_mesh
					vertIndexes[0] |= c_origIndexFlag;
					vertIndexes[1] |= c_origIndexFlag;
					vertIndexes[2] |= c_origIndexFlag;
				}
				else
				{
					//we flag the vertices indexes as referring to the 'source' m_mesh
					if ((vertIndexes[0] & c_origIndexFlag) == 0)
						vertIndexes[0] |= c_srcIndexFlag;
					if ((vertIndexes[1] & c_origIndexFlag) == 0)
						vertIndexes[1] |= c_srcIndexFlag;
					if ((vertIndexes[2] & c_origIndexFlag) == 0)
						vertIndexes[2] |= c_srcIndexFlag;
				}

				//get the vertices (from the right source!)
				OpenMesh::Vec3d V[3] = { ((vertIndexes[0] & c_origIndexFlag) ? origVertices : *sourceVertices)[vertIndexes[0] & c_realIndexMask],
									((vertIndexes[1] & c_origIndexFlag) ? origVertices : *sourceVertices)[vertIndexes[1] & c_realIndexMask],
									((vertIndexes[2] & c_origIndexFlag) ? origVertices : *sourceVertices)[vertIndexes[2] & c_realIndexMask] };

				if (d == 0)
				{
					//perform a triangle-box overlap test the first time!
					if (!TriBoxOverlapd(boxCenter, boxHalfSize, V))
					{
						if (ioParams.generateOutsideMesh)
							preservedTrianglesOutside.push_back(i);
							
						continue;
					}
				}

				//test the position of each vertex relatively to the current plane
				//char relativePos[3] = { 1, 1, 1 };
				//bool insideXY[3] = { false, false, false };
				std::vector<unsigned char> insideLocalVertIndexes;
				std::vector<unsigned char> outsideLocalVertIndexes;
				for (unsigned char j = 0; j < 3; ++j)
				{
					const OpenMesh::Vec3d& v = V[j];
					if (std::abs(v[Z] - planeCoord) < epsilon)
					{
						//printf("%d \n",i);
						//relativePos[j] = 0;
					}
					else
					{
						if (v[Z] < planeCoord)
						{
							insideLocalVertIndexes.push_back(j);
							//relativePos[j] = -1;
						}
						else
						{
							//relativePos is already equal to 1
							//relativePos[j] = 1;
							outsideLocalVertIndexes.push_back(j);
						}
					}
				}

				//depending on the number of entities on the plane
				//we'll process the triangles differently
				bool isFullyInside = false;
				bool isFullyOutside = false;
				switch (insideLocalVertIndexes.size() + outsideLocalVertIndexes.size())
				{
				case 0: //all vertices 'in' the plane
				{
					//we arbitrarily decide that the triangle is inside!
					isFullyInside = true;
				}
				break;

				case 1: //2 vertices 'in' the plane
				{
					//the triangle is either on one side or another ;)
					if (insideLocalVertIndexes.empty())
					{
						//the only vertex far from the plane is on the 'otuside'
						isFullyOutside = true;
					}
					else
					{
						//the only vertex far from the plane is on the 'inside'
						isFullyInside = true;
					}
				}
				break;

				case 2: //1 vertex 'in' the plane
				{
					//3 cases:
					if (insideLocalVertIndexes.empty())
					{
						//the two vertices far from the plane are 'outside'
						isFullyOutside = true;
					}
					else if (outsideLocalVertIndexes.empty())
					{
						//the two vertices far from the plane are 'inside'
						isFullyInside = true;
					}
					else
					{
						//the two vertices far from the plane are on both sides
						//the plane will cut through the edge connecting those two vertices
						unsigned char iInside = insideLocalVertIndexes.front();
						unsigned char iOuside = outsideLocalVertIndexes.front();

						unsigned char iCenter = 3 - iInside - iOuside;
						unsigned iCoutside;
						unsigned iCinside;
						//we can now create one vertex and two new triangles
						if (!ComputeEdgePoint(
							V[iInside], vertIndexes[iInside],
							V[iOuside], vertIndexes[iOuside],
							iCoutside, iCinside,
							planeCoord, Z,
							&outsideVertices, insideVertices)

							|| !AddTriangle(
								vertIndexes[iCenter],
								vertIndexes[iInside],
								keepBelow ? iCinside : iCoutside,
								keepBelow ? insideMesh : &outsideMesh,
								((iCenter + 1) % 3) == iInside)

							|| !AddTriangle(
								vertIndexes[iCenter],
								vertIndexes[iOuside],
								keepBelow ? iCoutside : iCinside,
								keepBelow ? &outsideMesh : insideMesh,
								((iCenter + 1) % 3) == iOuside))
						{
							//early stop
							i = triCount;
							error = true;
							break;
						}

						//remember (origin) source triangle index
						if (ioParams.trackOrigIndexes)
						{
							assert(triangleIsOriginal || souceTriIndex < origTriIndexesMapInsideBackup.size());
							unsigned origTriIndex = triangleIsOriginal ? souceTriIndex : origTriIndexesMapInsideBackup[souceTriIndex];
							//the source triangle is split in two so each side get one new triangle
							ioParams.origTriIndexesMapInside.push_back(origTriIndex);
							if (ioParams.generateOutsideMesh)
								ioParams.origTriIndexesMapOutside.push_back(origTriIndex);
						}
					}
				}
				break;

				case 3: //no vertex 'in' the plane
				{
					if (insideLocalVertIndexes.empty())
					{
						//all vertices are 'outside'
						isFullyOutside = true;
					}
					else if (outsideLocalVertIndexes.empty())
					{
						//all vertices are 'inside'
						isFullyInside = true;
					}
					else
					{
						//we have one vertex on one side and two on the other side
						unsigned char iLeft;
						unsigned char iRight1;
						unsigned char iRight2;
						bool leftIsInside = true;
						if (insideLocalVertIndexes.size() == 1)
						{
							assert(outsideLocalVertIndexes.size() == 2);
							iLeft = insideLocalVertIndexes.front();
							iRight1 = outsideLocalVertIndexes[0];
							iRight2 = outsideLocalVertIndexes[1];
							leftIsInside = keepBelow;
						}
						else
						{
							assert(insideLocalVertIndexes.size() == 2);
							iLeft = outsideLocalVertIndexes.front();
							iRight1 = insideLocalVertIndexes[0];
							iRight2 = insideLocalVertIndexes[1];
							leftIsInside = !keepBelow;
						}

						//the plane cuts through the two edges having the 'single' vertex in common
						//we are going to create 3 triangles
						unsigned i1outside;
						unsigned i1inside;
						unsigned i2outside;
						unsigned i2inside;
						if (!ComputeEdgePoint(V[iRight1], vertIndexes[iRight1],
							V[iLeft], vertIndexes[iLeft],
							i1outside, i1inside,
							planeCoord, Z,
							&outsideVertices, insideVertices)

							|| !ComputeEdgePoint(V[iRight2], vertIndexes[iRight2],
								V[iLeft], vertIndexes[iLeft],
								i2outside, i2inside,
								planeCoord, Z,
								&outsideVertices, insideVertices)

							|| !AddTriangle(vertIndexes[iLeft],
								leftIsInside ? i1inside : i1outside,
								leftIsInside ? i2inside : i2outside,
								leftIsInside ? insideMesh : &outsideMesh,
								((iLeft + 1) % 3) == iRight1)

							|| !AddTriangle(leftIsInside ? i1outside : i1inside,
								leftIsInside ? i2outside : i2inside,
								vertIndexes[iRight1],
								leftIsInside ? &outsideMesh : insideMesh,
								((iRight2 + 1) % 3) == iRight1)

							|| !AddTriangle(vertIndexes[iRight1],
								leftIsInside ? i2outside : i2inside,
								vertIndexes[iRight2],
								leftIsInside ? &outsideMesh : insideMesh,
								((iRight2 + 1) % 3) == iRight1)
							)
						{
							//early stop
							i = triCount;
							error = true;
							break;
						}

						//remember (origin) source triangle index
						if (ioParams.trackOrigIndexes)
						{
							assert(triangleIsOriginal || souceTriIndex < origTriIndexesMapInsideBackup.size());
							unsigned origTriIndex = triangleIsOriginal ? souceTriIndex : origTriIndexesMapInsideBackup[souceTriIndex];
							//each side gets at least one new triangle
							ioParams.origTriIndexesMapInside.push_back(origTriIndex);
							if (ioParams.generateOutsideMesh)
								ioParams.origTriIndexesMapOutside.push_back(origTriIndex);
							//the third triangle has been added either to the 'inside' or to the 'outside' m_mesh
							if (!leftIsInside)
								ioParams.origTriIndexesMapInside.push_back(origTriIndex);
							else if (ioParams.generateOutsideMesh)
								ioParams.origTriIndexesMapOutside.push_back(origTriIndex);
						}
					}
				}
				break;

				}

				if (isFullyInside || isFullyOutside)
				{
					//inverted selection?
					if (!keepBelow)
						std::swap(isFullyInside, isFullyOutside);

					if (triangleIsOriginal)
					{
						if (isFullyInside)
							preservedTrianglesInside->push_back(souceTriIndex);
						else if (ioParams.generateOutsideMesh)
							preservedTrianglesOutside.push_back(souceTriIndex);
					}
					else
					{
						//we import the former triangle
						if (!AddTriangle(vertIndexes[0], vertIndexes[1], vertIndexes[2], isFullyInside ? insideMesh : &outsideMesh, true))
						{
							//early stop
							error = true;
							break;
						}
						if (ioParams.trackOrigIndexes)
						{
							assert(souceTriIndex < origTriIndexesMapInsideBackup.size());
							unsigned origTriIndex = origTriIndexesMapInsideBackup[souceTriIndex];
							if (isFullyInside)
								ioParams.origTriIndexesMapInside.push_back(origTriIndex);
							else if (ioParams.generateOutsideMesh)
								ioParams.origTriIndexesMapOutside.push_back(origTriIndex);
						}
					}
				}

			}
			//end for each triangle

			if (!ImportSourceVertices(sourceVertices, insideMesh, insideVertices)
				|| (ioParams.generateOutsideMesh && !ImportSourceVertices(sourceVertices, &outsideMesh, &outsideVertices))
				)
			{
				//early stop
				error = true;
				break;
			}

			if (insideMesh->size() == 0 && preservedTrianglesInside->empty())
			{
				//no triangle inside!
				break;
			}

			if (d < 5)
			{
				//clear the source m_mesh and swap the buffers
				if (insideMesh == &insideMesh1)
				{
					assert(sourceMesh == &insideMesh2 || sourceMesh == &origFaces);
					insideMesh2.clear();
					insideVertices2.clear();
					insideVertices2.resize(0);

					sourceMesh = &insideMesh1;
					sourceVertices = &insideVertices1;
					insideMesh = &insideMesh2;
					insideVertices = &insideVertices2;
					preservedTrianglesInside2.resize(0);
					preservedTrianglesInside = &preservedTrianglesInside2;
					formerPreservedTriangles = &preservedTrianglesInside1;
				}
				else
				{
					assert(sourceMesh == &insideMesh1 || sourceMesh == &origFaces);
					insideMesh1.clear();
					insideVertices1.clear();
					insideVertices1.resize(0);

					sourceMesh = &insideMesh2;
					sourceVertices = &insideVertices2;
					insideMesh = &insideMesh1;
					insideVertices = &insideVertices1;
					preservedTrianglesInside1.resize(0);
					preservedTrianglesInside = &preservedTrianglesInside1;
					formerPreservedTriangles = &preservedTrianglesInside2;
				}
			}
		}
		//end for each plane

		//now add the remaining triangles
	}
	catch (const std::bad_alloc&)
	{
		//not enough memory
		error = true;
	}

	//free some memory
	s_edgePoint.clear();
	formerPreservedTriangles->resize(0);

	if (!error)
	{
		//import the 'preserved' (original) triangles 
		if (!MergeOldTriangles(&origFaces, &origVertices,
			insideMesh, insideVertices,
			*preservedTrianglesInside,
			ioParams.trackOrigIndexes ? &ioParams.origTriIndexesMapInside : nullptr)
			|| (ioParams.generateOutsideMesh
				&& !MergeOldTriangles(&origFaces, &origVertices,
					&outsideMesh, &outsideVertices,
					preservedTrianglesOutside,
					ioParams.trackOrigIndexes ? &ioParams.origTriIndexesMapOutside : nullptr))
			)
		{
			error = true;
		}
	}

	if (error)
		return false;
	

	if (insideMesh)
	{
		insideMesh->resize(insideMesh->size());
	}
	if (outsideMesh.size())
	{
		outsideMesh.resize(outsideMesh.size());
	}

	bool bRet = false;
	if (ioParams.generateOutsideMesh) {

		ioParams.outsideMesh = outsideMesh;
		ioParams.outsideVertices = outsideVertices;
		
		bRet = ioParams.outsideVertices.size() > 0;
		printf("verticesOut %d   facesOut %d \n",ioParams.outsideVertices.size(), ioParams.outsideMesh.size());
	}
	else {
	
		ioParams.insideMesh = *insideMesh;
		ioParams.insideVertices = *insideVertices;
		bRet = ioParams.insideVertices.size() > 0;
		printf("verticesIn %d    facesIn %d \n", ioParams.insideVertices.size(), ioParams.insideMesh.size());
	}

	return bRet;
}

// 
void Segmenter::computeInterpolationWeights(const OpenMesh::Vec3d* A, const OpenMesh::Vec3d* B, \
	const OpenMesh::Vec3d* C, const OpenMesh::Vec3d* P, OpenMesh::Vec3d& weights) {

	//barcyentric intepolation weights
	weights[0] = normd((*P - (*B)).cross(*C - (*B)))/*/2*/;
	weights[1] = normd((*P - (*C)).cross(*A - (*C)))/*/2*/;
	weights[2] = normd((*P - (*A)).cross(*B - (*A)))/*/2*/;

	//normalize weights
	double sum = weights[0] + weights[1] + weights[2];
	weights /= sum+EPSILON;
}

// reMapping m_mesh texture
int Segmenter::mappingSegmentMeshTexture(OBJData& origMesh, const std::vector<OpenMesh::Vec3i>& origFaces,MeshCutterParams& ioParams) {


	std::vector<OpenMesh::Vec3i>* segFaces = nullptr;
	std::vector<OpenMesh::Vec3d>*  segVertices = nullptr;
	if (ioParams.generateOutsideMesh) {
		segVertices = &(ioParams.outsideVertices);
		segFaces = &(ioParams.outsideMesh);
	}
	else {
		segVertices = &(ioParams.insideVertices);
		segFaces = &(ioParams.insideMesh);
	}

	const std::vector<unsigned>& origTriIndexes = ioParams.generateOutsideMesh ? ioParams.origTriIndexesMapOutside : ioParams.origTriIndexesMapInside;
	//for each new triangle
	//const double ZERO_TOLERANCE = static_cast<double>(FLT_EPSILON);
	for (unsigned i = 0; i < segFaces->size();i++) {
	
		//get the origin triangle
		unsigned origTriIndex = origTriIndexes[i];
		OpenMesh::Vec3i verticesId = origFaces[origTriIndex];
		
		OpenMesh::Vec3i textureId;
		{
			int numFaces = 0;
			int numFacesBefGroup = 0;
			for (int g = 0; g < origMesh.faceGroups.size();g++) {
				numFaces += origMesh.faceGroups[g].faces.size();
				if (numFaces > origTriIndex) {
					
					textureId = origMesh.faceGroups[g].faces[origTriIndex - numFacesBefGroup].texCoordsId;
					break;
				}
				numFacesBefGroup += origMesh.faceGroups[g].faces.size();
			}
		}

		const OpenMesh::Vec3d* A = &(origMesh.vertices[verticesId[0]]);
		const OpenMesh::Vec3d* B = &(origMesh.vertices[verticesId[1]]);
		const OpenMesh::Vec3d* C = &(origMesh.vertices[verticesId[2]]);

		// 
		const OpenMesh::Vec2d* tx1 = &(origMesh.texCoords[textureId[0]]);
		const OpenMesh::Vec2d* tx2 = &(origMesh.texCoords[textureId[1]]);
		const OpenMesh::Vec2d* tx3 = &(origMesh.texCoords[textureId[2]]);


		//get the new triangle
		OpenMesh::Vec3i texIndexes( -1, -1, -1 );
		for (unsigned j = 0; j < 3; ++j)
		{
			const OpenMesh::Vec3d* v = &(segVertices->at(segFaces->at(i)[j]));
			//intepolation weights
			OpenMesh::Vec3d w;
			computeInterpolationWeights(A,B,C, v, w);
			if (tx1) {
				if (std::isnan((*tx1)[0]) || std::isnan((*tx1)[1]) 
					 ||std::isnan(w[0]) || std::isnan(w[1]) || std::isnan(w[2])
					) {
					std::cout << "err" << std::endl;
				}
			}



			if ((tx1 || w[0] < ZERO_TOLERANCE)
				&& (tx2 || w[1] < ZERO_TOLERANCE)
				&& (tx3 || w[2] < ZERO_TOLERANCE))
			{
				OpenMesh::Vec2d t = OpenMesh::Vec2d((tx1 ? (*tx1)[0] * w[0] : 0.0) + (tx2 ? (*tx2)[0] * w[1] : 0.0) + (tx3 ? (*tx3)[0] * w[2] : 0.0),
					(tx1 ? (*tx1)[1] * w[0] : 0.0) + (tx2 ? (*tx2)[1] * w[1] : 0.0) + (tx3 ? (*tx3)[1] * w[2] : 0.0));

				ioParams.mappingTexCorrds.push_back(t);
				texIndexes[j] = ioParams.mappingTexCorrds.size() - 1;
			}
		}
		ioParams.mappingFaceTexId.push_back(texIndexes);
	}


	return 0;
}

// reMapping m_mesh normal
int Segmenter::mappingSegmentMeshNormal(OBJData& origMesh, const std::vector<OpenMesh::Vec3i>& origFaces, MeshCutterParams& ioParams) {


	std::vector<OpenMesh::Vec3i>* segFaces = nullptr;
	std::vector<OpenMesh::Vec3d>* segVertices = nullptr;
	if (ioParams.generateOutsideMesh) {
		segVertices = &(ioParams.outsideVertices);
		segFaces = &(ioParams.outsideMesh);
	}
	else {
		segVertices = &(ioParams.insideVertices);
		segFaces = &(ioParams.insideMesh);
	}

	const std::vector<unsigned>& origTriIndexes = ioParams.generateOutsideMesh ? ioParams.origTriIndexesMapOutside : ioParams.origTriIndexesMapInside;
	//for each new triangle
	for (unsigned i = 0; i < segFaces->size(); i++) {

		//get the origin triangle
		unsigned origTriIndex = origTriIndexes[i];
		OpenMesh::Vec3i verticesId = origFaces[origTriIndex];

		OpenMesh::Vec3i normalId;
		{
			int numFaces = 0;
			int numFacesBefGroup = 0;
			for (int g = 0; g < origMesh.faceGroups.size(); g++) {
				numFaces += origMesh.faceGroups[g].faces.size();
				if (numFaces > origTriIndex) {

					normalId = origMesh.faceGroups[g].faces[origTriIndex - numFacesBefGroup].normalsId;
					break;
				}
				numFacesBefGroup += origMesh.faceGroups[g].faces.size();
			}
		}

		const OpenMesh::Vec3d* A = &(origMesh.vertices[verticesId[0]]);
		const OpenMesh::Vec3d* B = &(origMesh.vertices[verticesId[1]]);
		const OpenMesh::Vec3d* C = &(origMesh.vertices[verticesId[2]]);

		// 
		const OpenMesh::Vec3d* n1 = &(origMesh.normals[normalId[0]]);
		const OpenMesh::Vec3d* n2 = &(origMesh.normals[normalId[1]]);
		const OpenMesh::Vec3d* n3 = &(origMesh.normals[normalId[2]]);

		//get the new triangle
		OpenMesh::Vec3i normalIndexes(-1, -1, -1);
		for (unsigned j = 0; j < 3; ++j)
		{
			const OpenMesh::Vec3d* v = &(segVertices->at(segFaces->at(i)[j]));
			//intepolation weights
			OpenMesh::Vec3d w;
			computeInterpolationWeights(A, B, C, v, w);
			if ((n1 || w[0] < ZERO_TOLERANCE)
				&& (n2 || w[1] < ZERO_TOLERANCE)
				&& (n3 || w[2] < ZERO_TOLERANCE))
			{
				OpenMesh::Vec3d n = OpenMesh::Vec3d((n1 ? (*n1)[0] * w[0] : 0.0) + (n2 ? (*n2)[0] * w[1] : 0.0) + (n3 ? (*n3)[0] * w[2] : 0.0),\
									(n1 ? (*n1)[1] * w[0] : 0.0) + (n2 ? (*n2)[1] * w[1] : 0.0) + (n3 ? (*n3)[1] * w[2] : 0.0),\
									(n1 ? (*n1)[2] * w[0] : 0.0) + (n2 ? (*n2)[2] * w[1] : 0.0) + (n3 ? (*n3)[2] * w[2] : 0.0)
				);

				ioParams.mappingNormals.push_back(n);
				normalIndexes[j] = ioParams.mappingNormals.size() - 1;
			}
		}
		ioParams.mappingFaceNormId.push_back(normalIndexes);
	}

	return 0;
}

// update mtl file in relative file_path
std::set<std::string> updateMtlRelativePath(const std::string& src, const std::string& dir) {

	std::set<std::string> textures;

	//reference
	std::ostringstream out;
	{
		std::ifstream fin(src.c_str());
		if (!fin.good())
			return textures;

		std::string line, keyword, value;

		while (fin.good()) {

			std::streampos pos = fin.tellg();
			fin >> keyword;
			if (keyword.size() > std::string("map_").size()) {
				if (keyword.substr(0, 4).compare("map_") == 0) {
				
					fin >> value;
					textures.insert(value);
					value = keyword + " " + dir+ "/" + value;
					out << value + "\n";
					value.clear();
					keyword.clear();
					continue;
				}
			}
			fin.seekg(pos);

			std::getline(fin, line);
			out << line + "\n";
			line.clear();
		}
		fin.close();
	}

	boost::filesystem::remove(src);


	// save
	{
		std::ofstream fout(src.c_str());
		if (!fout.good())
			return textures;

		fout << out.str();
		fout.close();
	}

	return textures;
}

/* * * * * * * * * * * * * * * * * *   m_mesh io   * * * * * * * * * * * * * * * * * * * * */

void Segmenter::get_bbox(OpenMesh::Vec3d& ptmin, OpenMesh::Vec3d& ptmax) {

	// get bounding box
	ptmin = m_mesh.vertices[0];
	ptmax = ptmin;
	for (auto val : m_mesh.vertices) {

		ptmin[0] = std::min(ptmin[0], val[0]);
		ptmin[1] = std::min(ptmin[1], val[1]);
		ptmin[2] = std::min(ptmin[2], val[2]);
		ptmax[0] = std::max(ptmax[0], val[0]);
		ptmax[1] = std::max(ptmax[1], val[1]);
		ptmax[2] = std::max(ptmax[2], val[2]);
	}
}
// m_mesh load 
bool Segmenter::LoadObj(const std::string& fileName)
{

	const unsigned int no_id = std::numeric_limits<uint32_t>::max();

	m_mesh.clear();
	m_mesh.fileName = fileName;

	std::ifstream fin(fileName.c_str());
	std::string line, keyword;
	std::istringstream in;
	while (fin.good()) {
		std::getline(fin, line);
		if (line.empty() || line[0u] == '#')
			continue;
		in.str(line);
		in >> keyword;
		if (keyword == "v") {
			double v[3];
			in >> v[0] >> v[1] >> v[2];
			if (in.fail())
				return false;
			m_mesh.vertices.push_back(OpenMesh::Vec3d(v[0],v[1],v[2]));
		}
		else
			if (keyword == "vt") {
				double vt[2];
				in >> vt[0] >> vt[1];
				if (in.fail())
					return false;
				m_mesh.texCoords.push_back(OpenMesh::Vec2d(vt[0],vt[1]));
				m_mesh.hasTexture = true;  //
			}
			else
				if (keyword == "vn") {
					double vn[3];
					in >> vn[0] >> vn[1] >> vn[2];
					if (in.fail())
						return false;
					m_mesh.normals.push_back(OpenMesh::Vec3d(vn[0],vn[1],vn[2]));
					m_mesh.hasNormal = true;  //
				}
				else
					if (keyword == "f") {
						Face f;
						memset(&f, 0xFF, sizeof(Face));
						for (size_t k = 0; k < 3; ++k) {
							in >> keyword;
							switch (sscanf(keyword.c_str(), "%u/%u/%u", &(f.verticesId[k]), &(f.texCoordsId[k]), &(f.normalsId[k]))) {
							case 1:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								break;
							case 2:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								if (f.texCoordsId[k] != no_id)
									f.texCoordsId[k] -= OBJ_INDEX_OFFSET;
								if (f.normalsId[k] != no_id)
									f.normalsId[k] -= OBJ_INDEX_OFFSET;
								break;
							case 3:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								f.texCoordsId[k] -= OBJ_INDEX_OFFSET;
								f.normalsId[k] -= OBJ_INDEX_OFFSET;
								break;
							default:
								return false;
							}
						}
						if (in.fail())
							return false;

						m_mesh.faceGroups.back().faces.push_back(f);
					}
					else
						if (keyword == "mtllib") {
							in >> keyword;

							m_mesh.mtlLib = keyword;

						}
						else
							if (keyword == "usemtl") {
								Group group;
								in >> group.usemtl;
								if (in.fail())
									return false;
								m_mesh.faceGroups.push_back(group);
							}
		in.clear();
	}

	return !m_mesh.vertices.empty();
}
// m_mesh load 
bool Segmenter::LoadObj_from_mesh_base(const std::string& fileName, OBJData& mesh,MeshBase& mesh_base)
{

	const unsigned int no_id = std::numeric_limits<uint32_t>::max();

	mesh.clear();
	mesh.fileName = fileName;

	std::ifstream fin(fileName.c_str());
	std::string line, keyword;
	std::istringstream in;
	while (fin.good()) {
		std::getline(fin, line);
		if (line.empty() || line[0u] == '#')
			continue;
		in.str(line);
		in >> keyword;
		if (keyword == "v") {
			double v[3];
			in >> v[0] >> v[1] >> v[2];
			if (in.fail())
				return false;
			mesh.vertices.push_back(OpenMesh::Vec3d(v[0], v[1], v[2]));
		}
		else
			if (keyword == "vt") {
				double vt[2];
				in >> vt[0] >> vt[1];
				if (in.fail())
					return false;
				mesh.texCoords.push_back(OpenMesh::Vec2d(vt[0], vt[1]));
				mesh.hasTexture = true;  //
			}
			else
				if (keyword == "vn") {
					double vn[3];
					in >> vn[0] >> vn[1] >> vn[2];
					if (in.fail())
						return false;
					mesh.normals.push_back(OpenMesh::Vec3d(vn[0], vn[1], vn[2]));
					mesh.hasNormal = true;  //
				}
				else
					if (keyword == "f") {
						Face f;
						memset(&f, 0xFF, sizeof(Face));
						for (size_t k = 0; k < 3; ++k) {
							in >> keyword;
							switch (sscanf(keyword.c_str(), "%u/%u/%u", &(f.verticesId[k]), &(f.texCoordsId[k]), &(f.normalsId[k]))) {
							case 1:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								break;
							case 2:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								if (f.texCoordsId[k] != no_id)
									f.texCoordsId[k] -= OBJ_INDEX_OFFSET;
								if (f.normalsId[k] != no_id)
									f.normalsId[k] -= OBJ_INDEX_OFFSET;
								break;
							case 3:
								f.verticesId[k] -= OBJ_INDEX_OFFSET;
								f.texCoordsId[k] -= OBJ_INDEX_OFFSET;
								f.normalsId[k] -= OBJ_INDEX_OFFSET;
								break;
							default:
								return false;
							}
						}
						if (in.fail())
							return false;

						mesh.faceGroups.back().faces.push_back(f);
					}
					else
						if (keyword == "mtllib") {
							in >> keyword;

							mesh.mtlLib = keyword;

						}
						else
							if (keyword == "usemtl") {
								Group group;
								in >> group.usemtl;
								if (in.fail())
									return false;
								mesh.faceGroups.push_back(group);
							}
		in.clear();
	}

	return !mesh.vertices.empty();
}

// m_mesh save
bool Segmenter::SaveObj(const std::string& fileName, const OBJData& mesh, unsigned precision ) {

	if (mesh.vertices.empty())
		return false;

	std::ofstream out(fileName.c_str());
	if (!out.good())
		return false;

	out << "#" << "\n";
	out << "# Wavefront OBJ file" << "\n";
	out << "#  mesh segmention with bounding box" << "\n";
	out << "#" << "\n\n";

	out << "mtllib " << mesh.mtlLib << "\n";

	out << std::fixed << std::setprecision(precision);
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
		//<< std::setprecision(4)
		out << "v "
			<< mesh.vertices[i][0] << " "
			<< mesh.vertices[i][1] << " "
			<< mesh.vertices[i][2] << "\n";
	}

	if (mesh.hasTexture) {
		for (size_t i = 0; i < mesh.texCoords.size(); ++i) {
			if (std::isinf(mesh.texCoords[i][0]) || std::isinf(mesh.texCoords[i][1])) {
				std::cout << "err" << std::endl;
			}
			if (std::isnan(mesh.texCoords[i][0]) || std::isnan(mesh.texCoords[i][1])) {
				std::cout << "err" << std::endl;
			}
			out << "vt "
				<< (double)mesh.texCoords[i][0] << " "
				<< (double)mesh.texCoords[i][1] << "\n";
		}
	}

	if (mesh.hasNormal) {
		for (size_t i = 0; i < mesh.normals.size(); ++i) {
			if (std::isinf(mesh.normals[i][0]) || std::isinf(mesh.normals[i][1])) {
				std::cout << "err" << std::endl;
			}
			if (std::isnan(mesh.normals[i][0]) || std::isnan(mesh.normals[i][1])) {
				std::cout << "err" << std::endl;
			}
			out << "vn "
				<< (double)mesh.normals[i][0] << " "
				<< (double)mesh.normals[i][1] << " "
				<< (double)mesh.normals[i][2] << "\n";
		}
	}

	for (size_t i = 0; i < mesh.faceGroups.size(); ++i) {
		out << "usemtl " << mesh.faceGroups[i].usemtl << "\n";
		for (size_t j = 0; j < mesh.faceGroups[i].faces.size(); ++j) {
			Face const& face = mesh.faceGroups[i].faces[j];
			out << "f";
			for (size_t k = 0; k < 3; ++k) {

				if (face.texCoordsId[k] + OBJ_INDEX_OFFSET == 0) {
					out << " " << face.verticesId[k] + OBJ_INDEX_OFFSET
						<< "/"
						<< "/" << face.normalsId[k] + OBJ_INDEX_OFFSET;
					continue;
				}
				if (face.normalsId[k] + OBJ_INDEX_OFFSET == 0) {
					out << " " << face.verticesId[k] + OBJ_INDEX_OFFSET
						<< "/" << face.texCoordsId[k] + OBJ_INDEX_OFFSET;
					continue;

				}
				out << " " << face.verticesId[k] + OBJ_INDEX_OFFSET
					<< "/" << face.texCoordsId[k] + OBJ_INDEX_OFFSET
					<< "/" << face.normalsId[k] + OBJ_INDEX_OFFSET;

			}
			out << "\n";
		}
	}

	return true;
}


/* * * * * * * * * * * * * * * *   m_mesh segmention   * * * * * * * * * * * * * * * * * * * * */
//
bool Segmenter::segmentMeshWithBBox(const std::string& fileImport, const OpenMesh::Vec3d& bBoxMinOffset, const OpenMesh::Vec3d& bBoxMaxOffset, OpenMesh::Vec3d& bBoxMin, OpenMesh::Vec3d& bBoxMax, MeshBase& mesh_base, const std::string& fileExport,const std::string& mtlName) {
	// obj file_path valid?
	if (!boost::filesystem::exists(fileImport.c_str()))
		return false;
	
	// m_mesh file_path
	std::string pathRead = fileImport;
	std::string pathSave = fileImport;//  boost::filesystem::file_path(file_path).parent_path().string() + "/" + boost::filesystem::basename(file_path) + "_segment" + ".obj";
	if (!fileExport.empty()) {
		pathSave = fileExport;
	}

	////////////////////////////////////////////////////////////////
	//TODO:考虑一下类的关系，直接从mesh类拿数据
	//mappingSegmentMeshTexture是否有必要，又是否足够正确
	//先确定下边到底有几步，再确定哪一步是必须的，mtl映射如果不要~！

	// 1. load m_mesh
	OBJData meshRead;
// 	if (!LoadObj(pathRead, meshRead))
// 		return false;
	if (!LoadObj_from_mesh_base(pathRead, meshRead, mesh_base))
		return false;

	// get face
	std::vector<OpenMesh::Vec3i> faces;
	for (auto group : meshRead.faceGroups) {

		for (auto f : group.faces) {
			faces.push_back(OpenMesh::Vec3i(f.verticesId[0], f.verticesId[1], f.verticesId[2]));
		}
	}

	// get bounding box
	OpenMesh::Vec3d bbBoxMin = meshRead.vertices[0];
	OpenMesh::Vec3d bbBoxMax = bbBoxMin;
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

	// 2. cut m_mesh
	ioParams.trackOrigIndexes = true;
	// m_mesh segmention
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

	// 4. save m_mesh data
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
			const OpenMesh::Vec3i& normalId = ioParams.mappingFaceNormId[i];
			f.normalsId = OpenMesh::Vec3i(normalId[0] + normalOffset[normalId[0]], normalId[1] + normalOffset[normalId[1]], normalId[2] + normalOffset[normalId[2]]);
		}

		// vt
		if (meshSave.hasTexture) {

			const OpenMesh::Vec3i& texId = ioParams.mappingFaceTexId[i];
			f.texCoordsId = OpenMesh::Vec3i(texId[0] + offsetVal[texId[0]], texId[1] + offsetVal[texId[1]], texId[2] + offsetVal[texId[2]]);
		}

		meshSave.faceGroups[groupId].faces.push_back(f);
	}


	// 5. remove old file
	if (fileExport.empty()) {
		boost::filesystem::remove(pathRead);
	}

	// 6. save m_mesh data
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

inline void get_file_name_parts(const std::string& filename, std::string& absolute_path, std::string& base_name, std::string& ext_name) {
	std::vector<std::string>  res_vec2 = split(filename, "/");
	std::string res2 = res_vec2[res_vec2.size() - 1];
	std::vector<std::string>  res_vec3 = split(res2, ".");
	base_name = res_vec3[0];
	ext_name = res_vec3[1];
	absolute_path = filename.substr(0, filename.size() - res2.size());
}
//
std::unordered_map<std::string, std::vector<double>>  Segmenter::segmentMeshWithSpatialBBox(const std::string& fileImport, const std::string& pathExport, const segParamSpatial& param, MeshBase& mesh_base) {

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
	const OpenMesh::Vec3d ptMinShiftReaerved = OpenMesh::Vec3d(-overlpReserved, -overlpReserved, -overlpReserved-0.00001f);
	const OpenMesh::Vec3d ptMaxShiftReaerved = OpenMesh::Vec3d(overlpReserved, overlpReserved, overlpReserved+0.00001f);

	//todo: 获取文件名、文件扩展名、文件路径
	std::string temp = fileImport;

	std::string file_base_name;
	std::string file_ext_name;
	std::string file_path;
	std::filesystem::path path(fileImport);
	if (path.has_extension()) {
		file_ext_name = path.extension().string();
	}
	if (path.has_stem()) {
		file_base_name = path.stem().string();
	}
	if (path.has_parent_path()) {
		file_path = path.parent_path().string();
	}
	file_path += "/";



	//todo:这一层才算是四叉树调用的位置（当然，有一些是可以写死了，比如2行2列）
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


		// calculate current cube border offset corresponding to the orignal m_mesh
		double x = 1.0f * xDimension * idx + ptMinShiftReaerved[0];
		double y = 1.0f * yDimension * idy + ptMinShiftReaerved[1];
		double z = 1.0f * zDimension * idz + ptMinShiftReaerved[2];
		OpenMesh::Vec3d bBoxMinOffset(x, y, z);

		x = 1.0f * xDimension * (idx - param.crts[0] + 1) + ptMaxShiftReaerved[0];
		y = 1.0f * yDimension * (idy - param.crts[1] + 1) + ptMaxShiftReaerved[1];
		z = 1.0f * zDimension * (idz - param.crts[2] + 1) + ptMaxShiftReaerved[2];
		OpenMesh::Vec3d bBoxMaxOffset(x, y, z);

		//强行增维
		if(bBoxMinOffset[2] == bBoxMaxOffset[2]){
			bBoxMinOffset[2] -= 0.0001f;
			bBoxMaxOffset[2] += 0.0001f;
		}

		std::string save_name = pathExport + param.subTag + strId + ".obj";

		printf("cube name: %s  cube position: [ %d %d %d ]  ", save_name.c_str(), idx, idy, idz);

		OpenMesh::Vec3d bBoxMin, bBoxMax;
		std::string mtlSharedName = "seg_shared.mtl";
		std::string mtlSharedPath = pathExport + mtlSharedName;
		if (segmentMeshWithBBox(fileImport, bBoxMinOffset, bBoxMaxOffset, bBoxMin, bBoxMax, mesh_base, save_name, mtlSharedName)) {
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
				std::string src = file_path  + file_base_name + ".mtl";
			
				boost::filesystem::copy_file(src, mtlSharedPath);
				std::string texturesDir = "tempTextures";
				std::set<std::string> textures = updateMtlRelativePath(mtlSharedPath, texturesDir);


				// textures
				for (auto filename : textures) {
				
					std::string src = file_path + filename;
					std::string dst = pathExport +  texturesDir + "/" + filename;
					
					boost::filesystem::create_directory(pathExport + texturesDir);
					boost::filesystem::copy_file(src, dst);
				}

			}
		}
	}

	return result_name;
}

std::unordered_map<std::string, std::vector<double>>  Segmenter::segmentMeshWithSpatialBBox_new(std::string input_obj_file, std::string pathExport) {
	MeshBase mesh_base;
	//获取文件名、文件扩展名、文件路径
	std::string file_base_name;
	std::string file_ext_name;
	std::string file_path;
	std::filesystem::path path(input_obj_file);
	if (path.has_extension()) {
		file_ext_name = path.extension().string();
	}
	if (path.has_stem()) {
		file_base_name = path.stem().string();
	}
	if (path.has_parent_path()) {
		file_path = path.parent_path().string();
	}
	

	//std::string pathExport = file_path+"_out"+"/";//这样会有bug，产生xxx_out_out，所以不能内部做，外部设定好
	//"F:/handwork_data/data/27_MJSJHY_out/" "F:/handwork_data/data_out/27_MJSJHY/";//TODO:挪出去，或者在这原地拼接一个
	file_path += "/";


	mesh_base.read_obj_file(input_obj_file);//四边形
	OpenMesh::Vec3i crts(2, 2, 1);//固定
	OpenMesh::Vec3d ptmin,ptmax;//实际模型的bboxmin
#if 0
	mesh_base.get_bbox(ptmin, ptmax);
#else
	LoadObj(input_obj_file);
	get_bbox(ptmin, ptmax);
#endif
	segParamSpatial param(file_base_name, crts, ptmin, ptmax);//TODO:文件名形式，每一层分别

	std::unordered_map<std::string, std::vector<double>> result_name;
	if (param.crts[0] * param.crts[1] * param.crts[2] < 1) {
		return result_name;
	}

	if (!boost::filesystem::exists(input_obj_file))
		return result_name;

	// variables 
	double xDimension = (param.bBoxPtMax[0] - param.bBoxPtMin[0]) / param.crts[0];
	double yDimension = (param.bBoxPtMax[1] - param.bBoxPtMin[1]) / param.crts[1];
	double zDimension = (param.bBoxPtMax[2] - param.bBoxPtMin[2]) / param.crts[2];

	const double overlpReserved = 0.00f;
	const double min_z_dimension = 1e-5;
	const OpenMesh::Vec3d ptMinShiftReaerved = OpenMesh::Vec3d(-overlpReserved, -overlpReserved, -overlpReserved - min_z_dimension);
	const OpenMesh::Vec3d ptMaxShiftReaerved = OpenMesh::Vec3d(overlpReserved, overlpReserved, overlpReserved + min_z_dimension);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//本版本：四叉树逻辑在外部通过字符串对接
	//todo:输入改为从MeshBase，输出也改为一个MeshBase的副本
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		// calculate current cube border offset corresponding to the orignal m_mesh
		double x = 1.0f * xDimension * idx + ptMinShiftReaerved[0];
		double y = 1.0f * yDimension * idy + ptMinShiftReaerved[1];
		double z = 1.0f * zDimension * idz + ptMinShiftReaerved[2];
		OpenMesh::Vec3d bBoxMinOffset(x, y, z);

		x = 1.0f * xDimension * (idx - param.crts[0] + 1) + ptMaxShiftReaerved[0];
		y = 1.0f * yDimension * (idy - param.crts[1] + 1) + ptMaxShiftReaerved[1];
		z = 1.0f * zDimension * (idz - param.crts[2] + 1) + ptMaxShiftReaerved[2];
		OpenMesh::Vec3d bBoxMaxOffset(x, y, z);

		if (bBoxMinOffset[2] == bBoxMaxOffset[2]) {
			bBoxMinOffset[2] -= min_z_dimension*10;//todo:*10是因为旧代码的数组是0.001
			bBoxMaxOffset[2] += min_z_dimension * 10;
		}

		std::string save_name = pathExport + param.subTag + strId + ".obj";

		printf("cube name: %s  cube position: [ %d %d %d ]  ", save_name.c_str(), idx, idy, idz);

		OpenMesh::Vec3d bBoxMin, bBoxMax;
		std::string mtlSharedName = "seg_shared.mtl";
		std::string mtlSharedPath = pathExport + mtlSharedName;
		if (segmentMeshWithBBox(input_obj_file, bBoxMinOffset, bBoxMaxOffset, bBoxMin, bBoxMax, mesh_base, save_name, mtlSharedName)) {
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
				std::string src = file_path + file_base_name + ".mtl";

				boost::filesystem::copy_file(src, mtlSharedPath);
				std::string texturesDir = "tempTextures";
				std::set<std::string> textures = updateMtlRelativePath(mtlSharedPath, texturesDir);

				// textures
				for (auto filename : textures) {

					std::string src = file_path + filename;
					std::string dst = pathExport + texturesDir + "/" + filename;

					boost::filesystem::create_directory(pathExport + texturesDir);
					boost::filesystem::copy_file(src, dst);
				}

			}
		}
	}

	return result_name;
}
void Segmenter::segment_entrance() {
	//废弃
	return;
}