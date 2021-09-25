/*
	*Entrance to tile node segmentation progress.
	*Build quadtree by node segmentation.
	*author:hqw
*/

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include "ScopeGuard.h"


#define TEST_NODE_SEGMENTATION

int main()
{


	struct_a* sa = new struct_a;
	if (1) {
		//struct_a * sa = new struct_a;

		DEFER(
			delete sa;
		sa = nullptr;
		);

	}
	system("pause");


	
	//std::cout << hello << std::endl;
	return 0;

}
