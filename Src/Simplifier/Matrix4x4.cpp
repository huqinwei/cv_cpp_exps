#include "Simplifier/Matrix4x4.h"
namespace REMTNS{
Matrix4x4::Matrix4x4()
	{
	this->SetToIdentity();
	}

Matrix4x4::Matrix4x4( const Matrix4x4 &other )
	{
	this->Set( &(other.m[0][0]) );
	}

Matrix4x4::Matrix4x4( const float *pMtx )
	{
	this->Set( pMtx );
	}

void Matrix4x4::Set( const float *pMtx )
	{
	memcpy_s( m , sizeof( float ) * 16,  pMtx , sizeof(float)*16 );
	}

Matrix4x4 & Matrix4x4::Transpose()
{
	Matrix4x4 tmp( *this );

	for( int j=0; j<4; ++j )
	{
		for( int i=0; i<4; ++i )
		{
			this->m[j][i] = tmp.m[i][j];
		}
	}
	return *this;
}

Matrix4x4 Matrix4x4::GetTranspose() const 
{
	Matrix4x4 mat = *this;
	mat.Transpose();
	return mat;
}
	
Matrix4x4 Matrix4x4::MultiplyMatrix( const Matrix4x4 &other ) const
	{
	Matrix4x4 tmp;

	for(int i = 0; i < 4; i++) 
    	{
		for(int k = 0; k < 4; k++) 
			{
			tmp.m[i][k] =	m[i][0] * other.m[0][k] +
					        m[i][1] * other.m[1][k] +
							m[i][2] * other.m[2][k] +
							m[i][3] * other.m[3][k];
			}
		}

	return tmp;
	}

void Matrix4x4::SetToZero()
	{
	memset( m , 0 , sizeof(float)*16 );
	}

void Matrix4x4::SetToIdentity()
	{
	this->SetToZero();
	this->m[0][0] = 1.f;
	this->m[1][1] = 1.f;
	this->m[2][2] = 1.f;
	this->m[3][3] = 1.f;
	}

void Matrix4x4::SetToRotationX( float angle )
	{
	this->SetToFreeAxisRotation( Vector3D(1,0,0) , angle );
	}

void Matrix4x4::SetToRotationY( float angle )
	{
	this->SetToFreeAxisRotation( Vector3D(0,1,0) , angle );
	}

void Matrix4x4::SetToRotationZ( float angle )
	{
	this->SetToFreeAxisRotation( Vector3D(0,0,1) , angle );
	}

void Matrix4x4::SetToFreeAxisRotation( const Vector3D &axis , float angle )
	{
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	
	this->SetToIdentity();
	if (angle == 0.0 || (x == 0.0 && y == 0.0 && z == 0.0)) 
    	{
		return;
    	}

	// convert degrees to radians
	angle = Deg2Rad(angle);

	// make a normalized quaternion
	float w = cosf(float(0.5)*angle);
	float f = sinf(float(0.5)*angle)/sqrtf(x*x+y*y+z*z);
	x *= f;
	y *= f; 
	z *= f;

	// convert the quaternion to a matrix
	float ww = w*w;
	float wx = w*x;
	float wy = w*y;
	float wz = w*z;

	float xx = x*x;
	float yy = y*y;
	float zz = z*z;

	float xy = x*y;
	float xz = x*z;
	float yz = y*z;

	float s = ww - xx - yy - zz;

	this->m[0][0] = xx*2 + s;
	this->m[1][0] = (xy + wz)*2;
	this->m[2][0] = (xz - wy)*2;

	this->m[0][1] = (xy - wz)*2;
	this->m[1][1] = yy*2 + s;
	this->m[2][1] = (yz + wx)*2;

	this->m[0][2] = (xz + wy)*2;
	this->m[1][2] = (yz - wx)*2;
	this->m[2][2] = zz*2 + s;
	}

void Matrix4x4::SetToTranslation( const Vector3D &translation )
	{
	this->SetToIdentity();
	this->m[0][3] = translation.x;
	this->m[1][3] = translation.y;
	this->m[2][3] = translation.z;
	}

void Matrix4x4::SetToProjection( float l , float r , float b , float t , float n , float f )
	{
	this->m[0][0] = (2.f*n)/(r-l);
	this->m[0][1] = 0;
	this->m[0][2] = (r+l)/(r-l);
	this->m[0][3] = 0;
	this->m[1][0] = 0;
	this->m[1][1] = (2.f*n)/(t-b);
	this->m[1][2] = (t+b)/(t-b);
	this->m[1][3] = 0;
	this->m[2][0] = 0;
	this->m[2][1] = 0;
	this->m[2][2] = -(f+n)/(f-n);
	this->m[2][3] = -(2.f*n*f)/(f-n);
	this->m[3][0] = 0;
	this->m[3][1] = 0;
	this->m[3][2] = -1;
	this->m[3][3] = 0;
	}

//////////////////////////////////////////////////////////////////////////////////

inline float	_Determinant2x2(float a,	float b,	float c,	float d)	
	{
	return (a	*	d	-	b	*	c);
	};

inline float	_Determinant3x3(float a1, float	a2,	float a3, 
								float b1, float	b2,	float b3, 
								float c1, float	c2,	float c3)
	{
	return ( a1	*	_Determinant2x2( b2, b3, c2, c3	)
		 - b1	*	_Determinant2x2( a2, a3, c2, c3	)
		 + c1	*	_Determinant2x2( a2, a3, b2, b3	)	);
	}

static void _Adjoint4x4(const float mtxin[4][4], float mtxout[4][4])
	{
	float a1, a2, a3, a4, b1, b2, b3, b4;
	float c1, c2, c3, c4, d1, d2, d3, d4;

	a1 = mtxin[0][0];	b1 = mtxin[0][1];	
	c1 = mtxin[0][2];	d1 = mtxin[0][3];

	a2 = mtxin[1][0];	b2 = mtxin[1][1];	
	c2 = mtxin[1][2];	d2 = mtxin[1][3];

	a3 = mtxin[2][0];	b3 = mtxin[2][1];
	c3 = mtxin[2][2];	d3 = mtxin[2][3];

	a4 = mtxin[3][0];	b4 = mtxin[3][1];	
	c4 = mtxin[3][2];	d4 = mtxin[3][3];


	// row column	labeling reversed	since	we transpose rows	&	columns
	mtxout[0][0]	=		
		_Determinant3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
	mtxout[1][0]	=	
		-	_Determinant3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
	mtxout[2][0]	=		
		_Determinant3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
	mtxout[3][0]	=	
		-	_Determinant3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

	mtxout[0][1]	=	
		-	_Determinant3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
	mtxout[1][1]	=		
		_Determinant3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
	mtxout[2][1]	=	
		-	_Determinant3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
	mtxout[3][1]	=		
		_Determinant3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);
				
