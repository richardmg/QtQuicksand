/**
 * The fluid-sand simulation main class.
 * Copyright (c) 2010 Nokia Corporation.
 *
 */

#ifndef __CVSAND_GRID__
#define __CVSAND_GRID__

#include "T2DMath.h"
#include "TBase.h"

// single grid item of a fluid simulation.
struct SVSandNode {
	TSDWORD amount;
	CT2DVector dir;

        CT2DVector normal;
	TSDWORD light;

        int new_amount;
        CT2DVector new_dir;

        // checksums for knowing which nodes have been changed.
        unsigned long checksum;
        unsigned long renderCheck; // is used to see if something is changed. a Checksum
};


//#define SANDGRID_DEBUG_ON         // will enable the debug-functionality


class CVSand_Grid {
public:
	CVSand_Grid( int w, int h );
	~CVSand_Grid();

	void resetGrid( int type );

        void run(CT2DVector &gravity, CT2DVector &light, TSDWORD bgLight );

	inline int width() { return m_width; }
	inline int height() { return m_height; }
	inline SVSandNode* smap() { return m_smap; }
	

	int curveMass( int x, int y, int r, int strength );		// retruns the amount actually curved
        int addMass( int x, int y, int r, int amount, int *initial_dir );			// returns the amount actually placed
        void push( int x, int y,int r,  int power, int *add_dir );
        void push_towards( int x, int y,int dirx, int diry, int r, int power );
        void scratch_to_target( int x, int y, int r, int power, TDWORD *target_buffer, int *initial_dir );

        void pull( int x, int y,int r,  int power );


        void selective_soften( int epsilon );

#ifdef SANDGRID_DEBUG_ON
        int m_totalAmount;
        int m_bleededAmount;
#endif


protected:
        void runGridStep( CT2DVector &gravity, CT2DVector &light, TSDWORD bgLight );

        void diffuse_step( int epsilon );
        inline SVSandNode* gi( int x, int y);
        inline int getAmount( const int x, const int y);

	int m_width;
	int m_height;
        int m_maxFixedWidth;
        int m_maxFixedHeight;

	SVSandNode *m_smap;
        void grid_push( SVSandNode *o, int x, int y );
};


#endif
