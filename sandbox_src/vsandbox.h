/**
 * The sandbox-playground
 * Copyright (c) 2010 Nokia Corporation.
 *
 */



#ifndef __VSANDBOX__
#define __VSANDBOX__

#include "TSurface_RGBA8888.h"
#include "vsand_grid.h"
#include "T2DMath.h"


// tools defined here for compatibility
enum eSANDTOOL { eSANDTOOL_NONE,
                 eSANDTOOL_SCRATCH_TO_TARGET,
                 eSANDTOOL_CURVE,
                 eSANDTOOL_ADD,
                 eSANDTOOL_PUSH,
                 eSANDTOOL_PUSH_TOWARDS,
                 eSANDTOOL_PULL,
                 eSANDTOOL_GRAVITY,
                 eSANDTOOL_LIGHT };


#define SANDGRID_DIV 8
#define SANDGRID_DIV_BITS 3
#define SANDGRID_LIGHT_SHR 22   // with size 8, 22          in "sqr" space,.. 2 bits per 1 change
#define SANDGRID_SAND_SHR 12

#define BG_SIZE 256


class CVSandBox {
public:
	CVSandBox( int simw, int simh );
	~CVSandBox();
    void setSize(int simw, int simh );
	int run( int fixedFrameTime );
        int render( CTSurface_RGBA8888 &target, int *bound_rect = 0 );

		// getters/setters
	inline CT2DVector &gravity() { return m_vGravity; }
        inline CT2DVector &light() { return m_vLight; }             // note, use only as get.
        inline CVSand_Grid *getGrid() { return m_sandGrid; }

        void setLight( int x, int y );
        void relightBg( int mul );

        void autoPaintTo( CTSurface_RGBA8888 &target, int type=-1, int power = 35 );
        void autoPaintToResetTarget( int index, int type=-1 );
        bool isScratching() { if (m_autoPaintType==4) return true; else return false; }
        TDWORD* getAutoPaintTargetMem() { return (TDWORD*)m_autoPaintTarget.getData(); }

        void shake( int dirx, int diry );
        void markAllChanged();

protected:
        void autoPaintStep();
        void drawGridItem( TDWORD *target,int pitch, SVSandNode *node, int u, int v );
	CVSand_Grid *m_sandGrid;
        CT2DVector m_vGravity;
	CT2DVector m_vLight;

        int m_bgLight;
        TDWORD *m_litBackground;

        // autopaint
        TSDWORD m_autoPaintStatus;
        TSDWORD m_autoPaintType;
        CTSurface_RGBA8888 m_autoPaintTarget;
};



#endif
