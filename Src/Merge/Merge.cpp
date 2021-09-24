#include "Merge/Merge.h"
//Ѱ�ұ߽�ߺͻ�
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
    //ȥ�������е��ظ�Ԫ��
    sort(head.begin(), head.end());
    head.erase(unique(head.begin(), head.end()), head.end());
    sort(tail.begin(), tail.end());
    tail.erase(unique(tail.begin(), tail.end()), tail.end());
    //�������ļ�
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
        std::cout << "�޷����ļ�" << std::endl;
    }
    else if (!head.empty() && !tail.empty())
    {
        ofile << "\n#Line\n";
        //��mesh��ӷ�������
        OpenMesh::VPropHandleT<bool> visited;

        if (!_mesh->get_property_handle(visited, "Visited Vertices"))
        {

            _mesh->add_property(visited, "Visited Vertices");
        }

        typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
        int vhcnt = 0;
        //ÿ����ĳ�ֵ����Ϊδ����״̬
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
                typename MeshT::Point tail0 = _mesh->point(tail[0]);//head0��tail0ֻ��λ������


                typename MeshT::VertexHandle vbegin = (overlap ? (head0[0] < tail0[0]) : (head0[1] < tail0[1])) ? head[i] : tail[ii];
                typename MeshT::VertexHandle vend = (overlap ? (head0[0] > tail0[0]) : (head0[1] > tail0[1])) ? head[i] : tail[ii];
                if (head[i].is_valid() && tail[0].is_valid())
                {
                    //walk around the boundary and select primitves
                    //���߽���
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
                                // overlap�����ص�����0����X�ص���1����Y�ص���                       
                                if ((overlap && pi[0] > head0[0] && pi[0] < tail0[0] && vi != vend) ||
                                    (!overlap && pi[1] > head0[1] && pi[1] < tail0[1]) && vi != vend)
                                {
                                    vhcnt++;//��¼�ֲ���
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
                                        vBound.push_back(vh);//��¼���ɱ߽��
                                        stack.push(vi);
                                    }
                                    //��Ӧ
                                    //if (_primitiveTypes & edgeType_)
                                    //    _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                                }//����߽���λԪ�أ��ҵ�β��
                                else if (find(tail.begin(), tail.end(), vi) != tail.end())
                                {
                                    vBound.push_back(vh);
                                    vBound.push_back(vi);
                                    //if (_primitiveTypes & edgeType_)
                                    //    _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);
                                }//���׵�����ֻص��׵�
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

        //��⻷��˼·����CC�ж�ȡ���б߽�㣬��ȥ�߽����ϵĵ㣬�������ʣ�ඥ�㣬��⻷����¼���ϵĵ㣻��ȥ���ϵĵ������⡭����
        ofile << "#Loop\n";
        //��edge_point��ʣ��߽����л����
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
                //Ѱ�һ�ʧ�ܣ��쳣�˳�
                std::cout << "Ѱ�һ�ʧ�ܣ������쳣�˳�" << std::endl;
                return false;
            }
        }

        _mesh->remove_property(visited);
    }
    ofile.close();
    return true;
}

