#include "vector_demo.h"



//���ʱ���㹻
vector<int>* new_vec()
{
	vector<int>* pvec = new vector<int>;
	pvec->push_back(10);
	pvec->emplace_back(11);
	pvec->emplace_back(12);
	int result = pvec->back();
	cout << result << endl;
	return pvec;
}

/*
* *�ο�д��
// CLASS TEMPLATE vector
template <class _Ty, class _Alloc = allocator<_Ty>>
class vector { // varying size array of values
*/
/*
* *TODO:Ѱ����ȷд�����������д��
template <typename T>
vector<T>* new_vec_templates(T) {//Tû��ʵ���ô���ֻ��Ϊ���趨���ͣ��Ƿ��и��ŵ�д��
	vector<T>* pvec = new vector<T>;
	pvec->push_back(10);
	pvec->emplace_back(11);
	pvec->emplace_back(12);
	int result = pvec->back();
	cout << result << endl;
	return pvec;
}
*/
void vector_lifetime_test()
{
	vector<int>* pvec = new_vec();
// 	double d1;
// 	string s1;
// 	vector<double>* pvec_d = new_vec_templates(d1);
// 	vector<string>* pvec_s = new_vec_templates(s1);
	int result = pvec->back();
	cout << result << endl;


	vector<int>* pvec2;
	if (1)
	{
		vector<int> vec_test;
		vec_test.emplace_back(19);
		vec_test.emplace_back(22);
		vec_test.emplace_back(33);
		int result = vec_test.back();
		cout << result << endl;
		pvec2 = &vec_test;
		result = pvec2->back();
		cout << result << endl;
	}
	result = pvec2->back();//�������Ϊvec�������ڵ���
	cout << result << endl;
}
