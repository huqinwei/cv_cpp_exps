#include "Merge/Merge.h"
//寻找边界边和环
template<class MeshT>
bool Merge::FindBoundary(MeshT* _mesh, const std::string& head_filename, std::vector<typename MeshT::VertexHandle>& head, const std::string& tail_filename, std::vector<typename MeshT::VertexHandle>& tail,
    int overlap, const std::string& edgefile, const std::string& objfile, std::vector<std::vector<typename MeshT::VertexHandle>>& merge_boundary)
{
    std::string line;
    std::ifstream ifile_head(head_filename);
    while (ifile_head.good())
    {
        getline(ifile_head, line);
        if (line != " " && line != "")
        {
            head.push_back(OpenMesh::VertexHandle::VertexHandle(atoi(line.c_str())));
        }
    }
    ifile_head.close();
    std::ifstream ifile_tail(tail_filename);
    while (ifile_tail.good())
    {
        getline(ifile_tail, line);
        if (line != " " && line != "")
        {
            tail.push_back(OpenMesh::VertexHandle::VertexHandle(atoi(line.c_str())));
        }
    }
    ifile_tail.close();
    //去除数组中的重复元素
    sort(head.begin(), head.end());
    head.erase(unique(head.begin(), head.end()), head.end());
    sort(tail.begin(), tail.end());
    tail.erase(unique(tail.begin(), tail.end()), tail.end());
    //输出结果文件
    std::ofstream ofile(objfile, std::ios::out | std::ios::app);
    std::ifstream ifile(edgefile, std::ios::in);
    std::set<typename MeshT::VertexHandle>edge_point;

    while (ifile.good())
    {
        getline(ifile, line);
        if (line != " " && line != "")
            edge_point.insert(MeshT::VertexHandle::VertexHandle(atoi(line.c_str())));
    }

    if (!ofile.is_open())
    {
        std::cout << "无法打开文件" << std::endl;
    }
    else if (!head.empty() && !tail.empty())
    {
        ofile << "\n#Line\n";
        //给mesh添加访问属性
        OpenMesh::VPropHandleT<bool> visited;

        if (!_mesh->get_property_handle(visited, "Visited Vertices"))
        {

            _mesh->add_property(visited, "Visited Vertices");
        }

        typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
        int vhcnt = 0;
        //每个点的初值设置为未访问状态
        for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it)
        {
            vhcnt++;
            _mesh->property(visited, *v_it) = false;
        }

        for (int i = 0; i < head.size(); i++)
        {
            for (int ii = 0; ii < tail.size(); ii++)
            {
                std::vector<typename MeshT::VertexHandle> vBound;
                typename MeshT::Point head0 = _mesh->point(head[0]);
                typename MeshT::Point tail0 = _mesh->point(tail[0]);//head0和tail0只做位置限制


                typename MeshT::VertexHandle vbegin = (overlap ? (head0[0] < tail0[0]) : (head0[1] < tail0[1])) ? head[i] : tail[ii];
                typename MeshT::VertexHandle vend = (overlap ? (head0[0] > tail0[0]) : (head0[1] > tail0[1])) ? head[i] : tail[ii];
                if (head[i].is_valid() && tail[0].is_valid())
                {
                    //walk around the boundary and select primitves
                    //检测边界线
                    std::stack<typename MeshT::VertexHandle> stack;
                    stack.push(vbegin);
                    int loop = 0;
                    while (!stack.empty())
                    {
                        loop++;
                        typename MeshT::VertexHandle vh = stack.top();
                        typename MeshT::Point p = _mesh->point(vh);
                        stack.pop();

                        if (_mesh->property(visited, vh))
                            continue;

                        //find outgoing boundary-edges
                        vhcnt = 0;
                        // auto result = vBound[vhcnt].insert(vh);
                        for (typename MeshT::VertexOHalfedgeIter voh_it(*_mesh, vh); voh_it.is_valid(); ++voh_it)
                        {
                            if (_mesh->is_boundary(_mesh->edge_handle(*voh_it)))
                            {
                                typename   MeshT::VertexHandle vi = _mesh->to_vertex_handle(*voh_it);
                                if (_mesh->property(visited, vi))
                                {
                                    continue;
                                }
                                typename MeshT::Point pi = _mesh->point(vi);
                                // overlap代表重叠方向，0代表X重叠，1代表Y重叠；                       
                                if ((overlap && pi[0] > head0[0] && pi[0] < tail0[0] && vi != vend) ||
                                    (!overlap && pi[1] > head0[1] && pi[1] < tail0[1]) && vi != vend)
                                {
                                    vhcnt++;//记录分叉数
                                    if (vhcnt > 1) {
                                        typename MeshT::VertexHandle v_pre = stack.top();
                                        typename MeshT::Point p_pre = _mesh->point(v_pre);
                                        if (abs(p_pre[2] - p[2]) > abs(pi[2] - p[2]))
                                        {

                                            stack.pop();
                                            stack.push(vi);
                                            //vBound[vhcnt - 1].insert(vh);
                                           // vBound[vhcnt - 1].insert(v_pre);
                                        }
                                        else {

                                        }
                                    }
                                    else
                                    {
                                        vBound.push_back(vh);//记录主干边界边
                                        stack.push(vi);
                                    }
                                    //对应
                                    //if (_primitiveTypes & edgeType_)
                                    //    _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                                }//处理边界首位元素，找到尾点
                                else if (find(tail.begin(), tail.end(), vi) != tail.end())
                                {
                                    vBound.push_back(vh);
                                    vBound.push_back(vi);
                                    //if (_primitiveTypes & edgeType_)
                                    //    _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                                }//从首点出发又回到首点
                                else if (find(head.begin(), head.end(), vi) != head.end())
                                {
                                    vBound.push_back(vh);
                                    vBound.push_back(vi);
                                    //if (_primitiveTypes & edgeType_)
                                    //    _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                                }
                            }
                        }
                        _mesh->property(visited, vh) = true;

                    }
                    if (!vBound.empty())
                    {

                        merge_boundary.push_back(vBound);
                        ofile << "l ";
                        for (int j = 0; j < vBound.size(); j++)
                        {
                            ofile << vBound[j].idx() + 1 << " ";
                        }
                        ofile << "\n";
                    }
                    ofile.flush();

                    for (int i = 0; i < vBound.size(); i++)
                    {
                        auto pos = edge_point.find(vBound[i]);
                        if (pos != edge_point.end())
                        {
                            edge_point.erase(pos);
                        }
                    }
                }
                else
                {
                    std::cout << "Unable to find boundary." << std::endl;
                    return false;
                }
            }

        }

        //检测环（思路：从CC中读取所有边界点，除去边界线上的点，如果遍历剩余顶点，检测环，记录环上的点；除去环上的点继续检测……）
        ofile << "#Loop\n";
        //对edge_point中剩余边界点进行环检测
        //std::vector<typename MeshT::VertexHandle> vBound;
        int kkk_loop = 0;
        while (!edge_point.empty())
        {
            kkk_loop++;
            std::vector<typename MeshT::VertexHandle> vloop;
            int vhcnt;
            typename MeshT::VertexHandle vh0 = *edge_point.begin();
            std::stack<typename MeshT::VertexHandle> stack;
            stack.push(vh0);
            while (!stack.empty())
            {
                typename MeshT::VertexHandle vh = stack.top();
                stack.pop();
                if (_mesh->property(visited, vh))
                {
                    continue;
                }

                vhcnt = 0;
                for (typename MeshT::VertexOHalfedgeIter voh_it(*_mesh, vh); voh_it.is_valid(); ++voh_it)
                {
                    if (_mesh->is_boundary(*voh_it))
                    {
                        vhcnt++;
                        vloop.push_back(vh);
                        stack.push(_mesh->to_vertex_handle(*voh_it));
                        //if (_primitiveTypes & edgeType_)
                        //	_mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                    }
                }

                _mesh->property(visited, vh) = true;
            }
            if (!vloop.empty())
            {
                vloop.push_back(vh0);
                merge_boundary.push_back(vloop);
                ofile << "l ";
                for (int i = 0; i < vloop.size(); i++)
                {
                    ofile << vloop[i].idx() + 1 << " ";
                    auto pos = edge_point.find(vloop[i]);
                    if (pos != edge_point.end())
                    {
                        edge_point.erase(pos);
                    }
                }
                ofile << vh0.idx() + 1 << "\n";
            }

            ofile.flush();
            if (edge_point.size() <= 2)
            {
                break;
            }
            if (kkk_loop == 1000)
            {
                //寻找环失败；异常退出
                std::cout << "寻找环失败，程序异常退出" << std::endl;
                return false;
            }
        }

        _mesh->remove_property(visited);
    }
    ofile.close();
    return true;
}

//加面
void Merge::add_face(TriMesh &m, std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_0, std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_1,
std::vector<TriMesh::VertexHandle>& head_0, std::vector<TriMesh::VertexHandle>& tail_0, std::vector<TriMesh::VertexHandle>& head_1, std::vector<TriMesh::VertexHandle>& tail_1,const double& move_dist,const std::string& dst_obj)
{
    //计算一个obj的首或尾两两坐标Z轴的差值的最小值，XY是一致的
    std::vector<double> error_z;//point_0和point_1的Z轴之差最小值
    std::vector<double> z_sub;//point_0和point_1的Z轴之差
    if (head_0.size() > 1)
    {
        for (int i = 0; i < head_0.size(); i++)
        {
            z_sub.clear();
            TriMesh::Point point_0 = m.point(head_0[i]);
            for (int j = 0; j < head_1.size(); j++)
            {
                TriMesh::Point point_1 = m.point(head_1[j]);
                z_sub.push_back(abs(point_1[2] - point_0[2]));
            }
            error_z.push_back(*std::min_element(z_sub.begin(), z_sub.end()));
        }
    }
    else if (tail_0.size() > 1)
    {
        for (int i = 0; i < tail_0.size(); i++)
        {
            z_sub.clear();
            TriMesh::Point point_0 = m.point(tail_0[i]);
            for (int j = 0; j < tail_1.size(); j++)
            {
                TriMesh::Point point_1 = m.point(tail_1[j]);
                z_sub.push_back(abs(point_1[2] - point_0[2]));
            }
            error_z.push_back(*std::min_element(z_sub.begin(), z_sub.end()));
        }
    }



    std::ofstream ofile_obj(dst_obj, std::ios::out | std::ios::app);
    //环加面，在起点加面
    //加面：缝合边加面，即寻找两个obj的一条缝合边的对应首尾点加面
    std::vector<TriMesh::VertexHandle> vhandles_head;
    std::vector<TriMesh::VertexHandle> vhandles_tail;
    std::vector<TriMesh::VertexHandle> vh_loop;
    for (int i = 0; i < merge_boundary_0.size(); i++)
    {
        vhandles_head.clear();
        vhandles_tail.clear();
        vh_loop.clear();
        TriMesh::VertexHandle vh0_head = merge_boundary_0[i][0];
        TriMesh::Point point0_head = m.point(vh0_head);
        TriMesh::VertexHandle vh0_head_next = merge_boundary_0[i][1];
        TriMesh::VertexHandle vh0_tail = merge_boundary_0[i][merge_boundary_0[i].size() - 1];
        TriMesh::Point point0_tail = m.point(vh0_tail);
        if (vh0_head != vh0_tail)
        {
            //边界边:头和头加面
            for (int j = 0; j < merge_boundary_1.size(); j++)
            {
                TriMesh::VertexHandle vh1_head = merge_boundary_1[j][0];
                TriMesh::Point point1_head = m.point(vh1_head);
                TriMesh::VertexHandle vh1_head_next = merge_boundary_1[j][1];
                TriMesh::Point point1_head_next = m.point(vh1_head_next);
                TriMesh::VertexHandle vh1_tail = merge_boundary_1[j][merge_boundary_1[j].size() - 1];
                TriMesh::Point point1_tail = m.point(vh1_tail);
                TriMesh::VertexHandle vh1_tail_next = merge_boundary_1[j][merge_boundary_1[j].size() - 2];
                TriMesh::Point point1_tail_next = m.point(vh1_tail_next);

                //首尾点加面
                if (vh1_head != vh1_tail && (find(error_z.begin(), error_z.end(), abs(point0_head[2] - point1_head[2])) != error_z.end() ||
                    find(error_z.begin(), error_z.end(), abs(point0_tail[2] - point1_tail[2])) != error_z.end()) || (head_0.size() == 1 && tail_0.size() == 1))
                {
                    vhandles_head.push_back(vh0_head);
                    vhandles_head.push_back(vh1_head);
                    vhandles_head.push_back(vh1_head_next);//取第一个obj和首点相邻的顶点
                    m.add_face(vhandles_head);
                    ofile_obj << "f " << vh0_head.idx() + 1 << " " << vh1_head.idx() + 1 << " " << vh1_head_next.idx() + 1 << "\n";

                    vhandles_tail.push_back(vh0_tail);
                    vhandles_tail.push_back(vh1_tail);
                    vhandles_tail.push_back(vh1_tail_next);//取第一个obj和首点相邻的顶点
                    m.add_face(vhandles_tail);
                    ofile_obj << "f " << vh0_tail.idx() + 1 << " " << vh1_tail.idx() + 1 << " " << vh1_tail_next.idx() + 1 << "\n";
                    break;
                }
                //边界边加面：头和尾加面
                if (vh1_head != vh1_tail && (find(error_z.begin(), error_z.end(), abs(point0_head[2] - point1_tail[2])) != error_z.end()))
                {
                    vhandles_head.push_back(vh0_head);
                    vhandles_head.push_back(vh0_head_next);
                    vhandles_head.push_back(vh1_tail);//取第一个obj和首点相邻的顶点
                    m.add_face(vhandles_head);
                    ofile_obj << "f " << vh0_head.idx() + 1 << " " << vh0_head_next.idx() + 1 << " " << vh1_tail.idx() + 1 << "\n";

                    vhandles_tail.push_back(vh0_tail);
                    vhandles_tail.push_back(vh1_head);
                    vhandles_tail.push_back(vh1_head_next);//取第一个obj和首点相邻的顶点
                    m.add_face(vhandles_tail);
                    ofile_obj << "f " << vh0_tail.idx() + 1 << " " << vh1_head.idx() + 1 << " " << vh1_head_next.idx() + 1 << "\n";
                    break;
                }
            }
        }
        else
        {
            //环加面
            //找到距离point0_head最近的点，进行加面
            double distance = DBL_MAX;
            TriMesh::VertexHandle vh_min_dis_index;
            for (int j = 0; j < merge_boundary_1.size(); j++)
            {
                TriMesh::VertexHandle vh1_head = merge_boundary_1[j][0];
                TriMesh::Point point1_head = m.point(vh1_head);
                TriMesh::VertexHandle vh1_head_next = merge_boundary_1[j][1];
                TriMesh::Point point_head_next = m.point(vh1_head_next);
                TriMesh::VertexHandle vh1_tail = merge_boundary_1[j][merge_boundary_1[j].size() - 1];
                TriMesh::Point point1_tail = m.point(vh1_tail);
                if (vh1_head == vh1_tail)
                {
                    double temp_dist;
                    for (int k = 0; k < merge_boundary_1[j].size(); k++)
                    {
                        TriMesh::Point point_temp = m.point(merge_boundary_1[j][k]);
                        temp_dist = sqrt(pow(point_temp[0] - point0_head[0], 2) + pow(point_temp[1] - point0_head[1], 2) + pow(point_temp[2] - point0_head[2], 2));
                        if (temp_dist < distance)
                        {
                            distance = temp_dist;
                            vh_min_dis_index = merge_boundary_1[j][k];
                        }
                    }
                }
            }
            //找到对应环和对应位置距离最小的点，找到的最小值点可能稍微比2*move_dist大一点；但相比于没有对应环的点的distance会大很多
            if (distance < 2 * move_dist * 1.1)
            {
                vh_loop.push_back(vh0_head);
                vh_loop.push_back(vh0_head_next);
                vh_loop.push_back(vh_min_dis_index);
                m.add_face(vh_loop);
                ofile_obj << "f " << vh0_head.idx() + 1 << " " << vh0_head_next.idx() + 1 << " " << vh_min_dis_index.idx() + 1 << "\n";
            }
        }
    }

    //if (!OpenMesh::IO::write_mesh(m, objfile))
    //{
       // ofile_log << "Cannot write mesh!\n";
    //}

    ofile_obj.close();
}

int main(int argc,char **argv)
{
    std::string objfile = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/test.obj";
 //   std::string logfile = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/log.txt";
 //   std::ofstream ofile_log(logfile, std::ios::out | std::ios::app);
    //文件备份
    std::string src_obj = objfile;
    std::size_t slash_pos = objfile.find_last_of("/");
    std::size_t dot_pos = objfile.find_last_of(".");
    std::string basename = objfile.substr(0, slash_pos);
    std::string dst_obj = basename + "/" + objfile.substr(slash_pos + 1, dot_pos - slash_pos - 1) + "bak.obj";
    if (!CopyFileA(src_obj.c_str(), dst_obj.c_str(), true))
    {
        std::cout << "无法备份文件！" << std::endl;
        return 1;
    }
    MeshBase mesh;
    mesh.read_obj_file(dst_obj);
    Merge merge = Merge(&mesh);

    
    OpenMesh::IO::Options ropt, wopt;
    //m.request_vertex_texcoords2D();
   // m.request_vertex_normals();
   // ropt += OpenMesh::IO::Options::VertexTexCoord;
    //ropt += OpenMesh::IO::Options::VertexNormal;
    //if (!OpenMesh::IO::read_mesh(m, dst_obj))
    //{
    //    std::cout << "Cannot read mesh!\n";
    //}
    //给mesh添加法线
//if (!ropt.check(OpenMesh::IO::Options::VertexNormal))
//{
//    m.request_face_normals();
//    m.update_normals();
//    m.release_face_normals();
//}

   // Merge merge = Merge(&m);

   //测试数据Y方向的重叠
    int overlap = 0;
    //两块obj各自平移的拉开距离
    double move_dist = 0.980103 / 2;
    //读取obj缝合首尾点
    std::vector<TriMesh::VertexHandle> vhead0;
    std::vector<TriMesh::VertexHandle> vhead1;
    std::string head_filename_0 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/head1_0.txt";
    std::string head_filename_1 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/head0_1.txt";


    std::vector<TriMesh::VertexHandle> vtail1;
    std::string tail_filename_0 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/tail1_0.txt";
    std::string tail_filename_1 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/tail0_1.txt";

    std::string edgefile_0 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/edge1_0.txt";
    std::string edgefile_1 = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/edge0_1.txt";
    std::vector<std::vector<TriMesh::VertexHandle>> merge_boundary_0;
    std::vector<std::vector<TriMesh::VertexHandle>> merge_boundary_1;
    std::vector<typename TriMesh::VertexHandle> head_0;
    std::vector<typename TriMesh::VertexHandle> tail_0;
    std::vector<typename TriMesh::VertexHandle> head_1;
    std::vector<typename TriMesh::VertexHandle> tail_1;
    
    if (!merge.FindBoundary(&mesh.m_mesh, head_filename_0, head_0, tail_filename_0, tail_0, overlap, edgefile_0, dst_obj, merge_boundary_0))
    {
        std::cout << "寻找边界边失败！！！" << std::endl;
    }
    if (!merge.FindBoundary(&mesh.m_mesh, head_filename_1, head_1, tail_filename_1, tail_1, overlap, edgefile_1, dst_obj, merge_boundary_1))
    {
        std::cout << "寻找边界边失败！！！" << std::endl;
    }
    merge.add_face(mesh.m_mesh,merge_boundary_0, merge_boundary_1, head_0, tail_0, head_1, tail_1,move_dist,dst_obj);

    std::string objout = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/test_output.obj";
    mesh.write_obj_file(objout);
}