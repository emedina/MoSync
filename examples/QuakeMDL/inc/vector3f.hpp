/*
 * vector3f.hpp
 *
 *  Created on: 2009-jun-10
 *      Author: Ali Mosavian
 */

#ifndef				__VECTOR3F_H__
#define				__VECTOR3F_H__

class Vector3f
{
public:
	float	m_x;
	float	m_y;
	float	m_z;

	/**
	 * Constructor
	 *
	 */
	Vector3f ( float x = 0.0f,
			   float y = 0.0f,
			   float z = 0.0f )
	: m_x( x ),
	  m_y( y ),
	  m_z( z )
	{
	};

	/**
	 * Constructor
	 *
	 */
	Vector3f ( const Vector3f &v )
	: m_x( v.m_x ), m_y( v.m_y ), m_z( v.m_z )
	{
	};


	/**
	 * Add two vectors
	 *
	 */
	Vector3f operator + ( const Vector3f &v ) const
	{
		return Vector3f( m_x+v.m_x, m_y+v.m_y, m_z+v.m_z );
	}


	/**
	 * Subtract two vectors
	 *
	 */
	Vector3f operator - ( const Vector3f &v ) const
	{
		return Vector3f( m_x-v.m_x, m_y-v.m_y, m_z-v.m_z );
	}


	/**
	 * Per element multiplication
	 *
	 */
	Vector3f operator * ( const Vector3f &v ) const
	{
		return Vector3f( m_x*v.m_x, m_y*v.m_y, m_z*v.m_z );
	}


	/**
	 * Multiply with scalar
	 *
	 */
	Vector3f operator * ( float s ) const
	{
		return Vector3f( m_x*s, m_y*s, m_z*s );
	}


	/**
	 * Divide by scalar
	 *
	 */
	Vector3f operator / ( float s ) const
	{
		return Vector3f( m_x/s, m_y/s, m_z/s );
	}


	/**
	 * Divide with scalar
	 *
	 */
	Vector3f operator /= ( float s )
	{
		m_x /= s;
		m_y /= s;
		m_z /= s;
                return *this;
	}


	/**
	 * Assignment
	 *
	 */
	Vector3f operator = ( const Vector3f &v )
	{
		m_x = v.m_x;
		m_y = v.m_y;
		m_z = v.m_z;
		return *this;
	}

	float32 dot ( const Vector3f &v ) const
	{
		return m_x*v.m_x+m_y*v.m_y+m_z*v.m_z;
	}

	float32 length ( void ) const
	{
		return sqrt( this->dot( *this ) );
	}

	Vector3f & normalize ( void )
	{
		float32 mag = this->length( );
		if ( mag > 0.0001f )
			*this /= mag;
		return *this;
	}

	Vector3f cross ( const Vector3f &v ) const
	{
		return Vector3f( m_y*v.m_z - m_z*v.m_y,
						 m_z*v.m_x - m_x*v.m_z,
						 m_x*v.m_y - m_y*v.m_x );
	}


};

#endif
