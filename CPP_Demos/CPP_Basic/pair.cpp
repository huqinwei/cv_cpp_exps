#pragma once
//
#include "boost/align.hpp"
#include "boost/cast.hpp"

#include <iostream>

#include "pair.h"


//pair是一个模板结构体，也不一定就是组合map用的（但是在map的头文件内）
// template<pair> make_pair(T1 a, T2 b)
// {
//     return pair(a, b);
// }

void std_pair_test()
{
	int i = 10;
	double d = 4.4;
	//尝试三种形式接收make_pair返回值
	auto pair_a = std::make_pair(i, d);
	std::pair<int, double> pair_b;
	pair_b = std::make_pair(i, d);
	std::cout << pair_a.first << std::endl;
	std::cout << pair_a.second << std::endl;
	std::pair<int, double> pair_c = std::make_pair(i, d);
	std::cout << pair_b.first << std::endl;
	std::cout << pair_b.second << std::endl;
	std::cout << pair_c.first << std::endl;
	std::cout << pair_c.second << std::endl;
	////////////////////////////////////////////////////////////////////////

	std::pair<int, double> p1(1, 1);//1可以当作double 1.0
	std::cout << p1.first << std::endl;
	std::cout << p1.second << std::endl;
	std::pair<int, double> p2(1.1, 1.1);//1.1可以强转int 1
	std::cout << p2.first << std::endl;
	std::cout << p2.second << std::endl;
	std::pair<int, double> p3(1, 1.1);
	std::cout << p3.first << std::endl;
	std::cout << p3.second << std::endl;
}
