//|-------------------------------------------------------------------------
//| File:        matrix4.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef matrix4HPP
#define matrix4HPP
//----------------------------------------------------------------------------

#include "vector3d.h"
#include "plane3d.h"
#include "aabbox3d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
//! Matrix4 is mainly used by the Irrlicht engine for doing transformations.
//! The matrix is a D3D style matrix, row major with translations in the 4th row.
class __MY_CORE_LIB_API__ matrix4
{
public:

    //! Constructors
    matrix4();
	//! Constructors
	matrix4(
		f32 m0,  f32 m1,  f32 m2,  f32 m3, 
		f32 m4,  f32 m5,  f32 m6,  f32 m7, 
		f32 m8,  f32 m9,  f32 m10, f32 m11, 
		f32 m12, f32 m13, f32 m14, f32 m15);
	matrix4(const f32 *data);

    //! Simple operator for directly accessing every element of the matrix.
    f32& operator () (s32 col, s32 row)
	{ return M[row * 4 + col]; }

    //! Simple operator for directly accessing every element of the matrix.
    const f32& operator () (s32 col, s32 row) const
	{  return M[row * 4 + col]; }

    //! Sets this matrix equal to the other matrix.
    matrix4& operator = (const matrix4 &other);

	//! Returns true if other matrix is greater then matrix.
    bool operator < (const matrix4 &other) const;

    //! Returns true if other matrix is equal to this matrix.
    bool operator == (const matrix4 &other) const;

    //! Returns true if other matrix is not equal to this matrix.
    bool operator != (const matrix4 &other) const;

    //! Multiply by another matrix.
    matrix4& operator *= (const matrix4& other);

    //! Multiply by another matrix.
    matrix4 operator * (const matrix4& other) const;

    //! Multiply by number.
    matrix4& operator *= (const f32 number);

    //! Multiply by number.
    matrix4 operator * (const f32 number) const;

	//! Simple operator for linearly accessing every element of the matrix.
	f32& operator [] (u32 index) { return M[index]; }

	//! Simple operator for linearly accessing every element of the matrix.
	const f32& operator [] (u32 index) const { return M[index]; }

    //! Set matrix to identity. 
	void makeIdentity();

    //! Returns true if the matrix is the identity matrix
    bool isIdentity() const;

    //! Set the translation of the current matrix. Will erase any previous values.
    void setTranslation( const vector3df& translation );            

    //! Gets the current translation
    vector3df getTranslation() const;

    //! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
    void setRotationRadians( const vector3df& rotation );           

    //! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
    void setRotationDegrees( const vector3df& rotation );           

    //! Returns the rotation, as set by setRotationDegrees()
    vector3df getRotationDegrees() const;  

    //! Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
    void setInverseRotationRadians( const vector3df& rotation );    

    //! Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
    void setInverseRotationDegrees( const vector3df& rotation );    

    //! Set Scale
    void setScale(const vector3df& scale);
    
    //! Translate a vector by the inverse of the translation part of this matrix.
    void inverseTranslateVect( vector3df& vect) const;         

    //! Rotate a vector by the inverse of the rotation part of this matrix.
    void inverseRotateVect(vector3df& vect) const;                

    //! Rotate a vector by the rotation part of this matrix.
    void rotateVect(vector3df& vect) const;               

    //! Transforms the vector by this matrix
    void transformVect(vector3df& vect) const;

    //! Transforms input vector by this matrix and stores result in output vector 
    void transformVect(const vector3df& in, vector3df& out) const;

    //! Translate a vector by the translation part of this matrix.
    void translateVect(vector3df& vect)  const;

    //! Transforms a plane by this matrix
    void transformPlane(plane3d<f32> &plane) const;

    //! Transforms a plane by this matrix
    void transformPlane(const plane3d<f32> &in, plane3d<f32> &out) const;

    //! Transforms a axis aligned bounding box
    void transformBox(aabbox3d<f32> &box) const;

    //! Multiplies this matrix by a 1x4 matrix
    void multiplyWith1x4Matrix(f32* matrix) const;

    //! Calculates inverse of matrix. Slow.
    //! \return Returns false if there is no inverse matrix.
    bool makeInversed();

    //! returns the inversed matrix of this one
    //! \param Target, where result matrix is written to.
    //! \return Returns false if there is no inverse matrix.
    bool getInversed(matrix4& out) const;

    //! returns the inversed matrix of this one
    matrix4 getInversed() const;

	//! return z near value from projection matrix
	f32 getZNear() const;

	//! return z far value from projection matrix
	f32 getZFar() const;

    //! Builds a right-handed perspective projection matrix based on a field of view
    void buildProjectionMatrixPerspectiveRH(f32 fov, f32 aspect, f32 zn, f32 zf);

    //! Builds a left-handed perspective projection matrix based on a field of view
    void buildProjectionMatrixPerspectiveLH(f32 fov, f32 aspect, f32 zn, f32 zf);

    //! Builds a left-handed orthogonal projection matrix.
    void buildProjectionMatrixOrthoLH(f32 width, f32 height, f32 zn, f32 zf);

