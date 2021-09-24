#include"Merge/FindBoundary.h"

template<class MeshT>
bool FindBoundary(MeshT* _mesh, const std::string& head_filename, std::vector<typename MeshT::VertexHandle>& head, const std::string& tail_filename, std::vector<typename MeshT::VertexHandle>& tail,
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
