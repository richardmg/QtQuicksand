/**
 * Helpers for fixedpoint math
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __TMATH__
#define __TMATH__





#include "TBase.h"


#define FP_BITS 14
#define FP_HALFBITS 7
#define FP_AND 16383
#define FP_VAL 16384


inline TSDWORD tmath_fixedCeil(const TSDWORD x) { return ((x+FP_AND) >> FP_BITS); }

#ifndef WIN32
#ifndef __int64
#define __int64 long long int
#endif
#endif


//TSDWORD tmath_sqrt( TSDWORD i );
TSDWORD tmath_sqrt(__int64 r);

TSDWORD tmath_fpsqrt( const TSDWORD i );
TSDWORD tmath_fpmul( const TSDWORD i1, const TSDWORD i2);
TSDWORD tmath_fpdiv( const TSDWORD i1, const TSDWORD i2 ); 
TSDWORD tmath_fpmuldiv( const TSDWORD m1, const TSDWORD m2, const TSDWORD div );
inline TSDWORD tmath_abs( const TSDWORD m1 ) { if (m1<0) return -m1; else return m1; }

TSDWORD tmath_interpolate( const TSDWORD m1, const TSDWORD m2, const TSDWORD shr );

TSDWORD tmath_angle( TSDWORD vx, TSDWORD vy );

float tmath_realsqrt( float val );
float tmath_sin( float angle );
float tmath_cos( float angle );

#define TMATH_TABLE_LENGTH 1024
#define TMATH_TABLE_AND 1023



class CTMath {
 public:
  CTMath();
  ~CTMath();

  TSDWORD cos( TSDWORD amount );
  TSDWORD sin( TSDWORD amount );

  TSDWORD cos_table[TMATH_TABLE_LENGTH];
};





#endif