	mtxout[0][2]	=		
		_Determinant3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
	mtxout[1][2]	=	
		-	_Determinant3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
	mtxout[2][2]	=		
		_Determinant3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
	mtxout[3][2]	=	
		-	_Determinant3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);
				
	mtxout[0][3]	=	
		-	_Determinant3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
	mtxout[1][3]	=		
		_Determinant3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
	mtxout[2][3]	=	
		-	_Determinant3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
	mtxout[3][3]	=		
		_Determinant3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
	}

float _Determinant4x4( const float mtxin[4][4] )
	{
	float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

	a1 = mtxin[0][0];	b1 = mtxin[0][1];	
	c1 = mtxin[0][2];	d1 = mtxin[0][3];

	a2 = mtxin[1][0];	b2 = mtxin[1][1];	
	c2 = mtxin[1][2];	d2 = mtxin[1][3];

	a3 = mtxin[2][0];	b3 = mtxin[2][1];	
	c3 = mtxin[2][2];	d3 = mtxin[2][3];

	a4 = mtxin[3][0];	b4 = mtxin[3][1];	
	c4 = mtxin[3][2];	d4 = mtxin[3][3];

	return a1	*	_Determinant3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
		 - b1	*	_Determinant3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
		 + c1	*	_Determinant3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
		 - d1	*	_Determinant3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
		}


static void _Invert4x4(const float mtxin[4][4], float mtxout[4][4])
	{
	int	i, j;
	float det;

	det	=	_Determinant4x4(mtxin);
	//Asserte( det !=	0.0 , "Singular	matrix,	no inverse exists!" );

	_Adjoint4x4(mtxin,	mtxout);

	for	(i=0;	i<4; i++)
		{
		for(j=0; j<4;	j++)
			{
			mtxout[i][j] = mtxout[i][j]	/	det;
			}
		}
	}

Matrix4x4 & Matrix4x4::Invert()
	{
	Matrix4x4 tmp(*this);
	_Invert4x4(tmp.m,this->m);

	return *this;
	}

Matrix4x4 Matrix4x4::GetInverse() const
{
	Matrix4x4 tmp(*this);
	tmp.Invert();
	return tmp;
}

void Matrix4x4::OrthoNormalize()
	{
	Vector3D x = this->GetXAxis();
	Vector3D y = this->GetYAxis();
	Vector3D z;

	x.Normalize();
	y.Normalize();
	z = Cross( x , y );
	z.Normalize();
	x = Cross( y , z );
	y = Cross( z , x );

	m[0][0] = x[0];
	m[1][0] = x[1];
	m[2][0] = x[2];

	m[0][1] = y[0];
	m[1][1] = y[1];
	m[2][1] = y[2];

	m[0][2] = z[0];
	m[1][2] = z[1];
	m[2][2] = z[2];
	}
}