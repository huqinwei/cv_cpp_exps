#include<iostream>

#include "pair.h"
#include "interview.h"
#include <tuple>
using namespace std;

int main()
{

	s_A sa = Get_s_A();
	s_A sa2 = sa;
	s_A* p_sa = new s_A();

	std_pair_test();

	cout << "hello" << endl;
	tuple<int, char, int, double> first(10, 'x', 100, 2.2);
	cout << get<0>(first) << endl;
	cout << get<1>(first) << endl;
	cout << get<2>(first) << endl;
	cout << get<3>(first) << endl;


	
	//cout << first[0] << endl;
	//C2676	二进制“[”:“std::tuple<int, char, int, double>”不定义该运算符或到预定义运算符可接收的类型的转换	CPP_Basic	D : \alk_workspace\exp_code_collection_root_path\cv_cpp_exps\CPP_Demos\CPP_Basic\main.cpp	16


	return 0;

}
