#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <Constants.h>


extern "C" void AI3D_API hi();

class AI3D_API Sample
{
public:
	Sample();
	virtual ~Sample();
	void hi();
};


#endif // _SAMPLE_H_