    //! Builds a right-handed orthogonal projection matrix.
    void buildProjectionMatrixOrthoRH(f32 width, f32 height, f32 zn, f32 zf);

    //! Builds a left-handed look-at matrix.
    void buildViewMatrixLH(const vector3df& pos, const vector3df& tgt_pos, const vector3df& up_vec);

    //! Builds a right-handed look-at matrix.
    void buildViewMatrixRH(const vector3df& pos, const vector3df& tgt_pos, const vector3df& up_vec);
	
    //! Builds a matrix that flattens geometry into a plane.
    //! \param light: light source
    //! \param plane: plane into which the geometry if flattened into
    //! \param point: value between 0 and 1, describing the light source. 
    //! If this is 1, it is a point light, if it is 0, it is a directional light.
    void buildShadowMatrix(vector3df light, plane3df plane, f32 point=1.0f);
    
    //! creates a new matrix as interpolated matrix from to other ones.
    //! \param b: other matrix to interpolate with
    //! \param time: Must be a value between 0 and 1.
    matrix4 interpolate(matrix4& b, f32 time) const;

	//! Build a reflection matrix for the passed in plane.
    void buildReflectionMatrix(const plane3d<f32> &p);

	//! Build a rotation matrix around vector by some angle (degrees)
	void buildRotationDegreesMatrix(f32 theta, core::vector3df &V);

	//! Build a rotation matrix around vector by some angle (radians)
	void buildRotationRadiansMatrix(f32 theta, core::vector3df &V);

	//! Converting to D3D texture matrix
	f32* toD3DTextureMatrix() const;

	//! Converting to OGL texture matrix
	f32* toOGLTextureMatrix() const;

	//! Returns pointer to internal array
	const f32* pointer() const { return M; }

	//! Returns pointer to internal array
	f32* pointer() { return M; }

	//! calculates matrix determinant
	f32 determinant() const;

	//! returns transposed matrix
	matrix4 getTransposed() const;

	//! returns transposed matrix to a plain 4x4 float matrix
	void getTransposed( matrix4& dest ) const;

	//! setting matrix from data array
	void set(const f32 *data)
	{ memcpy(M, data, sizeof(M)); }

private:

    //! Matrix data, stored in row-major order
	// 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    f32 M[16];

	mutable bool identity;
};

//----------------------------------------------------------------------------

MY_FORCEINLINE matrix4::matrix4()
{
	identity = false;
	makeIdentity();
}

//----------------------------------------------------------------------------

