#include "Simplifier\plane.h"
namespace REMTNS {
	void Plane::NormalizeNormal()
	{
		float len = N.Length();
		D /= len;
		N /= len;
	}
}
