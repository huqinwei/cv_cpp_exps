#ifdef _WIN32
#ifdef SAAS
#include <SimplygonSDKCloudLoader.h>
#else
#include <SimplygonSDKLoader.h>
#endif
#elif defined(__linux__)
#include <SimplygonSDKNixLoader.h>
#endif

#include "Simplifier\OBJReader.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <climits>

using namespace std;

//////////////////////////////////////////////////////////////////////////////

class ObjRead_vertex_triplet
{
public:
	int v;
	int vt;
	int vn;
	
	ObjRead_vertex_triplet() {};
	ObjRead_vertex_triplet( const ObjRead_vertex_triplet &other );
	ObjRead_vertex_triplet &operator = ( const ObjRead_vertex_triplet &other );

	bool operator == ( const ObjRead_vertex_triplet &other ) const;
	bool operator < ( const ObjRead_vertex_triplet &other ) const;
};

ObjRead_vertex_triplet::ObjRead_vertex_triplet( const ObjRead_vertex_triplet &other )
{
	v=other.v;
	vt=other.vt;
	vn=other.vn;
}

ObjRead_vertex_triplet &ObjRead_vertex_triplet::operator = ( const ObjRead_vertex_triplet &other )
{
	v=other.v;
	vt=other.vt;
	vn=other.vn;
	return *this;
}

bool ObjRead_vertex_triplet::operator == ( const ObjRead_vertex_triplet &other ) const
{
	return (memcmp(this,&other,sizeof(*this)) == 0);
}

bool ObjRead_vertex_triplet::operator < ( const ObjRead_vertex_triplet &other ) const
{
	return (memcmp(this,&other,sizeof(*this)) < 0);
}

class ObjRead_vertex
{
public:
	float v[3];
	float vt[2];
	float vn[3];
};

//////////////////////////////////////////////////////////////////////////////

ObjRead::ObjRead()
{
	pVertexLookup = NULL;
	pVertices = NULL;
	onlyCoords = false;
}

ObjRead::~ObjRead()
{
	if( pVertexLookup ) 
	{
		delete pVertexLookup;
	}
	if( pVertices ) 
	{
		delete pVertices;
	}
}

int ObjRead::ParseVertexTriplet( char *row )
{
	// up to 2 '/' chars can exist, treat each part separately
	ObjRead_vertex_triplet vtri;
			
	// find the parts
	int cnt = 0;
	int len=0;
	while( row[len] != '\0' )
	{	
		if( row[len] == '/' )
		{
			row[len] = '\0';
			++cnt;
		}
		++len;
	}
//	Asserte(cnt<=2,"Invalid .obj file field, more than 2 '/' in face token");
	
	int pos = 0;
	int partlen;
		
	// first part is the coord id
	partlen = int(strlen(&row[pos]));
	vtri.v = atoi(row)-1;
	pos += (partlen+1);
	
	vtri.vt = INT_MAX;
	vtri.vn = INT_MAX;
	
	// second part is the texcoord id
	if( cnt >= 1 )
	{
		partlen = int(strlen(&row[pos]));
		if( partlen > 0 && !(onlyCoords) )
		{
			vtri.vt = atoi(&row[pos])-1;
		}
		pos += (partlen+1);
	
		// third part is the vertex normal id
		if( cnt >= 2 )
		{
			partlen = int(strlen(&row[pos]));
			if( partlen > 0 && !(onlyCoords) )
			{
				vtri.vn = atoi(&row[pos])-1;
			}
		}
	}
	
	// find in the map
	map<ObjRead_vertex_triplet,int>::iterator i;
	i = pVertexLookup->find(vtri);
	
	// if the triplet was not found, add it
	if( i == pVertexLookup->end() )
	{
		ObjRead_vertex tmp;
		
		// copy vertex data
		tmp.v[0] = this->Coords[vtri.v*3+0];
		tmp.v[1] = this->Coords[vtri.v*3+1];
		tmp.v[2] = this->Coords[vtri.v*3+2];
		if( vtri.vt == INT_MAX )
		{
			tmp.vt[0] = 0.f;
			tmp.vt[1] = 0.f;
		}
		else
		{
			tmp.vt[0] = this->TexCoords[vtri.vt*2+0];
			tmp.vt[1] = this->TexCoords[vtri.vt*2+1];
		}
		if( vtri.vn == INT_MAX )
		{
			tmp.vn[0] = 0.f;
			tmp.vn[1] = 0.f;
			tmp.vn[2] = 0.f;
		}
		else
		{
			tmp.vn[0] = this->Normals[vtri.vn*3+0];
			tmp.vn[1] = this->Normals[vtri.vn*3+1];
			tmp.vn[2] = this->Normals[vtri.vn*3+2];
		}
		
		// add mapping
		int vid = int(pVertices->size());
		pVertexLookup->insert( pair<ObjRead_vertex_triplet,int>( vtri , vid ) );
	
		// add to vector
		pVertices->push_back( tmp );
		
		return vid;
	}
	
	// return id of the vertex
	return i->second;
}