//����
void Merge::add_face(TriMesh &m, std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_0, std::vector<std::vector<TriMesh::VertexHandle>>& merge_boundary_1,
std::vector<TriMesh::VertexHandle>& head_0, std::vector<TriMesh::VertexHandle>& tail_0, std::vector<TriMesh::VertexHandle>& head_1, std::vector<TriMesh::VertexHandle>& tail_1,const double& move_dist,const std::string& dst_obj)
{
    //����һ��obj���׻�β��������Z��Ĳ�ֵ����Сֵ��XY��һ�µ�
    std::vector<double> error_z;//point_0��point_1��Z��֮����Сֵ
    std::vector<double> z_sub;//point_0��point_1��Z��֮��
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
    //�����棬��������
    //���棺��ϱ߼��棬��Ѱ������obj��һ����ϱߵĶ�Ӧ��β�����
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
            //�߽��:ͷ��ͷ����
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

                //��β�����
                if (vh1_head != vh1_tail && (find(error_z.begin(), error_z.end(), abs(point0_head[2] - point1_head[2])) != error_z.end() ||
                    find(error_z.begin(), error_z.end(), abs(point0_tail[2] - point1_tail[2])) != error_z.end()) || (head_0.size() == 1 && tail_0.size() == 1))
                {
                    vhandles_head.push_back(vh0_head);
                    vhandles_head.push_back(vh1_head);
                    vhandles_head.push_back(vh1_head_next);//ȡ��һ��obj���׵����ڵĶ���
                    m.add_face(vhandles_head);
                    ofile_obj << "f " << vh0_head.idx() + 1 << " " << vh1_head.idx() + 1 << " " << vh1_head_next.idx() + 1 << "\n";

                    vhandles_tail.push_back(vh0_tail);
                    vhandles_tail.push_back(vh1_tail);
                    vhandles_tail.push_back(vh1_tail_next);//ȡ��һ��obj���׵����ڵĶ���
                    m.add_face(vhandles_tail);
                    ofile_obj << "f " << vh0_tail.idx() + 1 << " " << vh1_tail.idx() + 1 << " " << vh1_tail_next.idx() + 1 << "\n";
                    break;
                }
                //�߽�߼��棺ͷ��β����
                if (vh1_head != vh1_tail && (find(error_z.begin(), error_z.end(), abs(point0_head[2] - point1_tail[2])) != error_z.end()))
                {
                    vhandles_head.push_back(vh0_head);
                    vhandles_head.push_back(vh0_head_next);
                    vhandles_head.push_back(vh1_tail);//ȡ��һ��obj���׵����ڵĶ���
                    m.add_face(vhandles_head);
                    ofile_obj << "f " << vh0_head.idx() + 1 << " " << vh0_head_next.idx() + 1 << " " << vh1_tail.idx() + 1 << "\n";

                    vhandles_tail.push_back(vh0_tail);
                    vhandles_tail.push_back(vh1_head);
                    vhandles_tail.push_back(vh1_head_next);//ȡ��һ��obj���׵����ڵĶ���
                    m.add_face(vhandles_tail);
                    ofile_obj << "f " << vh0_tail.idx() + 1 << " " << vh1_head.idx() + 1 << " " << vh1_head_next.idx() + 1 << "\n";
                    break;
                }
            }
        }
        else
        {
            //������
            //�ҵ�����point0_head����ĵ㣬���м���
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
            //�ҵ���Ӧ���Ͷ�Ӧλ�þ�����С�ĵ㣬�ҵ�����Сֵ�������΢��2*move_dist��һ�㣻�������û�ж�Ӧ���ĵ��distance���ܶ�
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
    //�ļ�����
    std::string src_obj = objfile;
    std::size_t slash_pos = objfile.find_last_of("/");
    std::size_t dot_pos = objfile.find_last_of(".");
    std::string basename = objfile.substr(0, slash_pos);
    std::string dst_obj = basename + "/" + objfile.substr(slash_pos + 1, dot_pos - slash_pos - 1) + "bak.obj";
    if (!CopyFileA(src_obj.c_str(), dst_obj.c_str(), true))
    {
        std::cout << "�޷������ļ���" << std::endl;
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
    //��mesh��ӷ���
//if (!ropt.check(OpenMesh::IO::Options::VertexNormal))
//{
//    m.request_face_normals();
//    m.update_normals();
//    m.release_face_normals();
//}

   // Merge merge = Merge(&m);

   //��������Y������ص�
    int overlap = 0;
    //����obj����ƽ�Ƶ���������
    double move_dist = 0.980103 / 2;
    //��ȡobj�����β��
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
        std::cout << "Ѱ�ұ߽��ʧ�ܣ�����" << std::endl;
    }
    if (!merge.FindBoundary(&mesh.m_mesh, head_filename_1, head_1, tail_filename_1, tail_1, overlap, edgefile_1, dst_obj, merge_boundary_1))
    {
        std::cout << "Ѱ�ұ߽��ʧ�ܣ�����" << std::endl;
    }
    merge.add_face(mesh.m_mesh,merge_boundary_0, merge_boundary_1, head_0, tail_0, head_1, tail_1,move_dist,dst_obj);

    std::string objout = "D:/MyLearning/Learning_Materials/run/mergeobj/output1/test_output.obj";
    mesh.write_obj_file(objout);
}