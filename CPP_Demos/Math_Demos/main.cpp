
#include <windows.h>

/*#include"Types.h"*/



//这是求次幂的，但是写的也一般般，result是1.0，还乘以base，图什么？return base就好了。
float POWBASE(int base, int exp) {
	float result(1.0);
	if (exp > 0) {
		while (exp) {
			if (exp & 1)
				result *= base;
			exp >>= 1;
			base *= base;
		}
	}
	if (exp < 0) {
		float temp_base = 1.0f / base;
		int temp_exp = -exp;
		while (temp_exp) {
			if (temp_exp & 1) {
				result *= temp_base;
			}
			temp_exp >>= 1;
			temp_base *= temp_base;
		}
	}
	return result;
}
#define PI 3.1415926
#include <math.h>
#include <iostream>


//求点到三角形的距离
double distance_point_to_triangle() {
	//https://blog.csdn.net/Xu_Haocan/article/details/78209628内涵三个链接


	//第一步，三角形平面公式

	//三角形平面法向量

	//点P投影到平面，P'

	//点投影是否在三角形内？
	//重心法？
	//inlie测试？

	//如果不在三角形内，要算一个最近的边的距离（当然应该不是投影点到边的距离，是原点P


}


int main()
{
	//     int result_size = 0;
	//     result_size = POWI((int)2, CEIL2INT(LOGN((float)total_size[0]) / LOGN(2.f)));
	std::cout << 180 * 0.02 / PI << std::endl;
	std::cout << cos(0.02) << std::endl;
	std::cout << acos(0.9998) << std::endl;

	std::cout << cos(PI / 6) << std::endl;

	std::cout << sqrt(3) * 0.5 << std::endl;


	std::cout << cos(PI / 4) << std::endl;
	std::cout << cos(PI / 3) << std::endl;
	std::cout << cos(PI / 1) << std::endl;


	//POWBASE(2,2);


	system("pause");
	return 0;
}











