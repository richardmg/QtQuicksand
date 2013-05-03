/*
 * Helper classes for 2D linear mathematics
 * Copyright (c) 2010 Nokia Corporation.
 *
 */

#ifndef __T2DMATH__
#define __T2DMATH__

#include "TMath.h"


class CT2DVector {
 public:
  CT2DVector();
  ~CT2DVector();

  inline CT2DVector& set( const int x, const int y ) { m_x=x; m_y=y; return *this; }
  inline CT2DVector& set( const CT2DVector &s ) { set( s.m_x, s.m_y ); return *this; }
  
  void add( const CT2DVector &add );
  void sub( const CT2DVector &sub ); 
  void createBetweenVectors( const CT2DVector &start, const CT2DVector &end ); 

  int dotProduct( const CT2DVector &v ) const;
  int project( const CT2DVector &v );

  void fixedMul( const int mul, const int bits = FP_BITS );
  
  void normalize();
  int length();
  int setLength( const int l );


  int m_x;
  int m_y;
};

class CT2DMatrix {
 public:
  CT2DMatrix();
  ~CT2DMatrix();
  void setIdentity();


  void transform( const CT2DVector *source, CT2DVector *target, int count );  
  
  int m[2][2];
};









#endif
