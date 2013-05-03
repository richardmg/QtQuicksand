/**
 * The implementation for the intro with two logos
 * Copyright (c) 2010 Nokia Corporation.
 *
 */


#ifndef __CVSAND_INTRO__
#define __CVSAND_INTRO__

#include "TSurface_RGBA8888.h"


class CVSand_Intro {
public:
        CVSand_Intro(bool grab_bg );
	~CVSand_Intro();
	int run( int ft );
	void draw( CTSurface_RGBA8888 *target );

        inline void cancel() { m_cancelled = true; }
        int getScreenIndex() { return m_screenIndex; }


protected:
        bool m_cancelled;
        bool m_grabBackground;
	void fillbg( CTSurface_RGBA8888 *target );
	void effect2( CTSurface_RGBA8888 &target, CTSurface_RGBA8888 &source );

        void delightRed(CTSurface_RGBA8888 &target, int lx, int amount );
        void effectBlit1( CTSurface_RGBA8888 &target, CTSurface_RGBA8888 &source, int x, int y, int linelen, int fadeaway);
	
	void sandBlit1( CTSurface_RGBA8888 &target, int x, int y, CTSurface_RGBA8888 &source, int phase );


        void reflect( CTSurface_RGBA8888 &target, int y, int height );



        CTSurface_RGBA8888 m_tempBuffer;
	TSDWORD m_screenIndex;
	TSDWORD m_screenCounter;

	TSDWORD m_cosTable[1024];

	CTSurface_RGBA8888 m_flare;
	CTSurface_RGBA8888 m_currentLogo;
	CTSurface_RGBA8888 m_sand;
        CTSurface_RGBA8888 *m_bg;
};



#endif
