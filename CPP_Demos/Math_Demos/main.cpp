
#include <windows.h>

/*#include"Types.h"*/



//��������ݵģ�����д��Ҳһ��㣬result��1.0��������base��ͼʲô��return base�ͺ��ˡ�
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


//��㵽�����εľ���
double distance_point_to_triangle() {
	//https://blog.csdn.net/Xu_Haocan/article/details/78209628�ں���������


	//��һ����������ƽ�湫ʽ

	//������ƽ�淨����

	//��PͶӰ��ƽ�棬P'

	//��ͶӰ�Ƿ����������ڣ�
	//���ķ���
	//inlie���ԣ�

	//��������������ڣ�Ҫ��һ������ıߵľ��루��ȻӦ�ò���ͶӰ�㵽�ߵľ��룬��ԭ��P


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