bool ObjRead::Load(
		const char *pPath ,		// The path of the file
		int &vertex_count ,		// The number of vertices in the mesh
		int &triangle_count ,	// The number of triangles in the mesh
		float **ppCoords ,		// Pointer to Coords-pointer
		float **ppTexCoords ,	// Pointer to TexCoords-pointer (Set to NULL if not needed)
		float **ppNormals ,		// Pointer to Normals-pointer (Set to NULL if not needed)
		int **ppTriangles ,		// Pointer to VertexIds-pointer
		bool onlyCoordsFlag 
	)
{
	std::vector<int> Triangles;

	onlyCoords = onlyCoordsFlag;

	FILE *fp = fopen( pPath , "rt" );

	if(fp == NULL)
		{
		std::cerr << "Error: file not found\n";
		return false;
		}

	// clear the vertex lookup
	if( pVertexLookup == NULL )
	{
		pVertexLookup = new std::map<ObjRead_vertex_triplet,int>;
	}
	pVertexLookup->clear();
	if( pVertices == NULL )
	{
		pVertices = new vector<ObjRead_vertex>;
	}
	pVertices->clear();
	Coords.clear();
	TexCoords.clear();
	Normals.clear();

	// allocate the row used to read a string from the file
	char *row = new char[400];
	
	// loop until done
	while( !feof(fp) )
	{
		// read a row from the file
		fgets(row,400,fp);

		// check type

		// vertex, either coord, normal, or texcoord
		if( row[0] == 'v' )
		{
			float x = 0,y = 0,z = 0;

			if( row[1] == 't' )
			{
				// texture coord
				if(!sscanf(row, "vt %f %f", &x, &y))
					return false;

				TexCoords.push_back(x);
				TexCoords.push_back(y);

			} 
			else if( row[1] == 'n' )
			{
				// normal 
				if(!sscanf(row, "vn %f %f %f", &x, &y, &z))
					return false;
				Normals.push_back(x);
				Normals.push_back(y);
				Normals.push_back(z);
			}
			else
			{
				// coord
				if(!sscanf(row, "v %f %f %f", &x, &y, &z))
					return false;
				Coords.push_back(x);
				Coords.push_back(y);
				Coords.push_back(z);
			}
				
			continue;
		}

		// face
		if( row[0] == 'f' )
		{
			int tri[3];
			int vtx_cnt = 0;
				
			char *tmp = strtok( row , " \t\n" );
			tmp = strtok( NULL , " \t\n" );		
			while( tmp != NULL )
			{
				// parse the triplet
				int vid = this->ParseVertexTriplet( tmp );

				// add to the tri structure
				tri[vtx_cnt] = vid;
				if( vtx_cnt < 2 )
				{
					++vtx_cnt;
				}
				else
				{
					// output the triangle
					Triangles.push_back(tri[0]);
					Triangles.push_back(tri[1]);
					Triangles.push_back(tri[2]);

					tri[1] = vid;
				}

				// move to next triplet				
				tmp = strtok( NULL , " \t\n" );
			}

			continue;
		}

		// not recognized, go on
	}
	fclose(fp);

	delete [] row;

	// Copy the data
	vertex_count = int(pVertices->size());
	triangle_count = int(Triangles.size()/3);
	
	// Coords
	if( ppCoords )
	{
		*ppCoords = new float[vertex_count*3];
		for( int i=0; i<vertex_count; ++i )
		{
			(*ppCoords)[i*3+0] = (*pVertices)[i].v[0];
			(*ppCoords)[i*3+1] = (*pVertices)[i].v[1];
			(*ppCoords)[i*3+2] = (*pVertices)[i].v[2];
		}
	}
	
	// TexCoords
	if( ppTexCoords && TexCoords.size() > 1)
	{
		*ppTexCoords = new float[vertex_count*2];
		for( int i=0; i<vertex_count; ++i )
		{
			(*ppTexCoords)[i*2+0] = (*pVertices)[i].vt[0];
			(*ppTexCoords)[i*2+1] = (*pVertices)[i].vt[1];
		}
	}

	// Normals
	if( ppNormals && Normals.size() > 1 )
	{
		*ppNormals = new float[vertex_count*3];
		for( int i=0; i<vertex_count; ++i )
		{
			(*ppNormals)[i*3+0] = (*pVertices)[i].vn[0];
			(*ppNormals)[i*3+1] = (*pVertices)[i].vn[1];
			(*ppNormals)[i*3+2] = (*pVertices)[i].vn[2];
		}
	}

	// Triangles
	if( ppTriangles )
	{
		*ppTriangles = new int[triangle_count*3];
		memcpy_s( *ppTriangles , sizeof( int )*triangle_count * 3 , &Triangles[0] , sizeof(int)*triangle_count*3 );
	}

	return true;
}
