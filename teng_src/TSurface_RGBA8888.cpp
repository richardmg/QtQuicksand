/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include <string.h>
#include "TSurface_RGBA8888.h"

CTSurface_RGBA8888::CTSurface_RGBA8888() {

}

CTSurface_RGBA8888::~CTSurface_RGBA8888() {

}

void CTSurface_RGBA8888::negate() {
	TDWORD *d = (TDWORD*)m_data;
	TDWORD *d_target = d + m_width * m_height;
	while (d!=d_target) {
		*d = (0x00FFFFFF^(*d));
		d++;
	};	
};

TSDWORD CTSurface_RGBA8888::blit( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *source, eTBLITOPERATION blitType, TDWORD blitColor, TSDWORD width, TSDWORD height )
{
	TSDWORD sourceOffset = 0;
	TDWORD *t = ((TDWORD*)getData()) + 
		bltPrepeare(x,y, *source, width, height, sourceOffset );

	if (width<0 || height<0) return 0;

	TDWORD *s = ((TDWORD*)source->getData()) + sourceOffset;


	TDWORD *t_target;


	switch (blitType) {
  default:
	  return 0;

  case eTBLITOPERATION_COPY:
	  while (height>0) {
		  memcpy( t, s, width * 4 );
                  
		  s+=source->m_pitch;
		  t+=m_pitch; 
          height--;
	  }
	  return 1;

  case eTBLITOPERATION_COPY_LEAVEALPHA:
	  while (height>0) {
		  t_target = t + width;
		  while (t != t_target) {	
              *t &= 0xFF000000;
              *t |= ((*s)&0x00FFFFFF);
			  s++; t++; 
		  }
		  s-=width; s+=source->m_pitch;
		  t-=width; t+=m_pitch; height--;
	  }

	  return 1;

  case eTBLITOPERATION_FASTALPHA:
	  while (height>0) {
		  TDWORD mul;
		  t_target = t + width;
		  while (t != t_target) {	
			  mul = ((((((*s)>>24)*(blitColor>>24))>>8)+8) >> 4);
			  if (mul > 0) {
				  if (mul<16) 
					  *t = (((*s&0xF0F0F0F0)>>4)*mul + (((*t)&0xF0F0F0F0)>>4)*(16-mul));
				  else *t = *s;
			  }
			  s++; t++; 
		  }
		  s-=width; s+=source->m_pitch;
		  t-=width; t+=m_pitch; height--;
	  }

	  return 1;

  case eTBLITOPERATION_ADD:
	  while (height>0) {
		  TSDWORD r,g,b,a;
		  TDWORD col = blitColor;
		  //kerro colin komponentit alphalla
		  if (col>>24<255) {
			r = (((col>>0)&255) * (col>>24))>>8;
			g = (((col>>8)&255) * (col>>24))>>8;
			b = (((col>>16)&255) * (col>>24))>>8;
			col = r | (g<<8) | (b<<16);
		  }

		  t_target = t + width;
		  while (t != t_target) {	
			  // mul used as temp
			  a = *s>>24;
			   r = ((((*s>>0)&255) * a * ((col>>0)&255)) >> 16) + ((*t>>0)&255);
			  g = ((((*s>>8)&255) * a * ((col>>8)&255)) >> 16) + ((*t>>8)&255);
			  b = ((((*s>>16)&255) * a * ((col>>16)&255)) >> 16) + ((*t>>16)&255);

			  if (r>255) r = 255;
			  if (g>255) g = 255;
			  if (b>255) b = 255;

			  *t = (r) | (g<<8) | (b<<16);
			  

			  s++; t++; 
		  }
		  s-=width; s+=source->m_pitch;
		  t-=width; t+=m_pitch; height--;
	  }

	  return 1;



  case eTBLITOPERATION_FASTMULADD:
	  while (height>0) {
		  TDWORD mul,col;
		  t_target = t + width;
		  while (t != t_target) {	
			  // mul used as temp

			  col = ((*s&0xF0F0F0F0)>>4)*(blitColor>>29) + ((*t&0xFEFEFEFE)>>1);
			  mul = (col&0x80808080); 
			  col |= (mul-(mul>>7));
			  *t = ((col&0x7F7F7F7F)<<1);



			  s++; t++; 
		  }
		  s-=width; s+=source->m_pitch;
		  t-=width; t+=m_pitch; height--;
	  }

	  return 1;


  case eTBLITOPERATION_ALPHA:
	  TDWORD a, nega, seta;
	  while (height>0) {
		  t_target = t + width;
		  while (t != t_target) {	
			  a = ((((*s)>>24)&255) * ((blitColor>>24)&255)) >> 8;
			  seta = (((*t)>>24)&255) + a;
			  if (seta>255) seta = 255;
			  nega = (256-a)<<8;
			  *t = ( ( ( (((((*s)>>0 )&255) * ((blitColor>>0 )&255) ) * a ) + ((((*t)>>0 )&255) * nega) ) >> 16 ) << 0 ) |
				  ( ( ( (((((*s)>>8 )&255) * ((blitColor>>8 )&255) ) * a ) + ((((*t)>>8 )&255) * nega) ) >> 16 ) << 8 ) |
				  ( ( ( (((((*s)>>16)&255) * ((blitColor>>16)&255) ) * a ) + ((((*t)>>16)&255) * nega) ) >> 16 ) << 16) | (seta<<24);

			  s++; t++; 
		  }
		  s-=width; s+=source->m_pitch;
		  t-=width; t+=m_pitch; height--;
	  }
	  return 1;
	}
}

