/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef __TENGINE_BASE__
#define __TENGINE_BASE__

#include <string.h>

    // type definitions .
//#ifdef __SYMBIAN32__
#define BYTE TBYTE
#define WORD unsigned short
#define DWORD TDWORD
//#else
//#include <windows.h>
//#endif


    // 32 - bit architechture
#define TSBYTE char
#define TSWORD short
#define TSDWORD int

#define TBYTE unsigned char
#define TWORD unsigned short
#define TDWORD unsigned int



#define TBOOL TBYTE
#define TTRUE 1
#define TFALSE 0



enum eTKEYCODE { 
    TKEY_UNKNOWN, 

    TKEY_LEFT, 
    TKEY_RIGHT, 
    TKEY_UP, 
    TKEY_DOWN, 
    TKEY_ZOOMIN,        // TÄHTI
    TKEY_ZOOMOUT,       // RISTIKKO
    
    TKEY_MENU,
    TKEY_SELECT, 
    TKEY_BACK,

    TKEY_1,
    TKEY_2,
    TKEY_3,
    TKEY_4,
    TKEY_5,
    TKEY_6,
    TKEY_7,
    TKEY_8,
    TKEY_9,
    TKEY_0,

    TKEY_CALL	// Added by Johannes to make a call. 
};




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




enum eTRESOURCETYPE { 
    eTRESOURCETYPE_IMAGE = 0, 
    eTRESOURCETYPE_MESH = 1, 
    eTRESOURCETYPE_FONT = 2, 
    eTRESOURCETYPE_VFT = 3, 
    eTRESOURCETYPE_UNDEFINED = 0xFFFFFFFF           // force dword
};            


class ITResource {
public:
    ITResource() {};
    virtual ~ITResource() {};

  
};




class ITStream 
{
public:
  ITStream() {};
  virtual ~ITStream() {};
  
  virtual TSDWORD read( void *target, TSDWORD length ) = 0;
  virtual TSDWORD write( void *source, TSDWORD length ) = 0;
  virtual TSDWORD cancel() { return 0; }			// for blocking streams.
  
    // some helpers here?
  TSDWORD writeStream( ITStream *source ); // write stream until its ended
  TSDWORD writeString( const TSBYTE *str );
  TSDWORD readString( TSBYTE *target, TSDWORD targetSize );
};





class ITBuffer : public ITStream {
public:
	ITBuffer() {};
	virtual ~ITBuffer() {};
	virtual TSDWORD length() = 0;

	
};

class ITBufferWithPointer : public ITBuffer {
public:
    ITBufferWithPointer() {};
    virtual ~ITBufferWithPointer() {};
    virtual void *getPointer() = 0;
};

class CTMemoryReference : public ITBufferWithPointer {
public:
	CTMemoryReference( const TBYTE *data, TSDWORD length ) { 
		m_data = data; m_length = length; m_readPos = 0; 
	}
	virtual ~CTMemoryReference() {};
	void *getPointer() { return (void*)m_data; }

	TSDWORD read( void *target, TSDWORD length ) { 
		if (length+m_readPos>=m_length) length = m_length-m_readPos;
		if (length>0) memcpy( target, m_data+m_readPos, length );
		m_readPos+=length;
		return length;
	}
	TSDWORD write( void *source, TSDWORD length ) { return 0; }
	TSDWORD length() { return m_length; }

protected:
	const TBYTE *m_data;
	TSDWORD m_length;
	TSDWORD m_readPos;
};


enum eTSURFACEFORMAT { eTSURFACEFORMAT_NONE, 
					   eTSURFACEFORMAT_INDEXED8BIT,
                       eTSURFACEFORMAT_RGBA8888, 
                       eTSURFACEFORMAT_RGBA8888_PREPROCESSED,   // PREPROCESSED for fast lighting ,.. (highest 4bit of each component are 0 )
                       eTSURFACEFORMAT_RGBA4444, 
                       eTSURFACEFORMAT_RGB888, 
                       eTSURFACEFORMAT_RGB565, 
                       eTSURFACEFORMAT_SINGLECOMPONENT8BIT, 
                       eTSURFACEFORMAT_XX88,
					   eTSURFACEFORMAT_SINGLECOMPONENT16BIT,
                       eTSURFACEFORMAT_FORCEDWORD=0xFFFFFFFF };

enum eTBLITOPERATION { eTBLITOPERATION_COPY, 
                       eTBLITOPERATION_ALPHA, 
                       eTBLITOPERATION_ADD, 
                       eTBLITOPERATION_FASTALPHA, 
                       eTBLITOPERATION_FASTMULADD,
                       eTBLITOPERATION_COPY_LEAVEALPHA              // leaves alpha-channel untouch. otherwise, just a normal copy
                        };


#define TSURFACEFLAG_OWNDATA 1


class CTBaseSurface : public ITResource {
public:
	CTBaseSurface();
	virtual ~CTBaseSurface();
    
    eTRESOURCETYPE getResourceType() { return eTRESOURCETYPE_IMAGE; }
    TSDWORD getResourceBytes() { return (m_width*m_height*m_bpp) + sizeof( CTBaseSurface ); }
	
    TSDWORD create( TSDWORD width, TSDWORD height, eTSURFACEFORMAT format );
	TSDWORD setAsReference( const void *data, TSDWORD width, TSDWORD height, TSDWORD pitch, eTSURFACEFORMAT format );

    CTBaseSurface *changeFormat( eTSURFACEFORMAT copyFormat );      //  returns a copy with changed format.

		// As "tim" format. ... raw with simple header. 
	void toStream( ITStream *target );          
	TBOOL fromStream( ITStream *source );

	virtual TSDWORD readTGA( ITStream *source );
	virtual TSDWORD writeTGA( ITStream *target );

	void release();

	inline TSDWORD getWidth() { return m_width; }
	inline TSDWORD getHeight() { return m_height; }
	inline TSDWORD getPitch() { return m_pitch; }
    
    inline void* getData() { return m_data; }
	inline TDWORD getFlags() { return m_flags; }
	inline eTSURFACEFORMAT getFormat() { return m_format; }


	TSDWORD bltPrepeare(
					TSDWORD x, TSDWORD y, 
					CTBaseSurface &source, 
					TSDWORD &ibltwidth, TSDWORD &ibltheight, 
					TSDWORD &ibltsourceoffset );



	eTSURFACEFORMAT m_format;
	TSDWORD m_bpp;
	TSDWORD m_width;
	TSDWORD m_height;
	TSDWORD m_pitch;
	TDWORD m_flags;

	TBYTE *m_palette;
	void *m_data;
	

};


#endif

