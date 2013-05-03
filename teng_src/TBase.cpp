/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "TBase.h"


TDWORD tengine_checksum( const TBYTE *data, const TDWORD dataLength ) {
    register TDWORD rval = dataLength;
    register TDWORD pos = 0;
    register TDWORD bitsToMove;
    while (pos<dataLength) {
        rval ^= (TDWORD)( data[pos] );
        bitsToMove = ((rval&15)+1);
        rval = ( (rval>>(32-bitsToMove)) | ((rval&0x0FFFFFFF)<<bitsToMove) );
        pos++;
    };
    return rval;
};



TSDWORD ITStream::writeStream( ITStream *source )
{
    TSBYTE temp[256];
    TSDWORD f;
    TSDWORD count = 0;
    do {
        f = source->read( temp, 256 );
        if (f>0) {
            f = this->write( temp, f );
            if (f<1) return -1; // error.
        }
        count+=f;
    } while (f>0);
    return count;
}


TSDWORD ITStream::writeString( const TSBYTE *str ) {
    TSDWORD f = 0;
    while (str[f]!=0) f++;
    this->write( (void*)str, f+1 );
    return f;
};


TSDWORD ITStream::readString( TSBYTE *target, TSDWORD targetSize ) {
    TSDWORD f=0;
    target[0] = 0;
    target[targetSize-1]=0;
    while (f<targetSize-1) {
        this->read(target+f, 1);
        if (target[f]==0) break;
        f++;
    };	
    return f;
}	




CTBaseSurface::CTBaseSurface()
{
	m_palette = 0;
    m_data = 0;
    m_width = 0;
    m_height = 0;
    m_pitch = 0;
    m_bpp = 0;
    m_flags = 0;
}


CTBaseSurface::~CTBaseSurface()
{
    release();

}


TSDWORD CTBaseSurface::create( TSDWORD width, TSDWORD height, eTSURFACEFORMAT format )
{
    if (width==m_width && height==m_height && format==m_format) return 0;	// no need to recreate
    release();
    m_format = format;
    m_width = width;
    m_bpp = 0;
    m_height = height;
    m_pitch = width;
	m_palette = 0;
    m_flags = TSURFACEFLAG_OWNDATA;

    switch (format) {
  default:
      m_data = 0;
      m_format = eTSURFACEFORMAT_NONE;
      m_width = 0;
      m_height = 0;
      m_pitch = 0;
      m_flags = 0;
      return 0;

  case eTSURFACEFORMAT_RGB888:
      m_bpp = 3;
      m_data = new TBYTE[ width*height*3 ];
      return 1;

  case eTSURFACEFORMAT_RGBA4444:
  case eTSURFACEFORMAT_RGB565:
  case eTSURFACEFORMAT_XX88:
  case eTSURFACEFORMAT_SINGLECOMPONENT16BIT:
	  m_bpp = 2;
      m_data = new unsigned short[ width * height ];
	  return 1;

  case eTSURFACEFORMAT_SINGLECOMPONENT8BIT:
  case eTSURFACEFORMAT_INDEXED8BIT:
      m_bpp = 1;
      m_data = new TBYTE[ width * height ];
      return 1;

  case eTSURFACEFORMAT_RGBA8888:
  case eTSURFACEFORMAT_RGBA8888_PREPROCESSED:
      m_bpp = 4;
      m_data = new TDWORD[ width * height ];
      return 1;
    }
}

TSDWORD CTBaseSurface::setAsReference( const void *data, TSDWORD width, TSDWORD height, TSDWORD pitch, eTSURFACEFORMAT format )
{
	release();
	m_flags = 0;
	m_format = format;
	m_data = (void*)data;
	m_pitch = pitch;
	m_width = width;
	m_height = height;
	m_palette = 0;

	switch (m_format) {
	  case eTSURFACEFORMAT_RGB888:  m_bpp = 3;  break;
	  case eTSURFACEFORMAT_RGBA4444:
	  case eTSURFACEFORMAT_RGB565:
	  case eTSURFACEFORMAT_SINGLECOMPONENT16BIT:
		  m_bpp = 2;
		  break;
	  case eTSURFACEFORMAT_SINGLECOMPONENT8BIT:
	  case eTSURFACEFORMAT_INDEXED8BIT:
		  m_bpp = 1;
		  break;
	  case eTSURFACEFORMAT_RGBA8888:
	  case eTSURFACEFORMAT_RGBA8888_PREPROCESSED:
		  m_bpp = 4;
		  break;
	}

	return 1;
}