void CTSurface_RGBA8888::clearAlpha( TBYTE alpha ) {
	TDWORD newAlpha = (TDWORD)alpha<<24;
	TDWORD *t = (TDWORD*)m_data;
	TSDWORD height = getHeight();
	while ( height>0 ) {
		TDWORD *t_horline = t;
		TDWORD *t_target = t + getWidth();
		while (t_horline!=t_target) {
			*t_horline &= 0x00FFFFFF;
			*t_horline |= newAlpha;
			t_horline++;
		};
		height--;
		t+=getPitch();
	}


};

void CTSurface_RGBA8888::fillRect( TSDWORD x, TSDWORD  y, TSDWORD width, TSDWORD height, TDWORD col ) {
	CTSurface_RGBA8888 sub;
	sub.setReference(this, x,y, width, height );
	sub.clear( col );
};

TSDWORD CTSurface_RGBA8888::alphaColorBlit_FromColorImage( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *alphaSource, TDWORD color, TSDWORD width, TSDWORD height ) {
	TSDWORD sourceOffset = 0;
	TDWORD *t = ((TDWORD*)getData()) + 
		bltPrepeare(x,y, *alphaSource, width, height, sourceOffset );

	if (width<1 || height<1) return 0;

	TDWORD *s = ((TDWORD*)alphaSource->getData()) + sourceOffset;
	TDWORD *t_target;

	TDWORD a;

	TDWORD r,g,b;

	r = ((color>>0)&255);
	g = ((color>>8)&255);
	b = ((color>>16)&255);

	while (height>0) {
		t_target = t + width;
		while (t != t_target) {	
			a = (((*s>>24) * ((color>>24)&255) >> 8));
			*t = ((((((*t)>>0)&255)*(255^a) + r*a)>>8)<<0) |
				((((((*t)>>8)&255)*(255^a) + g*a)>>8)<<8) |
				((((((*t)>>16)&255)*(255^a) + b*a)>>8)<<16) | ((*t)&0xFF000000); // | (a<<24); // do not touch alpha at this point, keep the original

			s++; t++; 
		}
		s-=width; s+=alphaSource->m_pitch;
		t-=width; t+=m_pitch; height--;
	}

	return 1;
};


TSDWORD CTSurface_RGBA8888::alphaColorBlit( TSDWORD x, TSDWORD y, CTBaseSurface *alphaSource, TDWORD color, TSDWORD width, TSDWORD height )
{
	TSDWORD sourceOffset = 0;
	TDWORD *t = ((TDWORD*)getData()) + 
		bltPrepeare(x,y, *alphaSource, width, height, sourceOffset );

	if (width<1 || height<1) return 0;

	TBYTE *s = ((TBYTE*)alphaSource->getData()) + sourceOffset;
	TDWORD *t_target;

	TBYTE a, nega;

	TDWORD r,g,b;

	r = ((color>>0)&255);
	g = ((color>>8)&255);
	b = ((color>>16)&255);

	while (height>0) {
		t_target = t + width;
		while (t != t_target) {	
			a = (TBYTE)(((*s) * ((color>>24)&255) >> 8));
			nega = ~a;
			*t = ((((((*t)>>0)&255)*nega + r*a)>>8)<<0) |
				((((((*t)>>8)&255)*nega + g*a)>>8)<<8) |
				((((((*t)>>16)&255)*nega + b*a)>>8)<<16) | ((*t)&0xFF000000); // | (a<<24); // do not touch alpha at this point, keep the original

			s++; t++; 
		}
		s-=width; s+=alphaSource->m_pitch;
		t-=width; t+=m_pitch; height--;
	}

	return 1;
}



