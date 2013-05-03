/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __CTSTRING__
#define __CTSTRING__


#include "TBase.h"

// helpers
// returns 1 if one of the strings exist.
TSDWORD strBegin( const TSBYTE*  str, const TSBYTE*  match );
TSDWORD cmpString( const TSBYTE *s1, const TSBYTE *s2 );



class CTString {
 public:
  CTString( TSBYTE *initial = 0 );
  ~CTString();
  
  void set( const TSBYTE *set );
  
  TSDWORD length( const TSBYTE *str = 0); 
  void insert( const TSBYTE *test, TSDWORD pos=-1 );
  void emphty();
  
  void erase( TSDWORD beg, TSDWORD end );
  void eraseLast();
  
  inline TSBYTE *getStr() { return mData; }
  
  static TDWORD cstr_getHex( TSBYTE v );
  static TDWORD cstr_scanColour( const TSBYTE *string, TDWORD def = 0 );
  static TDWORD cstr_scanShortColour( const TSBYTE *string, TDWORD def = 0 );
  
 protected:
  TSBYTE *mData;
};



#endif
