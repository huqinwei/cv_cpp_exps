#ifndef __OBJREADER_H__
#define __OBJREADER_H__

#include <map>
#include <vector>

// ObjRead - Loads Wavefront .obj files from a file
class ObjRead_vertex_triplet;
class ObjRead_vertex;

class ObjRead
{
	std::map<ObjRead_vertex_triplet, int>* pVertexLookup;
	std::vector<ObjRead_vertex>* pVertices;

	// Temporary 
	std::vector<float> Coords;
	std::vector<float> TexCoords;
	std::vector<float> Normals;

	// Parses a face-vertex triplet, and returns the vertex id
	int ParseVertexTriplet(char* row);

public:
	ObjRead();
	~ObjRead();

	bool onlyCoords;

	// Loads an object from a Wavefront .obj file
	// Allocates data for vertices and triangles
	bool Load(
		const char* pPath,		// The path of the file
		int& vertex_count,		// The number of vertices in the mesh
		int& triangle_count,	// The number of triangles in the mesh
		float** ppCoords,		// Pointer to Coords-pointer
		float** ppTexCoords,	// Pointer to TexCoords-pointer (Set to NULL if not needed)
		float** ppNormals,		// Pointer to Normals-pointer (Set to NULL if not needed)
		int** ppTriangles, 		// Pointer to Triangles-pointer
		bool onlyCoordsFlag = false
	);
};

#endif //__OBJREADER_H__