void CTBaseSurface::release()
{
    if (m_flags&TSURFACEFLAG_OWNDATA) {
        if (m_data) 
            delete [] ((TDWORD*)m_data);
		if (m_palette) delete [] m_palette;
    }
	
	m_palette = 0;
    m_data = 0;
    m_width = 0;
    m_height = 0;
    m_pitch = 0;
    m_flags = 0;
}


/*
struct STGAHeader {
   TSBYTE  idlength;
   TSBYTE  colourmaptype;
   TSBYTE  datatypecode;
   short TSDWORD colourmaporigin;
   short TSDWORD colourmaplength;
   TSBYTE  colourmapdepth;
   short TSDWORD x_origin;
   short TSDWORD y_origin;
   short width;
   short height;
   TSBYTE  bitsperpixel;
   TSBYTE  imagedescriptor;

   TSBYTE idstring[256];
};									// header for TGA, files

*/

TSDWORD CTBaseSurface::writeTGA( ITStream *target ) {
	if (!target) return 0;


	STGAHeader header;			// this must be filled. 

	memset( &header, 0, sizeof(STGAHeader) );			// zero all 

	header.bitsperpixel = m_bpp*8;
	header.width = m_width;
	header.height = m_height;
	header.datatypecode = 2;
	
	target->write( &header.idlength, 1 );
    target->write( &header.colourmaptype, 1 );
    target->write( &header.datatypecode, 1 );

    target->write( &header.colourmaporigin, 2 );
    target->write( &header.colourmaplength, 2 );

    target->write( &header.colourmapdepth, 1 );
    target->write( &header.x_origin, 2 );
    target->write( &header.y_origin, 2 );
    target->write( &header.width, 2 );
    target->write( &header.height, 2 );

    target->write( &header.bitsperpixel, 1 );
    target->write( &header.imagedescriptor, 1 );


    if (header.idlength>0) {			// read the id. 
        target->write(header.idstring, header.idlength );	
    }


	for (TSDWORD y = 0; y<m_height; y++) {
		target->write( (char*)m_data + m_bpp*(m_height-y-1)*m_pitch, m_bpp*m_width );
	};



	return 1;
};




TSDWORD CTBaseSurface::readTGA( ITStream *source )
{
    if (!source) return 0;

    release();
    STGAHeader header;


    source->read( &header.idlength, 1 );
    source->read( &header.colourmaptype, 1 );
    source->read( &header.datatypecode, 1 );

    source->read( &header.colourmaporigin, 2 );
    source->read( &header.colourmaplength, 2 );

    source->read( &header.colourmapdepth, 1 );
    source->read( &header.x_origin, 2 );
    source->read( &header.y_origin, 2 );
    source->read( &header.width, 2 );
    source->read( &header.height, 2 );

    source->read( &header.bitsperpixel, 1 );
    source->read( &header.imagedescriptor, 1 );


    if (header.idlength>0) {			// read the id. 
        source->read(header.idstring, header.idlength );	
    }

    TBYTE *palette = 0;
    if (header.colourmaplength>0) {
        TSDWORD paletteLength =  header.colourmaplength * header.colourmapdepth / 8;
        palette = new TBYTE[ paletteLength ];
        source->read( palette, paletteLength );
    }



    
	m_palette = 0;
    m_width = header.width;
    m_height = header.height;
    m_pitch = header.width;
    m_flags = TSURFACEFLAG_OWNDATA;
    m_data = 0;
    if (header.datatypecode == 1) {
        m_palette = palette;
    } else { 
        if (palette) delete [] palette;  
        palette = 0; 
    }
    

    TSDWORD bytes = m_width * m_height * header.bitsperpixel / 8;

    m_data = new TBYTE[ bytes ];
	TSDWORD linelength = (m_width * header.bitsperpixel) >> 3;

	if ((header.imagedescriptor&32)==0) {			//image flipped
		TBYTE *t = (TBYTE*)m_data + m_pitch * (m_height-1) * (header.bitsperpixel>>3);
		// do the flip and copy the data.
		for (TSDWORD f=0; f<header.height; f++) {
			if (source->read(t, linelength ) != linelength) {
				//printf("ERROR LOADING TGA.... RLE-COMPRESSED?\n");
				release();
				return 0;
			};
			t-=linelength;
		}
	} else {
		source->read( m_data, linelength*header.height );
	};

    m_bpp = header.bitsperpixel/8;
    if (m_bpp<1) m_bpp = 1;			// Warning?


     
    switch ( m_bpp )
    {
        case 4: m_format = eTSURFACEFORMAT_RGBA8888; break;
        case 3: m_format = eTSURFACEFORMAT_RGB888; break;
        case 2: m_format = eTSURFACEFORMAT_RGB565; break;
        case 1: 
            if (m_palette)  
                m_format = eTSURFACEFORMAT_INDEXED8BIT;
            else 
                m_format = eTSURFACEFORMAT_SINGLECOMPONENT8BIT; 
            break;
    }


    return 1;
}




