/*
 * Copyright (c) 2010 Nokia Corporation.
 */

// TEngine.cpp: implementation of the TEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "TEngine.h"


CTFile::CTFile()
{
	fileHandle = 0;
}

CTFile::~CTFile()
{
	close();
}


CTFile::CTFile( const TSBYTE *fileName )
{
	fileHandle = 0;
	if (!openExisting( fileName )) 
		openNew( fileName );
}


TSDWORD CTFile::openExisting( const TSBYTE *fname )
{
	close();
	
#ifdef WIN32
	//TSDWORD errorNo = fopen_s(&fileHandle, fname, "rb" );
	//if (fileHandle) return 1; else return 0;
	fileHandle = fopen(fname, "rb");
#else
	fileHandle = fopen( fname, "rb" );
#endif

	if (fileHandle) return 1; else return 0;
}


TSDWORD CTFile::openNew( const TSBYTE *fname )
{
	close();
	//
#ifdef WIN32
	//TSDWORD errorNo = fopen_s( &fileHandle, fname, "wb" );
	fileHandle = fopen(fname, "wb");
#else
	fileHandle = fopen( fname, "wb" );
#endif
	if (fileHandle) return 1; else return 0;
}



TSDWORD CTFile::close()
{
	if (fileHandle != 0) {
		fclose(fileHandle);
		fileHandle = 0;
	}
	return 1;
}


TSDWORD CTFile::seek( TSDWORD pos ) {
    if (fileHandle==0) return 0;
    return fseek( fileHandle, pos, SEEK_SET );
};

TSDWORD CTFile::curPos() {
    if (fileHandle == 0) return -1;
    return ftell(fileHandle);
};


TSDWORD CTFile::length()
{
	if (fileHandle==0) return 0;
	TSDWORD curpos = ftell( fileHandle );
	fseek( fileHandle, 0, SEEK_END );
	TSDWORD rval = ftell( fileHandle );
	fseek( fileHandle, curpos, SEEK_SET );
	return rval;
}


void CTFile::setPos( const TSDWORD pos ) {
	if (!fileHandle) return;
	fseek( fileHandle, pos, SEEK_SET );
}

TSDWORD CTFile::read( void *target, TSDWORD length )
{
	if (!fileHandle) return 0;
	return (int)fread( target, 1,length, fileHandle );
}

TSDWORD CTFile::write( void *source, TSDWORD length )
{
	if (!fileHandle) return 0;
	return (int)fwrite( source, 1,length, fileHandle);
}



