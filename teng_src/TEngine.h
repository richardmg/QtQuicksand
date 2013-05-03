/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __TENGINE_MAIN__
#define __TENGINE_MAIN__



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "TBase.h"


class CTFile : public ITBuffer
{
public:
  CTFile();
  CTFile( const TSBYTE *fileName );
  virtual ~CTFile();
  
  TSDWORD read( void *target, TSDWORD length );				// returns the nof bytes read.
  TSDWORD write( void *source, TSDWORD length );			// returns the nof bytes wrote.
  TSDWORD curPos();                                     // returns the current-file-position.
  TSDWORD seek( TSDWORD pos );
  TSDWORD length();
  

  void setPos( const TSDWORD pos = 0 );

  TSDWORD openNew( const TSBYTE *fname );
  TSDWORD openExisting( const TSBYTE *fname );
  TSDWORD close();
  
  
 protected:
  FILE *fileHandle;

  
};







#endif



