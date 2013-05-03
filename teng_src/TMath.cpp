/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include <math.h>
#include <stdio.h>
#include "TMath.h"

#if defined __SYMBIAN32__
inline float sinf( float arg )
{
    return sin(arg);
}

inline float cosf( float arg )
{
    return cos(arg);
}
#endif

float tmath_sin( float angle ) { return sinf( angle ); }
float tmath_cos( float angle ) { return cosf(angle ); }


float tmath_realsqrt( float val ) {
	return sqrtf( val );
};


CTMath::CTMath()
{
  for (TSDWORD f=0; f<TMATH_TABLE_LENGTH; f++) {
    cos_table[f] = (int)( cosf( (float)f /((float)TMATH_TABLE_LENGTH) *3.14159f * 2.0f ) * 65536.0f );
  }
}

CTMath::~CTMath()
{

}

TSDWORD CTMath::cos( TSDWORD i )
{
  return cos_table[i&TMATH_TABLE_AND];
}

TSDWORD CTMath::sin( TSDWORD i )
{
  return cos_table[(i+256)&TMATH_TABLE_AND];

}





#ifndef WIN32

TSDWORD tmath_sqrt(__int64 r)
{
  __int64 t;
  __int64 b;
  __int64 c=0;
  
  for (b=0x100000000000000LL;b!=0;b>>=2) {
    t = c + b;
    c >>= 1;
    if (t <= r) {
      r -= t;
      c += b;
    }
  }
  
  return (int)(c);
}
#else


TSDWORD tmath_sqrt(__int64 r)
{
  __int64 t;
  __int64 b;
  __int64 c=0;
  
  for (b=0x100000000000000;b!=0;b>>=2) {
    t = c + b;
    c >>= 1;
    if (t <= r) {
      r -= t;
      c += b;
    }
  }
  
  return (int)(c);
}

#endif


TSDWORD tmath_fpmul( const TSDWORD i1, const TSDWORD i2 ) { 
  return (int)( ((__int64)i1*(__int64)i2)>>FP_BITS ); 
}

TSDWORD tmath_fpdiv( const TSDWORD i1, const TSDWORD i2 ) {
  if (i2==0) return i1; else 
	  return (int)( (((__int64)i1)<<(__int64)(FP_BITS)) / ((__int64)i2) );
      //return (int)( (double)i1 / ((double)i2 / (double)FP_VAL) );
}

TSDWORD tmath_fpsqrt( const TSDWORD i ) {
  return tmath_sqrt( ((__int64)i)<<FP_BITS );
}

TSDWORD tmath_fpmuldiv( const TSDWORD m1, const TSDWORD m2, const TSDWORD div ) {
  return (int)( ((__int64)m1 * (__int64)m2) / div ) ;
}

TSDWORD tmath_interpolate( const TSDWORD m1, const TSDWORD m2, const TSDWORD shr ) {
  return (int)(( (__int64)m1 * (__int64)m2) >> shr );
}


TSDWORD tmath_angle(TSDWORD vx, TSDWORD vy)
{
    return (TSDWORD)( (atan2((float)vy, (float)vx ) + 3.14159f ) / (3.14159f) *32768.0f );
}


