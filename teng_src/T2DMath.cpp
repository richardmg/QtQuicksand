/*
 * Copyright (c) 2010 Nokia Corporation.
 */


#include "T2DMath.h"


int CT2DVector::setLength( const int l )
{
  int oldl = length();
  
  if (oldl<1) {
    return 0;
  }
  m_x = tmath_fpmuldiv( m_x, l, oldl );
  m_y = tmath_fpmuldiv( m_y, l, oldl );
  return oldl;
}

void CT2DVector::normalize()
{
  int l = length()+1;
  //if (l==0) return;
  m_x = tmath_fpdiv( m_x, l );
  m_y = tmath_fpdiv( m_y, l );
}

int CT2DVector::length()
{
  __int64 x = (__int64)m_x * (__int64)m_x;
  __int64 y = (__int64)m_y * (__int64)m_y;
  return tmath_sqrt( x + y );
}



void CT2DVector::fixedMul( const int mul, const int bits )
{
  m_x = tmath_interpolate( m_x, mul, bits );
  m_y = tmath_interpolate( m_y, mul, bits );
}


void CT2DVector::add( const CT2DVector &add )
{
  m_x += add.m_x;
  m_y += add.m_y;
}

void CT2DVector::sub( const CT2DVector &add )
{
  m_x -= add.m_x;
  m_y -= add.m_y;
}


void CT2DVector::createBetweenVectors( const CT2DVector &start, const CT2DVector &end )
{
  m_x = (end.m_x - start.m_x);
  m_y = (end.m_y - start.m_y);
}


int CT2DVector::dotProduct( const CT2DVector &v ) const 
{
  return (int)((
	       (__int64)m_x * (__int64)v.m_x + 
	       (__int64)m_y * (__int64)v.m_y) >> FP_BITS);
	       
}


int CT2DVector::project( const CT2DVector &v )
{
  int divid = dotProduct( *this );
  int todiv = dotProduct( v );
  if (divid == 0) divid = 1;
  return (int)((((__int64)todiv)<<FP_BITS)/(__int64)divid);
    

}








CT2DMatrix::CT2DMatrix()
{
  setIdentity();
}


CT2DMatrix::~CT2DMatrix()
{

}


void CT2DMatrix::setIdentity()
{
  m[0][0] = FP_VAL;
  m[0][1] = 0;
  m[1][0] = 0;
  m[1][1] = FP_VAL;
}



void CT2DMatrix::transform( const CT2DVector *source, CT2DVector *target, int count )
{
  while (count>0) {
    target->m_x = ((source->m_x*m[0][0] + source->m_y*m[0][1])>>8);
    target->m_y = ((source->m_x*m[1][0] + source->m_y*m[1][1])>>8);
    count--;
    target++;
    source++;
  }
}






CT2DVector::CT2DVector()
{
  m_x = 0;
  m_y = 0;

}


CT2DVector::~CT2DVector()
{


}
