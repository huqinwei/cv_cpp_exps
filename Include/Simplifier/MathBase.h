#ifndef __MATHBASE_H__
#define __MATHBASE_H__

//#include <math.h>
#include <cmath>
#include <cstddef>
#include <assert.h>
#include <float.h>

const double pi = 3.1415926535897932384626433832795;

inline float Deg2Rad(float deg)
{
	return float(deg / 180.0 * pi);
}

inline float Rad2Deg(float rad)
{
	return float(rad * 180.0 / pi);
}

inline double Deg2Rad(double deg)
{
	return double(deg / 180.0 * pi);
}

inline double Rad2Deg(double rad)
{
	return double(rad * 180.0 / pi);
}

inline float squared(float x)
{
	return x * x;
}

template <typename T> inline T mod(T a, T b)
{
	const T m = a % b;

	return m >= T(0) ? m : b + m;
}

inline bool SameSign(float x, float y)
{
	return (x * y) >= 0.0f;
}

inline float cot(float rad)
{
	return 1.0f / ::tan(rad);
}

template<class T> inline T clamp(T val, T min_val, T max_val)
{
	if (val < min_val)
		return min_val;
	else if (val > max_val)
		return max_val;
	else
		return val;
}

/*extern void ErrorBox( const char *str , int line , const char *file_base_name , const char *funcname );

#define ErrorMessage( str ) ::ErrorBox( #str , __LINE__ , __FILE__ , __FUNCSIG__ );

#define Asserte( cmp , str ) if(!(cmp)) { ErrorMessage(str); }

#ifdef _DEBUG
#define DebugAsserte( cmp , str ) Asserte( cmp , str )
#else
#define DebugAsserte( cmp , str )
#endif*/

typedef unsigned int uint;

#endif //__MATHBASE_H__