void CTSurface_RGBA8888::clear()
{
	clear(0xFF000000);
}


void CTSurface_RGBA8888::clearWhite()
{
    clear(0xFFFFFFFF);
}


void CTSurface_RGBA8888::clearGradient( TDWORD col1, TDWORD col2, TDWORD col3, TDWORD col4 ) {
  if (m_width<1 || m_height<1) return;
  TSDWORD r = (TSDWORD)((col1>>0)&255)<<16;
  TSDWORD g = (TSDWORD)((col1>>8)&255)<<16;
  TSDWORD b = (TSDWORD)((col1>>16)&255)<<16;
  TSDWORD a = (TSDWORD)((col1>>24)&255)<<16;
  TSDWORD rinc  = (((TSDWORD)((col4>>0)&255)<<16) - r) / (m_height);
  TSDWORD ginc  = (((TSDWORD)((col4>>8)&255)<<16) - g) / (m_height);
  TSDWORD binc  = (((TSDWORD)((col4>>16)&255)<<16) - b) / (m_height);
  TSDWORD ainc  = (((TSDWORD)((col4>>24)&255)<<16) - a) / (m_height);
  
  TSDWORD r2 = (TSDWORD)((col2>>0)&255)<<16;
  TSDWORD g2 = (TSDWORD)((col2>>8)&255)<<16;
  TSDWORD b2 = (TSDWORD)((col2>>16)&255)<<16;
  TSDWORD a2 = (TSDWORD)((col2>>24)&255)<<16;
  TSDWORD rinc2  = (( (TSDWORD)((col3>>0)&255)<<16) - r2) / (m_height);
  TSDWORD ginc2  = (( (TSDWORD)((col3>>8)&255)<<16) - g2) / (m_height);
  TSDWORD binc2  = (( (TSDWORD)((col3>>16)&255)<<16) - b2) / (m_height);
  TSDWORD ainc2  = (( (TSDWORD)((col3>>24)&255)<<16) - a2) / (m_height);
  
  r+=(rinc>>1); 
  g+=(ginc>>1); 
  b+=(binc>>1); 
  a+=(ainc>>1);
  r2+=(rinc2>>1); 
  g2+=(ginc2>>1); 
  b2+=(binc2>>1); 
  a2+=(ainc2>>1);

  TSDWORD y=0;
  while (y<m_height) {
    TDWORD *t = (TDWORD*)m_data + y*m_pitch;
    TDWORD *t_target = t + m_width;

    TSDWORD innerrinc = (r2-r) / (m_width);
    TSDWORD innerginc = (g2-g) / (m_width);
    TSDWORD innerbinc = (b2-b) / (m_width);
    TSDWORD innerainc = (a2-a) / (m_width);    
    
    TSDWORD innerr = r +(innerrinc>>1);
    TSDWORD innerg = g +(innerginc>>1);
    TSDWORD innerb = b +(innerbinc>>1);
    TSDWORD innera = a +(innerainc>>1);
    
	
	if (innerainc==0 && (innera>>16)>=255) {
		while (t!=t_target) {
		  *t =  (innerr>>16) | ((innerg>>16)<<8) | ((innerb>>16)<<16);
		  innerr += innerrinc;
		  innerg += innerginc;
		  innerb += innerbinc;
		   t++;
		};
	} else 
		while (t!=t_target) {
	      
	      
		  *t = 
		((((((*t)>>0)&255) * (256-(innera>>16)) + (innerr>>16) * (innera>>16))>>8)<<0) |
		((((((*t)>>8)&255) * (256-(innera>>16)) + (innerg>>16) * (innera>>16))>>8)<<8) |
		((((((*t)>>16)&255) * (256-(innera>>16)) + (innerb>>16) * (innera>>16))>>8)<<16);
	      
		  innerr += innerrinc;
		  innerg += innerginc;
		  innerb += innerbinc;
		  innera += innerainc;
	      
		  t++;
		};
    
    r+=rinc; g+=ginc; b+=binc; a+=ainc;
    r2+=rinc2; g2+=ginc2; b2+=binc2; a2+=ainc2;
    
    y++;
  };
  
};








