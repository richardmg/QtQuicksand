/**
 * Helpers for perpixel drawing / image manipulation
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __TDRAWHELPERS__
#define __TDRAWHELPERS__

#include "TSurface_RGBA8888.h"


class CTDrawHelpers {
public:
  CTDrawHelpers(void);
  ~CTDrawHelpers(void);
  
  static TSDWORD linearScaleImage( CTSurface_RGBA8888 *source, CTSurface_RGBA8888 *target, TSDWORD type );

  static void zoom( CTSurface_RGBA8888 *target, 
                    TSDWORD x, TSDWORD y, TSDWORD width, TSDWORD height, 
                    CTBaseSurface *source );

  static void rotateBlit( CTSurface_RGBA8888*target, CTSurface_RGBA8888 *source,
                          TSDWORD middlex, TSDWORD middley,
                          TSDWORD vx, TSDWORD vy,
                          TDWORD col, eTBLITOPERATION o = eTBLITOPERATION_ALPHA );
 

	
  static CTBaseSurface* qualityShrink( CTBaseSurface *source, TSDWORD sizeDiv, eTSURFACEFORMAT returnFormat );
};


#endif