TSDWORD CTBaseSurface::bltPrepeare(
                               TSDWORD x, TSDWORD y, 
                               CTBaseSurface &source, 
                               TSDWORD &ibltwidth, TSDWORD &ibltheight, 
                               TSDWORD &ibltsourceoffset )
{
    if (ibltwidth<1) ibltwidth = source.getWidth();
    if (ibltheight<1) ibltheight = source.getHeight();
    ibltsourceoffset = 0;


    // do clippping agains this image
    if (y<0) {
        ibltheight+=y;
        ibltsourceoffset-=y*source.getPitch();
        y =0;
    }
    if (x<0) {
        ibltwidth+=x;
        ibltsourceoffset -= x;
        x =0;
    }
    if (x+ibltwidth>=getWidth()) {
        ibltwidth -= ( (x+ibltwidth)-getWidth() );
    }
    if (y+ibltheight>=getHeight()) {
        ibltheight -= ( (y+ibltheight)-getHeight() );
    }
    if (ibltwidth>0 && ibltheight>0) return m_pitch*y+x; else return 0;
}



void CTBaseSurface::toStream( ITStream *target ) {

    target->write( &m_format, 4 );
    target->write( &m_width, 4 );
    target->write( &m_height, 4 );

	TBYTE hasPalette = 0;
	if (m_palette) hasPalette = 1;
	target->write( &hasPalette, 1 );
	if (hasPalette) {
		target->write( m_palette, 256*3 );
	};

    for (TSDWORD f=0; f<m_height; f++) 
        target->write( ((TBYTE*)m_data)+f*m_pitch*m_bpp, m_width*m_bpp );
};


TBOOL CTBaseSurface::fromStream( ITStream *source ) {

    release();
    eTSURFACEFORMAT format;
    TSDWORD width, height;
    if (source->read( &format, 4 ) != 4) return TFALSE;
    if (source->read( &width, 4 ) != 4) return TFALSE;
    if (source->read( &height, 4 ) != 4) return TFALSE;

	TBYTE hasPalette;
	source->read( &hasPalette, 1 );


    if (create(width,height,format) == 0) return TFALSE;



    if (hasPalette) {
		m_palette = new TBYTE[ 256*3 ];
		source->read( m_palette, 256*3 );
	};

    
    
    
    if (source->read(m_data, m_width*m_height*m_bpp) != m_width*m_height*m_bpp) return TFALSE;
    return TTRUE;
};