MY_FORCEINLINE matrix4::matrix4(
	f32 m0,  f32 m1,  f32 m2,  f32 m3, 
	f32 m4,  f32 m5,  f32 m6,  f32 m7, 
	f32 m8,  f32 m9,  f32 m10, f32 m11, 
	f32 m12, f32 m13, f32 m14, f32 m15)
{
    M[0]  = m0;
	M[1]  = m1;
	M[2]  = m2;
	M[3]  = m3;
	M[4]  = m4;
	M[5]  = m5;
	M[6]  = m6;
	M[7]  = m7;
	M[8]  = m8;
	M[9]  = m9;
	M[10] = m10;
	M[11] = m11;
	M[12] = m12;
	M[13] = m13;
	M[14] = m14;
	M[15] = m15;

	identity = false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE matrix4::matrix4(const f32 *data)
{
    set(data);
	identity = false;
}

//----------------------------------------------------------------------------

//! multiply by another matrix
MY_FORCEINLINE matrix4& matrix4::operator *= ( const matrix4& other )
{
	static const s32 size16 = 16*sizeof(f32);

    static f32 newMatrix[16];
    const f32 *m1 = M, *m2 = other.M;

    newMatrix[0]  = m1[0]*m2[0]  + m1[4]*m2[1]  + m1[8] *m2[2]  + m1[12]*m2[3];
    newMatrix[1]  = m1[1]*m2[0]  + m1[5]*m2[1]  + m1[9] *m2[2]  + m1[13]*m2[3];
    newMatrix[2]  = m1[2]*m2[0]  + m1[6]*m2[1]  + m1[10]*m2[2]  + m1[14]*m2[3];
    newMatrix[3]  = m1[3]*m2[0]  + m1[7]*m2[1]  + m1[11]*m2[2]  + m1[15]*m2[3];
    
    newMatrix[4]  = m1[0]*m2[4]  + m1[4]*m2[5]  + m1[8] *m2[6]  + m1[12]*m2[7];
    newMatrix[5]  = m1[1]*m2[4]  + m1[5]*m2[5]  + m1[9] *m2[6]  + m1[13]*m2[7];
    newMatrix[6]  = m1[2]*m2[4]  + m1[6]*m2[5]  + m1[10]*m2[6]  + m1[14]*m2[7];
    newMatrix[7]  = m1[3]*m2[4]  + m1[7]*m2[5]  + m1[11]*m2[6]  + m1[15]*m2[7];
    
    newMatrix[8]  = m1[0]*m2[8]  + m1[4]*m2[9]  + m1[8] *m2[10] + m1[12]*m2[11];
    newMatrix[9]  = m1[1]*m2[8]  + m1[5]*m2[9]  + m1[9] *m2[10] + m1[13]*m2[11];
    newMatrix[10] = m1[2]*m2[8]  + m1[6]*m2[9]  + m1[10]*m2[10] + m1[14]*m2[11];
    newMatrix[11] = m1[3]*m2[8]  + m1[7]*m2[9]  + m1[11]*m2[10] + m1[15]*m2[11];
    
    newMatrix[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8] *m2[14] + m1[12]*m2[15];
    newMatrix[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9] *m2[14] + m1[13]*m2[15];
    newMatrix[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
    newMatrix[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
    
	memcpy(M, newMatrix, size16);

	identity = false;

    return *this;
}

//----------------------------------------------------------------------------

//! multiply by another matrix
MY_FORCEINLINE matrix4 matrix4::operator * ( const matrix4& other ) const
{
    static matrix4 tmtrx;
    const f32 *m1 = M, *m2 = other.M;
    f32 *m3 = tmtrx.M;

    m3[0]  = m1[0]*m2[0]  + m1[4]*m2[1]  + m1[8] *m2[2]  + m1[12]*m2[3];
    m3[1]  = m1[1]*m2[0]  + m1[5]*m2[1]  + m1[9] *m2[2]  + m1[13]*m2[3];
    m3[2]  = m1[2]*m2[0]  + m1[6]*m2[1]  + m1[10]*m2[2]  + m1[14]*m2[3];
    m3[3]  = m1[3]*m2[0]  + m1[7]*m2[1]  + m1[11]*m2[2]  + m1[15]*m2[3];
    
    m3[4]  = m1[0]*m2[4]  + m1[4]*m2[5]  + m1[8] *m2[6]  + m1[12]*m2[7];
    m3[5]  = m1[1]*m2[4]  + m1[5]*m2[5]  + m1[9] *m2[6]  + m1[13]*m2[7];
    m3[6]  = m1[2]*m2[4]  + m1[6]*m2[5]  + m1[10]*m2[6]  + m1[14]*m2[7];
    m3[7]  = m1[3]*m2[4]  + m1[7]*m2[5]  + m1[11]*m2[6]  + m1[15]*m2[7];
    
    m3[8]  = m1[0]*m2[8]  + m1[4]*m2[9]  + m1[8] *m2[10] + m1[12]*m2[11];
    m3[9]  = m1[1]*m2[8]  + m1[5]*m2[9]  + m1[9] *m2[10] + m1[13]*m2[11];
    m3[10] = m1[2]*m2[8]  + m1[6]*m2[9]  + m1[10]*m2[10] + m1[14]*m2[11];
    m3[11] = m1[3]*m2[8]  + m1[7]*m2[9]  + m1[11]*m2[10] + m1[15]*m2[11];
    
    m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8] *m2[14] + m1[12]*m2[15];
    m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9] *m2[14] + m1[13]*m2[15];
    m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
    m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

	identity = false;

    return tmtrx;
}

//----------------------------------------------------------------------------

//! Multiply by number.
MY_FORCEINLINE matrix4& matrix4::operator *= ( const f32 number )
{
    M[0]  *= number;
    M[1]  *= number;
    M[2]  *= number;
    M[3]  *= number;
   
    M[4]  *= number;
    M[5]  *= number;
    M[6]  *= number;
    M[7]  *= number;
    
    M[8]  *= number;
    M[9]  *= number;
    M[10] *= number;
    M[11] *= number;
    
    M[12] *= number;
    M[13] *= number;
    M[14] *= number;
    M[15] *= number;
    
	identity = false;

    return *this;
}

//----------------------------------------------------------------------------

//! Multiply by number.
MY_FORCEINLINE matrix4 matrix4::operator * ( const f32 number ) const
{
	static matrix4 tmtrx;

    tmtrx.M[0]  = number * M[0];
    tmtrx.M[1]  = number * M[1];
    tmtrx.M[2]  = number * M[2];
    tmtrx.M[3]  = number * M[3];
    
    tmtrx.M[4]  = number * M[4];
    tmtrx.M[5]  = number * M[5];
    tmtrx.M[6]  = number * M[6];
    tmtrx.M[7]  = number * M[7];
    
    tmtrx.M[8]  = number * M[8];
    tmtrx.M[9]  = number * M[9];
    tmtrx.M[10] = number * M[10];
    tmtrx.M[11] = number * M[11];
    
    tmtrx.M[12] = number * M[12];
    tmtrx.M[13] = number * M[13];
    tmtrx.M[14] = number * M[14];
    tmtrx.M[15] = number * M[15];

	identity = false;

    return tmtrx;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::makeInversed ()
{
    static matrix4 temp;

	identity = false;

    if (getInversed(temp))
    {
        *this = temp;
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE matrix4& matrix4::operator = ( const matrix4 &other )
{
	static const s32 size16 = 16*sizeof(f32);

	memcpy(M, other.M, size16);

	identity = false;

    return *this;
}
//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::operator < ( const matrix4 &other ) const
{
    for (s32 i = 0; i < 16; ++i)
        if (M[i] != other.M[i])
            return M[i] < other.M[i];
    return false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::operator == ( const matrix4 &other ) const
{
	if ( identity && identity == other.identity )
		return true;

    for (s32 i = 0; i < 16; ++i)
        if (M[i] != other.M[i])
            return false;

    return true;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::operator != ( const matrix4 &other ) const
{ return !(*this == other); }

//----------------------------------------------------------------------------

//! Set matrix to identity. 
MY_FORCEINLINE void matrix4::makeIdentity ()
{
	static const s32 size16 = 16*sizeof(f32);

	if (identity)
		return;

	memset(M, 0, size16);
	M[0] = M[5] = M[10] = M[15] = 1;

	identity = true;
}

//----------------------------------------------------------------------------

//! calculates matrix determinant
MY_FORCEINLINE f32 matrix4::determinant () const
{
	const matrix4 &m = *this;

	f32 s1 = 
		(m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) - 
		(m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3));

	f32 s2 = 
		(m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)) + 
		(m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3));

	f32 s3 = 
		(m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + 
		(m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));

	return s1 + s2 - s3;
}

//----------------------------------------------------------------------------

//! returns transposed matrix
MY_FORCEINLINE matrix4 matrix4::getTransposed () const
{
	static matrix4 t;
	getTransposed ( t );
	return t;
}

//----------------------------------------------------------------------------

//! returns transposed matrix
MY_FORCEINLINE void matrix4::getTransposed ( matrix4& o ) const
{
	o[ 0] = M[ 0];
	o[ 1] = M[ 4];
	o[ 2] = M[ 8];
	o[ 3] = M[12];

	o[ 4] = M[ 1];
	o[ 5] = M[ 5];
	o[ 6] = M[ 9];
	o[ 7] = M[13];

	o[ 8] = M[ 2];
	o[ 9] = M[ 6];
	o[10] = M[10];
	o[11] = M[14];

	o[12] = M[ 3];
	o[13] = M[ 7];
	o[14] = M[11];
	o[15] = M[15];
}

MY_FORCEINLINE vector3df matrix4::getTranslation() const
{
    return vector3df(M[12], M[13], M[14]);
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setTranslation( const vector3df& translation )
{
    M[12] = translation.X;
    M[13] = translation.Y;
    M[14] = translation.Z;

	identity = false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setScale( const vector3df& scale )
{
    M[0] = scale.X;
    M[5] = scale.Y;
    M[10] = scale.Z;

	identity = false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setRotationDegrees( const vector3df& rotation )
{
    setRotationRadians( rotation * (f32)DEG2RAD );
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setInverseRotationDegrees( const vector3df& rotation )
{
    setInverseRotationRadians( rotation * (f32)DEG2RAD );
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setRotationRadians( const vector3df& rotation )
{
    f64 cr = cos( rotation.X );
    f64 sr = sin( rotation.X );
    f64 cp = cos( rotation.Y );
    f64 sp = sin( rotation.Y );
    f64 cy = cos( rotation.Z );
    f64 sy = sin( rotation.Z );

    M[0] = (f32)( cp*cy );
    M[1] = (f32)( cp*sy );
    M[2] = (f32)( -sp );

    f64 srsp = sr*sp;
    f64 crsp = cr*sp;

    M[4] = (f32)( srsp*cy-cr*sy );
    M[5] = (f32)( srsp*sy+cr*cy );
    M[6] = (f32)( sr*cp );

    M[8] = (f32)( crsp*cy+sr*sy );
    M[9] = (f32)( crsp*sy-sr*cy );
    M[10] = (f32)( cr*cp );

	identity = false;
}

//----------------------------------------------------------------------------

//! Returns the rotation, as set by setRotationDegrees(). This code was sent
//! in by Chev.
MY_FORCEINLINE vector3df matrix4::getRotationDegrees() const
{
    const matrix4 &mat = *this;
    
    f32 Y = -asinf(mat(2,0)); 
    f32 D = Y;
    f32 C = cosf(Y); 
    Y *= 180.000f/PI; 

    f32 rotx, roty, X, Z;

    if (fabs(Y)>0.0005f) 
    {
        rotx = mat(2,2) / C; 
        roty = mat(2,1)  / C; 
        X = atan2f( roty, rotx ) * (f32)RAD2DEG; 
        rotx =  mat(0,0) / C; 
        roty = mat(1,0) / C; 
        Z = atan2f( roty, rotx ) * (f32)RAD2DEG;
    } 
    else 
    {
        X  = 0.0f; 
        rotx = -mat(1,1); 
        roty = mat(1,0); 
        Z  = atan2f( roty, rotx ) * (f32)RAD2DEG; 
    } 

    if (fabs(X)>=360.00f) X=0.00f; 
    if (fabs(Y)>=360.00f) Y=0.00f; 
    if (fabs(Z)>=360.00f) Z=0.00f; 

    // fix values that get below zero 
    // before it would set (!) values to 360 
    // that where above 360: 
    if (X < 0.00) X += 360.00; 
    if (Y < 0.00) Y += 360.00; 
    if (Z < 0.00) Z += 360.00;

	identity = false;

    return vector3df(X,Y,Z); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::setInverseRotationRadians( const vector3df& rotation )
{
    f64 cr = cos( rotation.X );
    f64 sr = sin( rotation.X );
    f64 cp = cos( rotation.Y );
    f64 sp = sin( rotation.Y );
    f64 cy = cos( rotation.Z );
    f64 sy = sin( rotation.Z );

    M[0] = (f32)( cp*cy );
    M[4] = (f32)( cp*sy );
    M[8] = (f32)( -sp );

    f64 srsp = sr*sp;
    f64 crsp = cr*sp;

    M[1] = (f32)( srsp*cy-cr*sy );
    M[5] = (f32)( srsp*sy+cr*cy );
    M[9] = (f32)( sr*cp );

    M[2] = (f32)( crsp*cy+sr*sy );
    M[6] = (f32)( crsp*sy-sr*cy );
    M[10] = (f32)( cr*cp );

	identity = false;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::isIdentity() const
{
	if ( identity )
		return identity;

	for (s32 i=0; i<4; ++i)
		for (s32 j=0; j<4; ++j)
		{
			if (j != i)
			{
				if ((*this)(i,j) < -0.0000001f || (*this)(i,j) >  0.0000001f)
					return false;
			}
			else
				if ((*this)(i,j) < 0.9999999f || (*this)(i,j) > 1.0000001f)
					return false;
		}
	
	identity = true;

	return identity;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::rotateVect( vector3df& vect ) const
{
    static vector3df tmp;

	tmp = vect;

    vect.X = -(tmp.X*M[0] + tmp.Y*M[1] + tmp.Z*M[2]);
    vect.Y = -(tmp.X*M[4] + tmp.Y*M[5] + tmp.Z*M[6]);
    vect.Z = -(tmp.X*M[8] + tmp.Y*M[9] + tmp.Z*M[10]);
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::inverseRotateVect( vector3df& vect ) const
{
    static vector3df tmp;

	tmp = vect;

    vect.X = tmp.X*M[0] + tmp.Y*M[1] + tmp.Z*M[2];
    vect.Y = tmp.X*M[4] + tmp.Y*M[5] + tmp.Z*M[6];
    vect.Z = tmp.X*M[8] + tmp.Y*M[9] + tmp.Z*M[10];
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::transformVect(vector3df& vect) const
{
    static f32 vector[3];

    vector[0] = vect.X*M[0] + vect.Y*M[4] + vect.Z*M[8] + M[12];
    vector[1] = vect.X*M[1] + vect.Y*M[5] + vect.Z*M[9] + M[13];
    vector[2] = vect.X*M[2] + vect.Y*M[6] + vect.Z*M[10] + M[14];

    vect.X = vector[0];
    vect.Y = vector[1];
    vect.Z = vector[2];
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::transformVect( const vector3df& in, vector3df& out) const
{
    out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
    out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
    out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
}

//----------------------------------------------------------------------------

//! Transforms a plane by this matrix
MY_FORCEINLINE void matrix4::transformPlane( plane3d<f32> &plane) const
{
    static vector3df member;

    transformVect(plane.getMemberPoint(), member);

    vector3df origin(0,0,0);
    transformVect(plane.Normal);
    transformVect(origin);

    plane.Normal -= origin;
    plane.D = - member.getDotProduct(plane.Normal);
}

//----------------------------------------------------------------------------

//! Transforms a plane by this matrix
MY_FORCEINLINE void matrix4::transformPlane( const plane3d<f32> &in, plane3d<f32> &out) const
{
    out = in;
    transformPlane( out );
}

//----------------------------------------------------------------------------

//! Transforms a axis aligned bounding box
MY_FORCEINLINE void matrix4::transformBox(aabbox3d<f32> &box) const
{
	static vector3df edges[8];

	box.getEdges(edges);

	transformVect(edges[0]);
	box.reset(edges[0]);
	for (u32 i = 1; i < 8; i++)
	{
		transformVect(edges[i]);
		box.addInternalPoint(edges[i]);
	}
}

//----------------------------------------------------------------------------

//! Multiplies this matrix by a 1x4 matrix
MY_FORCEINLINE void matrix4::multiplyWith1x4Matrix(f32* matrix) const
{    
    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
 
    static f32 mat[4];

    mat[0] = matrix[0];
    mat[1] = matrix[1];
    mat[2] = matrix[2];
    mat[3] = matrix[3];

    matrix[0] = M[0]*mat[0] + M[4]*mat[1] + M[8]*mat[2] + M[12]*mat[3];
    matrix[1] = M[1]*mat[0] + M[5]*mat[1] + M[9]*mat[2] + M[13]*mat[3];
    matrix[2] = M[2]*mat[0] + M[6]*mat[1] + M[10]*mat[2] + M[14]*mat[3];
    matrix[3] = M[3]*mat[0] + M[7]*mat[1] + M[11]*mat[2] + M[15]*mat[3];
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::inverseTranslateVect( vector3df& vect ) const
{
    vect.X = vect.X-M[12];
    vect.Y = vect.Y-M[13];
    vect.Z = vect.Z-M[14];
}

//----------------------------------------------------------------------------

MY_FORCEINLINE void matrix4::translateVect( vector3df& vect ) const
{
    vect.X = vect.X+M[12];
    vect.Y = vect.Y+M[13];
    vect.Z = vect.Z+M[14];
}

//----------------------------------------------------------------------------

MY_FORCEINLINE bool matrix4::getInversed(matrix4& out) const
{
    /// Calculates the inverse of this Matrix 
    /// The inverse is calculated using Cramers rule.
    /// If no inverse exists then 'false' is returned.

    const matrix4 &m = *this;

    f32 d = determinant();
    
    if (d == 0.f)
        return false;

    d = 1.f / d;

    out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)));
    out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3)));
    out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)));
    out(3, 0) = d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2)));
    out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3)));
    out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3)));
    out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1)));
    out(3, 1) = d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2)));
    out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3)));
    out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3)));
    out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1)));
    out(3, 2) = d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2)));
    out(0, 3) = d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3)));
    out(1, 3) = d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3)));
    out(2, 3) = d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1)));
    out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2)));

    return true;
}

