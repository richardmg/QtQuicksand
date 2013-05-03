/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "TString.h"
#include <string.h>





TSDWORD strBegin( const TSBYTE*  str, const TSBYTE*  match ) {
    if( str == 0 ) return( -1 );
    TSDWORD rval = 0;
    while( *str != 0 && *match != 0 ) {
        if( *str != *match ) return -1;
        match++;
        str++;
        rval++;
    }
    return( rval );
}



TSDWORD cmpString( const TSBYTE *s1, const TSBYTE *s2 ) {
	TSBYTE c1,c2;
	while (*s1!=0 && *s2!=0) {
		c1 = *s1; c2 = *s2; 
		if (c1>='A' && c1<='Z') c1 = c1-'A'+'a'; 
		if (c2>='A' && c2<='Z') c2 = c2-'A'+'a';
		if (c1=='Ä') c1= 'ä';
		if (c2=='Ä') c2= 'ä';
		if (c1=='Ö') c1= 'ö';
		if (c2=='Ö') c2= 'ö';

		if (c1!=c2) break; 
		s1++; s2++;
	}
	if (*s2==0 && *s1==0) return 1; else return 0;
}

CTString::CTString( TSBYTE *initial ) {
	mData = 0;
	insert( initial );
}

CTString::~CTString() {
    if (mData) delete [] mData;	
};

void CTString::set( const TSBYTE *set ) {
    if (mData) { delete [] mData; mData = 0; }
    if (!set) return;
    TSDWORD l = length(set);
    mData = new TSBYTE[ l+1 ];
    memcpy( mData, set, l );
    //for (TSDWORD f=0; f<l; f++) mData[f] = set[f]; 
    mData[l] = 0;
};


void CTString::emphty() {
	if (mData) delete [] mData;
	mData = 0;
};


void CTString::insert( const TSBYTE *text, TSDWORD pos ) {
	if (text==0) return;
	TSDWORD curlen = length();	
	TSDWORD addLength = length( text );
	if (pos<0) pos = curlen;

	TSBYTE *oldData = mData;
	mData = new TSBYTE[ curlen + addLength + 1 ];
	// copy utill pos.
	if (oldData) {
		memcpy( mData, oldData, pos );			// old data, part 1
        //for (f=0; f<pos; f++) mData[f] = oldData[f];
	}
		// copy the new data. 
	memcpy( mData+pos, text, addLength );
    //for (f=0; f<addLength; f++) mData[pos+f] = text[f]; 

	if (oldData) {
        if (curlen!=pos) {
			memcpy( mData+pos+addLength, oldData+pos,curlen-pos );	// old data, part 2
            //for (f=0; f<(curlen-pos); f++) mData[pos+addLength+f] = oldData[pos+f];
        }
		delete [] oldData;
	}
	mData[ addLength+curlen ] = 0;		// zero the string.
};


void CTString::eraseLast() {
	if (!mData) return;
	TSDWORD l = length();
	if (l>0) mData[l-1]=0;			// do not reallocate..no need. 
};

void CTString::erase( TSDWORD beg, TSDWORD end ) {
	if (!mData) return;
	TSDWORD curlen = length();
	if (beg>=curlen) return;
	if (end<0) return;

	if (end>=curlen) end = curlen;
	if (beg<0) beg=0; 

	TSDWORD removelen = end-beg;
	if (removelen >= curlen) { delete [] mData; mData = 0; }

	TSBYTE *oldData = mData;

    if (beg>0) {
        memcpy( mData, oldData, beg );
        //for (TSDWORD f=0; f<beg; f++) mData[f] = oldData[f];
    }
    if (curlen-end>0) {
        memcpy( mData+beg, oldData+end, curlen-end );
        //for (TSDWORD f=0; f<(curlen-end); f++) mData[beg+f] = oldData[end+f];
    }
	mData[ curlen - removelen ] = 0;

	delete [] oldData;
};


TSDWORD CTString::length( const TSBYTE *str) { 
	if (!str) str = mData;
	if (!str) return 0; 
	TSDWORD rval=0; 
	while ( str[rval] ) rval ++; return rval; 
};


TDWORD CTString::cstr_getHex( TSBYTE v ) {
	if (v>='0' && v<='9') return (TDWORD)(v-'0');
	if (v>='A' && v<='Z') v = v-'A'+'a'; // low case
	if (v>='a' && v<='f') return 10+(v-'a');
	return 0;
};

TDWORD CTString::cstr_scanColour( const TSBYTE *string, TDWORD def ) {
	if (string==0 || string[0] == 0) return def;
	return (
		   (cstr_getHex(string[0])<<28) | (cstr_getHex(string[1])<<24) |
		   (cstr_getHex(string[2])<<20) | (cstr_getHex(string[3])<<16) |
		   (cstr_getHex(string[4])<<12) | (cstr_getHex(string[5])<<8) |
		   (cstr_getHex(string[6])<<4) | (cstr_getHex(string[7])<<0) 
		   );
};

TDWORD CTString::cstr_scanShortColour( const TSBYTE *string, TDWORD def ) {
	if (string==0 || string[0] == 0) return def;
	return (
		   (cstr_getHex(string[0])<<28) | (cstr_getHex(string[0])<<24) |
		   (cstr_getHex(string[1])<<20) | (cstr_getHex(string[1])<<16) |
		   (cstr_getHex(string[2])<<12) | (cstr_getHex(string[2])<<8) |
		   (cstr_getHex(string[3])<<4) | (cstr_getHex(string[3])<<0) 
		   );
};
