#pragma once

#define _USE_MATH_DEFINES
// #include <OpenMesh/Core/Mesh/DefaultTriMesh.hh>
// #include <OpenMesh/Core/Utils/PropertyManager.hh>
// #include <OpenMesh/Core/Mesh/TriConnectivity.hh>
#include <iostream>
#include <vector>


///////////////////////////////////////////////////////////////////////////////////////

#include <OpenMesh/Core/IO/MeshIO.hh>//必须先定义
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>

#include <OpenMesh/Tools/Utils/getopt.h>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/IO/reader/OBJReader.hh>



// Define my personal traits
struct MyTriTraits : OpenMesh::DefaultTraits
{
    // Let Point and Normal be a vector of doubles
    typedef OpenMesh::Vec3d Point;
    typedef OpenMesh::Vec3d Normal;

    VertexTraits{
private:
    Point cog_;
public:
    VertexT() :cog_(Point(0.0f, 0.0f, 0.0f)) {}
    const Point& cog()const { return cog_; }
    void set_cog(const Point& _p) { cog_ = _p; }
    };
};
typedef OpenMesh::TriMesh_ArrayKernelT<MyTriTraits> MyTriMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh_origin;


using namespace OpenMesh;
#define CHKROPT( Option ) \
  std::cout << "  provides " << #Option \
            << (OpenMesh::IO::Options::ropt.check(OpenMesh::IO::Options:: Option)?": yes\n":": no\n")
#define CHKWOPT( Option ) \
  std::cout << "  write " << #Option \
            << (OpenMesh::IO::Options::wopt.check(OpenMesh::IO::Options:: Option)?": yes\n":": no\n")
#define MESHOPT( msg, tf ) \
  std::cout << "  " << msg << ": " << ((tf)?"yes\n":"no\n")

void parse_commandline(int _argc, char **_argv, MyTriMesh& _mesh,
    OpenMesh::IO::Options &ropt, OpenMesh::IO::Options &wopt);
void usage_and_exit(int xcode);



#define SIZEOF( entity,b ) \
  std::cout << _prefix << "size of " << #entity << ": " \
            << sizeof( entity ) << std::endl;          \
  b += sizeof( entity )
template <typename Mesh>
void print_size(const std::string& _prefix = "")
{
    size_t total = 0;
    SIZEOF(typename Mesh::Vertex, total);
    SIZEOF(typename Mesh::Halfedge, total);
    SIZEOF(typename Mesh::Edge, total);
    SIZEOF(typename Mesh::Face, total);
    std::cout << _prefix << "total: " << total << std::endl;
}
#undef SIZEOF

int MyTriMesh_write_cube();


int MyTriMesh_smooth_mesh();

//How to add and remove custom properties,
//How to get and set the value of a custom property
int MyTriMesh_custom_properties();
int MyTriMesh_smooth_mesh_custom_mesh();
int MyTriMesh_smoother_T();




int MyTriMesh_move_vertex_to_normal();

int MyTriMesh_custom_traits();


int MyTriMesh_custom_trits_cogs();

int MyTriMesh_delete_elements();

//控制IO，读取并写入带纹理的（甚至多纹理的）模型
int MyTriMesh_custom_IO();


#include <algorithm>
#include <OpenMesh/Core/Utils/Property.hh>
#ifndef DOXY_IGNORE_THIS
template <class Mesh> class SmootherT
{
public:
    typedef typename Mesh::Point            cog_t;
    typedef OpenMesh::VPropHandleT< cog_t > Property_cog;
public:
    // construct with a given mesh
    SmootherT(Mesh& _mesh)
        : mesh_(_mesh)
    {
        mesh_.add_property(cog_);
    }
    ~SmootherT()
    {
        mesh_.remove_property(cog_);
    }
    // smooth mesh _iterations times
    void smooth(unsigned int _iterations)
    {
        for (unsigned int i = 0; i < _iterations; ++i)
        {
            std::for_each(mesh_.vertices_begin(),
                mesh_.vertices_end(),
                ComputeCOG(mesh_, cog_));
            std::for_each(mesh_.vertices_begin(),
                mesh_.vertices_end(),
                SetCOG(mesh_, cog_));
        }
    }
private:
    //--- private classes ---
    class ComputeCOG
    {
    public:
        ComputeCOG(Mesh& _mesh, Property_cog& _cog)
            : mesh_(_mesh), cog_(_cog)
        {}
        void operator()(const typename Mesh::VertexHandle& _vh)
        {
            typename Mesh::VertexVertexIter  vv_it;
            typename Mesh::Scalar            valence(0.0);

            mesh_.property(cog_, _vh) = typename Mesh::Point(0.0, 0.0, 0.0);
            for (vv_it = mesh_.vv_iter(_vh); vv_it.is_valid(); ++vv_it)
            {
                mesh_.property(cog_, _vh) += mesh_.point(*vv_it);
                ++valence;
            }
            mesh_.property(cog_, _vh) /= valence;
        }
    private:
        Mesh&         mesh_;
        Property_cog& cog_;
    };
    class SetCOG
    {
    public:
        SetCOG(Mesh& _mesh, Property_cog& _cog)
            : mesh_(_mesh), cog_(_cog)
        {}
        void operator()(const typename Mesh::VertexHandle& _vh)
        {
            if (!mesh_.is_boundary(_vh))
                mesh_.set_point(_vh, mesh_.property(cog_, _vh));
        }
    private:
        Mesh&         mesh_;
        Property_cog& cog_;
    };
    //--- private elements ---
    Mesh&        mesh_;
    Property_cog cog_;
};
#endif






//顶点结构
/*
struct HE_vert
{
float x;
float y;
float z;

HE_edge* edge;  // one of the half-edges emantating from the vertex//一个点触发的edge可以很多，但是只存了一个，为什么（其他的可以搜到）
};
struct HE_face
{
HE_edge* edge;  // one of the half-edges bordering the face
};

//基本的HE结构
struct HE_edge
{

HE_vert* vert;   // vertex at the end of the half-edge
HE_edge* pair;   // oppositely oriented adjacent half-edge
HE_face* face;   // face the half-edge borders
HE_edge* next;   // next half-edge around the face

};
*/
void HE_basic_operation();