//----------------------------------------------------------------------------

//! returns the inversed matrix of this one
MY_FORCEINLINE matrix4 matrix4::getInversed() const
{
	static matrix4 t;
	getInversed ( t );
	return t;
}

//----------------------------------------------------------------------------

//! return z near value from left handed projection matrix
MY_FORCEINLINE f32 matrix4::getZNear() const
{
	f32 v1 = (*this)(2,2);
	f32 v2 = (*this)(2,3);

	return -v2/v1;
}

//----------------------------------------------------------------------------

//! return z far value from left handed projection matrix
MY_FORCEINLINE f32 matrix4::getZFar() const
{
	f32 v1 = (*this)(2,2);
	f32 v2 = (*this)(2,3);

	return v2/(1-v1);
}

//----------------------------------------------------------------------------

//! Builds a right-handed perspective projection matrix based on a field of view
MY_FORCEINLINE void matrix4::buildProjectionMatrixPerspectiveRH(
	f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar)
{
	f32 h = (f32)(1.0/tan(fieldOfViewRadians/2.0));
	f32 w = h / aspectRatio;

	M[0] = w;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = h;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[8] = 0.0f;
	M[9] = 0.0f;
	M[10] = zFar/(zNear-zFar); 
	M[11] = -1.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = zNear*zFar/(zNear-zFar); 
	M[15] = 0.0f;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a left-handed perspective projection matrix based on a field of view
MY_FORCEINLINE void matrix4::buildProjectionMatrixPerspectiveLH(
	f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar
	)
{
	f32 f = (f32)(1.0/tan(fieldOfViewRadians/2.0));

	M[0] = f/aspectRatio;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = f;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[8] = 0.0f;
	M[9] = 0.0f;
	M[10] = -(zFar+zNear)/(zNear-zFar);
	M[11] = 1.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = 2*zNear*zFar/(zNear-zFar);
	M[15] = 0.0f;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a left-handed orthogonal projection matrix.
MY_FORCEINLINE void matrix4::buildProjectionMatrixOrthoLH(
	f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar
	)
{
    (*this)(0,0) = 2/widthOfViewVolume;
    (*this)(1,0) = 0;
    (*this)(2,0) = 0;
    (*this)(3,0) = 0;

    (*this)(0,1) = 0;
    (*this)(1,1) = 2/heightOfViewVolume;
    (*this)(2,1) = 0;
    (*this)(3,1) = 0;

    (*this)(0,2) = 0;
    (*this)(1,2) = 0;
    (*this)(2,2) = -1/(zNear-zFar);
    (*this)(3,2) = 0;

    (*this)(0,3) = 0;
    (*this)(1,3) = 0;
    (*this)(2,3) = zNear/(zNear-zFar);
    (*this)(3,3) = 1;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a right-handed orthogonal projection matrix.
MY_FORCEINLINE void matrix4::buildProjectionMatrixOrthoRH(
	f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar
	)
{
    (*this)(0,0) = 2/widthOfViewVolume;
    (*this)(1,0) = 0;
    (*this)(2,0) = 0;
    (*this)(3,0) = 0;

    (*this)(0,1) = 0;
    (*this)(1,1) = 2/heightOfViewVolume;
    (*this)(2,1) = 0;
    (*this)(3,1) = 0;

    (*this)(0,2) = 0;
    (*this)(1,2) = 0;
    (*this)(2,2) = 1/(zNear-zFar);
    (*this)(3,2) = 0;

    (*this)(0,3) = 0;
    (*this)(1,3) = 0;
    (*this)(2,3) = zNear/(zNear-zFar);
    (*this)(3,3) = -1;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a left-handed look-at matrix.
MY_FORCEINLINE void matrix4::buildViewMatrixLH( 
	const vector3df& position,
    const vector3df& target,
    const vector3df& upVector
	)
{
    vector3df zaxis = target - position;
    zaxis.normalize();

    vector3df xaxis = upVector.getCrossProduct(zaxis);
    xaxis.normalize();

    vector3df yaxis = zaxis.getCrossProduct(xaxis);

    (*this)(0,0) = xaxis.X;
    (*this)(1,0) = yaxis.X;
    (*this)(2,0) = zaxis.X;
    (*this)(3,0) = 0;

    (*this)(0,1) = xaxis.Y;
    (*this)(1,1) = yaxis.Y;
    (*this)(2,1) = zaxis.Y;
    (*this)(3,1) = 0;

    (*this)(0,2) = xaxis.Z;
    (*this)(1,2) = yaxis.Z;
    (*this)(2,2) = zaxis.Z;
    (*this)(3,2) = 0;

    (*this)(0,3) = -xaxis.getDotProduct(position);
    (*this)(1,3) = -yaxis.getDotProduct(position);
    (*this)(2,3) = -zaxis.getDotProduct(position);
    (*this)(3,3) = 1.0f;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a right-handed look-at matrix.
MY_FORCEINLINE void matrix4::buildViewMatrixRH( 
	const vector3df& position,
    const vector3df& target,
    const vector3df& upVector
	)
{
    vector3df zaxis = position - target;
    zaxis.normalize();

    vector3df xaxis = upVector.getCrossProduct(zaxis);
    xaxis.normalize();

    vector3df yaxis = zaxis.getCrossProduct(xaxis);

    (*this)(0,0) = xaxis.X;
    (*this)(1,0) = yaxis.X;
    (*this)(2,0) = zaxis.X;
    (*this)(3,0) = 0;

    (*this)(0,1) = xaxis.Y;
    (*this)(1,1) = yaxis.Y;
    (*this)(2,1) = zaxis.Y;
    (*this)(3,1) = 0;

    (*this)(0,2) = xaxis.Z;
    (*this)(1,2) = yaxis.Z;
    (*this)(2,2) = zaxis.Z;
    (*this)(3,2) = 0;

    (*this)(0,3) = -xaxis.getDotProduct(position);
    (*this)(1,3) = -yaxis.getDotProduct(position);
    (*this)(2,3) = -zaxis.getDotProduct(position);
    (*this)(3,3) = 1.0f;

	identity = false;
}

//----------------------------------------------------------------------------

//! Builds a matrix that flattens geometry into a plane.
MY_FORCEINLINE void matrix4::buildShadowMatrix(vector3df lpos, plane3df plane, f32 w)
{/*
	// Find dot product between light position vector and ground plane normal.
    
	f32 d = plane.Normal.getDotProduct(lpos);

    (*this)(0,0) = plane.Normal.X * lpos.X + d;
    (*this)(1,0) = plane.Normal.X * lpos.Y;
    (*this)(2,0) = plane.Normal.X * lpos.Z;
    (*this)(3,0) = plane.Normal.X * point;

    (*this)(0,1) = plane.Normal.Y * lpos.X;
    (*this)(1,1) = plane.Normal.Y * lpos.Y + d;
    (*this)(2,1) = plane.Normal.Y * lpos.Z;
    (*this)(3,1) = plane.Normal.Y * point;

    (*this)(0,2) = plane.Normal.Z * lpos.X;
    (*this)(1,2) = plane.Normal.Z * lpos.Y;
    (*this)(2,2) = plane.Normal.Z * lpos.Z + d;
    (*this)(3,2) = plane.Normal.Z * point;

    (*this)(0,3) = plane.D * lpos.X + d;
    (*this)(1,3) = plane.D * lpos.Y;
    (*this)(2,3) = plane.D * lpos.Z;
    (*this)(3,3) = plane.D * point;
*/

	/*
	core::vector3df ldir = lpos;
	ldir.normalize();

	f32 l1 = ldir.getLength();
	f32 l2 = lpos.getLength();
	f32 l1_div_l2;

	if (l2==0)
	{
		l1_div_l2 = 0;
		point = 0;
	}
	else
	{
		l1_div_l2 = l1 / l2;
	}

	f32 pl = point * l1_div_l2;

	f32 pdl = plane.D * pl;

	f32 d = 
		plane.Normal.X * ldir.X + 
		plane.Normal.Y * ldir.Y + 
		plane.Normal.Z * ldir.Z + pdl; 

	M[0] = d - ldir.X * plane.Normal.X; 
	M[4] =   - ldir.X * plane.Normal.Y; 
	M[8] =   - ldir.X * plane.Normal.Z; 
	M[12]=   - ldir.X * plane.D; 

	M[1] =   - ldir.Y * plane.Normal.X; 
	M[5] = d - ldir.Y * plane.Normal.Y; 
	M[9] =   - ldir.Y * plane.Normal.Z; 
	M[13]=   - ldir.Y * plane.D; 

	M[2] =   - ldir.Z * plane.Normal.X; 
	M[6] =   - ldir.Z * plane.Normal.Y; 
	M[10]= d - ldir.Z * plane.Normal.Z; 
	M[14]=   - ldir.Z * plane.D; 

	M[3] =   - pl * plane.Normal.X; 
	M[7] =   - pl * plane.Normal.Y; 
	M[11]=   - pl * plane.Normal.Z; 
	M[15]= d - pdl; 
*/

	f32 dot = 
		plane.Normal.X * lpos.X + 
		plane.Normal.Y * lpos.Y + 
		plane.Normal.Z * lpos.Z + w; 

	M[0] = dot - lpos.X * plane.Normal.X; 
	M[4] = 0.f - lpos.X * plane.Normal.Y; 
	M[8] = 0.f - lpos.X * plane.Normal.Z; 
	M[12]= 0.f - lpos.X * plane.D; 

	M[1] = 0.f - lpos.Y * plane.Normal.X; 
	M[5] = dot - lpos.Y * plane.Normal.Y; 
	M[9] = 0.f - lpos.Y * plane.Normal.Z; 
	M[13]= 0.f - lpos.Y * plane.D; 

	M[2] = 0.f - lpos.Z * plane.Normal.X; 
	M[6] = 0.f - lpos.Z * plane.Normal.Y; 
	M[10]= dot - lpos.Z * plane.Normal.Z; 
	M[14]= 0.f - lpos.Z * plane.D; 

	M[3] = 0.f - w * plane.Normal.X; 
	M[7] = 0.f - w * plane.Normal.Y; 
	M[11]= 0.f - w * plane.Normal.Z; 
	M[15]= dot - w * plane.D;

	identity = false;
}

//----------------------------------------------------------------------------

//! creates a new matrix as interpolated matrix from to other ones.
//! \param time: Must be a value between 0 and 1.
MY_FORCEINLINE matrix4 matrix4::interpolate(matrix4& b, f32 time) const
{
    f32 ntime = 1.0f - time;
    matrix4 mat;
    for (s32 i=0; i<16; ++i)
        mat.M[i] = M[i]*ntime + b.M[i]*time;

    return mat;
}

//----------------------------------------------------------------------------

//! Build a reflection matrix for the passed in plane.
MY_FORCEINLINE void matrix4::buildReflectionMatrix(const plane3d<f32> &p)
{
	M[0] = -2 * p.Normal.X * p.Normal.X + 1;
	M[1] = -2 * p.Normal.X * p.Normal.Y;      
	M[2] = -2 * p.Normal.X * p.Normal.Z;       
	M[3] = -2 * p.Normal.X * p.D;

    M[4] = -2 * p.Normal.Y * p.Normal.X;     
	M[5] = -2 * p.Normal.Y * p.Normal.Y + 1;   
	M[6] = -2 * p.Normal.Y * p.Normal.Z;       
	M[7] = -2 * p.Normal.Y * p.D; 
        
	M[8]  = -2 * p.Normal.Z * p.Normal.X;     
	M[9]  = -2 * p.Normal.Z * p.Normal.Y;       
	M[10] = -2 * p.Normal.Z * p.Normal.Z + 1;  
	M[11] = -2 * p.Normal.Z * p.D; 
         
	M[12] = 0;                                
	M[13] = 0;                                  
	M[14] = 0;                                  
	M[15] = 1;

	identity = false;
}

//----------------------------------------------------------------------------

//! Build a rotation matrix around vector by some angle (degrees)
MY_FORCEINLINE void matrix4::buildRotationDegreesMatrix(f32 theta, core::vector3df &V)
{
	buildRotationRadiansMatrix( f32(DEG2RAD) * theta, V);
}

//----------------------------------------------------------------------------

//! Build a rotation matrix around vector by some angle (radians)
MY_FORCEINLINE void matrix4::buildRotationRadiansMatrix(f32 theta, core::vector3df &V)
{
	f32 theta_div_2 = 0.5f*theta;

    f32 cos_t = cos(theta_div_2);
    f32 sin_t = sin(theta_div_2);

    f32 x = V.X * sin_t;
    f32 y = V.Y * sin_t;
    f32 z = V.Z * sin_t;
    f32 w = cos_t;

    f32 xx = x*x, xy = x*y, xz = x*z, xw=x*w;
    f32 yy = y*y, yz = y*z, yw = y*w;
    f32 zz = z*z, zw = z*w;

    M[0] = 1 - 2*(yy+zz);
	M[1] = 2*(xy-zw);
	M[2] = 2*(xz+yw);
	M[3] = 0;
                            
	M[4] = 2*(xy+zw); 
	M[5] = 1 - 2*(xx+zz);     
	M[6] = 2*(yz-xw); 
	M[7] = 0;
                            
	M[8]  = 2*(xz-yw);     
	M[9]  = 2*(yz+xw); 
	M[10] = 1 - 2*(xx+yy); 
	M[11] = 0;

	M[12] = 0;                                
	M[13] = 0;                                  
	M[14] = 0;                                  
	M[15] = 1;

	identity = false;
}

//----------------------------------------------------------------------------

//! Converting to D3D texture matrix
MY_FORCEINLINE f32* matrix4::toD3DTextureMatrix() const
{
	static f32 m_buff[16];
	static const s32 size16 = 16*sizeof(f32);

	memset(m_buff, 0, size16);

	m_buff[10] = m_buff[15] = 1;

	m_buff[0]=M[0];
	m_buff[1]=M[1];
	m_buff[4]=M[4];
	m_buff[5]=M[5];

	m_buff[8]=M[12];
	m_buff[9]=M[13];

	m_buff[2]=M[3];
	m_buff[6]=M[7];

	return (f32*)m_buff;
}

//----------------------------------------------------------------------------

//! Converting to OGL texture matrix
MY_FORCEINLINE f32* matrix4::toOGLTextureMatrix() const
{
	return (f32*)M;
}

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif 