void CTSurface_RGBA8888::clear(TDWORD col)
{
	if ((col>>24)==0) return;
	TDWORD *t = (TDWORD*)m_data;
	TSDWORD height = m_height;
	TDWORD *thorline, *thorline_target;

	if ((col>>24)==255) {
		while (height>0) {
			thorline = t;
			thorline_target = t+m_width;
			while (thorline!=thorline_target) { *thorline =col; thorline++; }
			t += m_pitch;
			height--;
		}
	} else {
		TDWORD mul = (col>>24);
		TDWORD premulr = ((col&255) * mul) ;
		TDWORD premulg = (((col>>8)&255) * mul);
		TDWORD premulb = (((col>>16)&255) * mul);
		mul = 256-mul;

		while (height>0) {
			thorline = t;
			thorline_target = t+m_width;

			while (thorline!=thorline_target) {
				*thorline = 
						(((((*thorline)&255) * mul) + premulr) >> 8) |
						(((((((*thorline)>>8)&255) * mul) + premulg) >> 8) << 8 )|
						(((((((*thorline)>>16)&255) * mul) + premulb) >> 8) << 16);
				
				thorline++;

			};

			t += m_pitch;
			height--;
		}

	};
}


void CTSurface_RGBA8888::setReference( CTSurface_RGBA8888 *s, TSDWORD x, TSDWORD y, TSDWORD width, TSDWORD height )
{
	

	if (x>=s->getWidth() || y>=s->getHeight() || x+width<0 || y+height<0) { 
		release(); return; 
	}
	
	if (x<0) { width+=x; x=0; }
	if (y<0) { height+=y; y=0; }
	if (x+width>s->getWidth()) width-= ((x+width)-s->getWidth());
	if (y+height>s->getHeight()) height-= ((y+height)-s->getHeight());
	
	if (width<0 || height<0) { release();return;};
	
	TDWORD *sdata = (TDWORD*)s->getData() + y*s->getPitch()+x;
	CTBaseSurface::setAsReference( sdata, width, height, s->getPitch(), s->getFormat() );
}






void CTSurface_RGBA8888::add( TSDWORD x, TSDWORD y, CTSurface_RGBA8888 *source )
{
	TSDWORD width, height;
	width = source->getWidth();
	height = source->getHeight();

	TSDWORD sourceOffset = 0;
	TDWORD *t = ((TDWORD*)getData()) + 
		bltPrepeare(x,y, *source, width, height, sourceOffset );

	if (width<0 || height<0) return;
	TDWORD *s = ((TDWORD*)source->getData()) + sourceOffset;
	TDWORD *t_target;


	while (height>0) {
		t_target = t + width;
		while (t != t_target) {	
			*t = TRGBA8888Operation_add( *t, *s );
			s++; t++; 
		}
		s-=width; s+=source->m_pitch;
		t-=width; t+=m_pitch; height--;
	}
}

TSDWORD CTSurface_RGBA8888::readTGA( ITStream *source ) {
	if ( !CTBaseSurface::readTGA( source ) ) return 0;
	switch (m_format) {
		case eTSURFACEFORMAT_RGBA8888:
			return 1;
		case eTSURFACEFORMAT_RGB888:
			{
				TBYTE *od = (TBYTE*)m_data;
				m_data = new TDWORD[ m_width * m_height ];
				TBYTE *source = od;
				TDWORD *target = (TDWORD*)m_data;
				TSDWORD l = m_width*m_height;
				while (l>0) {
					*target = (source[0]<<0) | (source[1]<<8) | (source[2]<<16) | (255<<24);
					l--;
					source+=3;
					target++;
				};
				delete [] od;
				m_format = eTSURFACEFORMAT_RGBA8888;
				return 1;
			}
			break;
		default:
			release();
			return 0;
	}

	return 1;	
};



TDWORD CTSurface_RGBA8888::getPixel( const TSDWORD x, const TSDWORD y) {
	if (x<0 || y<0 || x>=m_width || y>=m_height) return 0;
	return ((TDWORD*)m_data)[ m_pitch * y + x ];
};


void CTSurface_RGBA8888::setPixel( const TSDWORD x, const TSDWORD y, TDWORD set ) {
	if (x<0 || y<0 || x>=m_width || y>=m_height) return;
	((TDWORD*)m_data)[ m_pitch * y + x ] = set;
};