CTBaseSurface *CTBaseSurface::changeFormat( eTSURFACEFORMAT copyFormat ) {
    if (!m_data) return 0;
    CTBaseSurface *rval = new CTBaseSurface;
    rval->create( m_width, m_height, copyFormat );

    // do the copy. 
    switch (m_format) {
    default:
        delete rval;            // not implemented
        return 0;
        break;

    case eTSURFACEFORMAT_INDEXED8BIT:
        if (copyFormat==eTSURFACEFORMAT_RGBA8888) {
            for (TSDWORD f=0; f<m_height; f++) {
                  TDWORD *t = (TDWORD*)rval->getData()+rval->getPitch()*f;
                  TBYTE *s = (TBYTE*)getData()+getPitch()*f;
                  for (TSDWORD x=0; x<m_width; x++) {
                    *t = ( m_palette[ (*s)*3 + 0 ] << 0) |
                         ( m_palette[ (*s)*3 + 1 ] << 8) |
                         ( m_palette[ (*s)*3 + 2 ] << 16) |
                         (255<<24);     // full alpha
                    t++;
                    s++;

                  };
            };
            return rval;
        };

        delete rval;
        return 0;
        break;

    case eTSURFACEFORMAT_RGBA4444:
        if (copyFormat == eTSURFACEFORMAT_RGBA8888_PREPROCESSED) {
            for (TSDWORD f=0; f<m_height; f++) {
                    TDWORD *t = (TDWORD*)rval->getData()+rval->getPitch()*f;
                    unsigned short *s = (unsigned short*)getData()+getPitch()*f;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (TDWORD)( 
                            ((*s)&15) |
                            ((((*s)>>4)&15)<<8) |
                            ((((*s)>>8)&15)<<16) |
                            ((((*s)>>12)&15)<<24) );
						t++;s++;
					};
            } 
            return rval;
        };

        delete rval;
        return 0;
        break;

	case eTSURFACEFORMAT_RGB888:
		if (copyFormat == eTSURFACEFORMAT_RGBA8888) {
		   for (TSDWORD f=0; f<m_height; f++) {
                    TDWORD *t = (TDWORD*)rval->getData()+rval->getPitch()*f;
                    TBYTE *s = (TBYTE*)getData()+getPitch()*f*3;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (TDWORD)( s[0] | (s[1]<<8) | (s[2]<<16) );
						t++;s+=3;
					};
            } 
            return rval;
		} else if ( copyFormat== eTSURFACEFORMAT_RGB565 ) {
			
			for (TSDWORD f=0; f<m_height; f++) {
                    TWORD *t = (TWORD*)rval->getData()+rval->getPitch()*f;
                    TBYTE *s = (TBYTE*)getData()+getPitch()*f*3;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (TWORD)( (s[2]>>3) | ((s[1]>>2)<<5) | ((s[0]>>3)<<11) );
						t++;s+=3;
					};
            } 
            return rval;
		};

		delete rval;
		return 0;
    

    case eTSURFACEFORMAT_RGBA8888:            // FROM RGBA8888.
        {
            
            if (copyFormat == eTSURFACEFORMAT_RGBA4444) {
                for (TSDWORD f=0; f<m_height; f++) {
                    unsigned short *t = (unsigned short*)rval->getData()+rval->getPitch()*f;
                    TDWORD *s = (TDWORD*)getData()+getPitch()*f;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (unsigned short)((((*s)>>4)&15) | 
                             ((((*s)>>12)&15)<<4) |
                             ((((*s)>>20)&15)<<8) |
                             ((((*s)>>28)&15)<<12));

						t++;s++;
					};
                } 

				return rval;
            } else if (copyFormat == eTSURFACEFORMAT_RGB888) {
				  for (TSDWORD f=0; f<m_height; f++) {
                    unsigned short *t = (unsigned short*)rval->getData()+rval->getPitch()*f;
                    TBYTE *s = (TBYTE*)getData()+getPitch()*f*3;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (unsigned short)((((s[0])>>4)&15) | 
                             (((s[1]>>4)&15)<<4) |
                             (((s[2]>>4)&15)<<8) );
						t++;s+=3;
					};
				  }

			} else if ( copyFormat== eTSURFACEFORMAT_RGB565 ) {			// NOTE, .. check that this works when converted to byte
				for (TSDWORD f=0; f<m_height; f++) {
                    TWORD *t = (TWORD*)rval->getData()+rval->getPitch()*f;
                    TBYTE *s = (TBYTE*)getData()+getPitch()*f*4;
                    for (TSDWORD x=0; x<m_width; x++) {
                        *t = (TWORD)( (s[2]>>3) | ((s[1]>>2)<<5) | ((s[0]>>3)<<11) );
						t++;s+=4;
					};
            } 
            return rval;
		};


            delete rval;
            return 0;
        }
        break;
    };


    return rval;

};
