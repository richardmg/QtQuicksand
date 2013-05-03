/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __TSURFACE_RGBA8888_H__
#define __TSURFACE_RGBA8888_H__

//#include "T3DMath.h"
#include "TBase.h"



inline TDWORD TRGBA8888Operation_add( const TDWORD c1, const TDWORD c2 )
{
  register TDWORD temp = ((c1&0xFEFEFEFE)>>1)+((c2&0xFEFEFEFE)>>1);
  register TDWORD mask = (temp&0x80808080); 
  temp |= (mask-(mask>>7));
  return ((temp&0x7F7F7F7F)<<1);
};


inline TDWORD TRGBA8888Operation_sampleColor( const TDWORD *d, const TSDWORD x, const TSDWORD y, const TSDWORD pitch )
{
  return (((
	    ((d[y*pitch+x]>>4)&0x0e0e0e0e) * (16-(x&15)) +
	    ((d[y*pitch+x+1]>>4)&0x0e0e0e0e) * (x&15)
	    )>>4)&0x0e0e0e0e) * (16-(y&15)) + 
    (((
       ((d[(y+1)*pitch+x]>>4)&0x0e0e0e0e) * (16-(x&15)) +
       ((d[(y+1)*pitch+x+1]>>4)&0x0e0e0e0e) * (x&15)
       )>>4)&0x0e0e0e0e) * (y&15);
}




class CTSurface_RGBA8888 : public CTBaseSurface  
{
public:
  CTSurface_RGBA8888();
  ~CTSurface_RGBA8888();
  
  
  TDWORD getPixel( const TSDWORD x, const TSDWORD y);
  void setPixel( const TSDWORD x, const TSDWORD y, TDWORD set );

  TSDWORD blit( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *source, eTBLITOPERATION blitType = eTBLITOPERATION_COPY, TDWORD blitColor = 0xFFFFFFFF, TSDWORD width = -1, TSDWORD height = -1 );
  
  TSDWORD alphaColorBlit( TSDWORD x, TSDWORD y, CTBaseSurface *alphaSource, TDWORD color, TSDWORD width = -1, TSDWORD height = -1 );
  TSDWORD alphaColorBlit_FromColorImage( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *alphaSource, TDWORD color, TSDWORD width = -1, TSDWORD height = -1 );
  
  void add( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *source );
  void clear();
  void clearWhite();
  void clear( TDWORD col );
  void clearGradient( TDWORD col1, TDWORD col2, TDWORD col3, TDWORD col4 );
  void fillRect( TSDWORD x, TSDWORD y, TSDWORD width, TSDWORD height, TDWORD col );
  void clearAlpha( TBYTE alpha );
  
  void setReference( CTSurface_RGBA8888 *s, TSDWORD x, TSDWORD y, TSDWORD width, TSDWORD height );

  //void colorTransform( CTMatrix4 *m );          // 4x4 matrix with 8bit accuracy. 
  //void hueRotate( TSDWORD fixedAngle );
	void negate();
  
  TSDWORD readTGA( ITStream *source );

 };

#endif //__TSURFACE_RGBA8888_H__
