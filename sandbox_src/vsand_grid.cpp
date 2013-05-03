/**
 * Copyright (c) 2010 Nokia Corporation.
 */


#include <math.h>
#include "TMath.h"
#include <memory.h>
#include <stdlib.h>
#include "vsand_grid.h"


#define SELECTIVE_SOFTEN_EPSILON 6000           // 1600 was working quite nicely


CVSand_Grid::CVSand_Grid( int w, int h ) {
    m_width = w;
    m_height = h;
    m_smap = new SVSandNode[ m_width * m_height ];

    m_maxFixedWidth = ((m_width-2)<<16)+65535;
    m_maxFixedHeight = ((m_height-2)<<16)+65535;
    resetGrid( 0 );
};


CVSand_Grid::~CVSand_Grid() {
    if (m_smap) delete [] m_smap;
};

void CVSand_Grid::resetGrid( int type ) {
    memset( m_smap, 0, m_width * m_height*sizeof(SVSandNode));

    for (int f=0; f<m_width*m_height; f++) {
        m_smap[f].checksum = 1000;
    };
};


#define TRANSMIT_EPSILON 0

SVSandNode* CVSand_Grid::gi( int x, int y) {
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (x>=m_width) x = m_width-1;
    if (y>=m_height) y = m_height-1;
    return &m_smap[ y*m_width + x ];
};

int CVSand_Grid::getAmount( const int x, const int y) {
    return gi(x,y)->amount;

};

void CVSand_Grid::grid_push(SVSandNode *o, int posx, int posy) {
    if (posx<0) posx = 0;
    if (posy<0) posy = 0;
    if (posx>m_maxFixedWidth) posx = m_maxFixedWidth;
    if (posy>m_maxFixedHeight) posy = m_maxFixedHeight;

    int ofsx = (posx&65535)>>2;
    int ofsy = (posy&65535)>>2;
    int ul_mul = ((((16384-ofsx) * (16384-ofsy)) >> 14));
    int ur_mul = ((((ofsx) * (16384-ofsy)) >> 14));
    int ll_mul = ((((16384-ofsx) * (ofsy)) >> 14));
    int lr_mul = ((((ofsx) * (ofsy)) >> 14));
    posx>>=16;
    posy>>=16;

    SVSandNode *ul_node = m_smap+posy*m_width + posx;
    SVSandNode *ur_node = ul_node+1;
    SVSandNode *ll_node = ul_node+m_width;
    SVSandNode *lr_node = ll_node+1;

    ul_node->new_amount += ((o->amount*ul_mul)>>14);
    ur_node->new_amount += ((o->amount*ur_mul)>>14);
    ll_node->new_amount += ((o->amount*ll_mul)>>14);
    lr_node->new_amount += ((o->amount*lr_mul)>>14);

    ul_node->new_dir.m_x += ((o->dir.m_x*ul_mul)>>14);
    ul_node->new_dir.m_y += ((o->dir.m_y*ul_mul)>>14);

    ur_node->new_dir.m_x += ((o->dir.m_x*ur_mul)>>14);
    ur_node->new_dir.m_y += ((o->dir.m_y*ur_mul)>>14);

    ll_node->new_dir.m_x += ((o->dir.m_x*ll_mul)>>14);
    ll_node->new_dir.m_y += ((o->dir.m_y*ll_mul)>>14);

    lr_node->new_dir.m_x += ((o->dir.m_x*lr_mul)>>14);
    lr_node->new_dir.m_y += ((o->dir.m_y*lr_mul)>>14);
};




void CVSand_Grid::diffuse_step(int epsilon) {
    int dif, dx,dy;
    int x,y;

    SVSandNode *g;

    for (y=0; y<m_height; y++) {
        g = m_smap + y*m_width;
        for (x=0; x<m_width-1; x++) {
            // xdif
            dif = g->amount - g[1].amount;

            if (abs(dif)>epsilon) {
                dif >>= 2;
                g->amount -= dif;
                g[1].amount += dif;

                dif>>=1;
                g->dir.m_x += dif;
                g[1].dir.m_x += dif;



            }

            if (y<m_height-1) {		// ydif
                dif = g->amount - g[m_width].amount;
                if (abs(dif)>epsilon) {
                    dif >>= 2;
                    g->amount -= dif;
                    g[m_width].amount += dif;

                    dif>>=1;
                    g->dir.m_y += dif;
                    g[m_width].dir.m_y += dif;

                }
            }

            g++;
        };
    }

};


void CVSand_Grid::selective_soften(int epsilon ) {
    int x,y;
    SVSandNode *g;
    int dif;

    SVSandNode *lineplus, *lineminus;
    // selective soften
    for (y=0; y<m_height; y++) {
        g = m_smap+y*m_width;
        if (y>0) lineminus = g-m_width; else lineminus = g;
        if (y<m_height-1) lineplus = g+m_width; else lineplus = g;


        for (x=0; x<m_width; x++) {
            if (x>0 && x<m_width-1) {
                dif = ((g[-1].amount+g[1].amount+lineminus->amount+lineplus->amount)>>2);       // average
                if (abs(g->amount-dif)>epsilon) {
                    g->amount = (g->amount + dif)>>1;
                    g->dir.m_x=(g->dir.m_x>>1) + ((g[-1].dir.m_x+g[1].dir.m_x+lineminus->dir.m_x+lineplus->dir.m_x)>>3);
                    g->dir.m_y=(g->dir.m_y>>1) + ((g[-1].dir.m_y+g[1].dir.m_y+lineminus->dir.m_y+lineplus->dir.m_y)>>3);
                };
            } else {
                dif = ((gi(x-1,y)->amount + gi(x+1,y)->amount +gi(x,y-1)->amount + gi(x, y+1)->amount)>>2);
                if (abs(g->amount-dif)>epsilon) {
                    g->amount = (g->amount + dif)>>1;
                    g->dir.m_x=(g->dir.m_x>>1) + ((gi(x-1,y)->dir.m_x + gi(x+1,y)->dir.m_x +gi(x,y-1)->dir.m_x + gi(x, y+1)->dir.m_x)>>3);
                    g->dir.m_y=(g->dir.m_y>>1) + ((gi(x-1,y)->dir.m_y + gi(x+1,y)->dir.m_y +gi(x,y-1)->dir.m_y + gi(x, y+1)->dir.m_y)>>3);
                };

            };

            lineplus++;
            lineminus++;
            g++;
        };
    };
};



void CVSand_Grid::run(CT2DVector &gravity, CT2DVector &light, TSDWORD bgLight ) {
#ifndef WIN32
    runGridStep(gravity, light, -1 ); // Additional runstep.
#endif
    runGridStep(gravity, light, bgLight );
}


void CVSand_Grid::runGridStep( CT2DVector &gravity, CT2DVector &light, TSDWORD bgLight ) {		// process sand-cells

    int y;
    int v,i;
    int tx,ty;
    SVSandNode *t, *t_target;

    for (y=0; y<m_height; y++) {
        t = m_smap + (y*m_width);
        t_target = t+m_width;
        //int x=0;
        while (t!=t_target) {

            //t->dir.add( t->normal );
            if (abs(t->dir.m_x * t->dir.m_y) < 5000 || t->amount<5) {                  // stop the flow if small enough
                t->dir.set(0,0);
            } else {
                t->dir.m_x = ((t->dir.m_x * 212)>>8) + gravity.m_x; //+ ((gravity.m_x * t->amount)>>16);
                t->dir.m_y = ((t->dir.m_y * 212)>>8) + gravity.m_y; //+ ((gravity.m_y * t->amount)>>16);
            }

            t->new_amount = 0;
            t->new_dir.set( 0,0 );
            t++;
        }
    }


#ifdef SANDGRID_DEBUG_ON
    m_totalAmount = 0;
    m_bleededAmount = 0;
#endif


    // exchange.
    for (y=0; y<m_height; y++) {
        t = m_smap + (y*m_width);
        t_target = t+m_width;
        int x = 0;					// is required?
        while (t!=t_target) {

            if (t->dir.m_x!=0 || t->dir.m_y!=0) {
                grid_push(t, ((int)(x) << 16) + (t->dir.m_x>>1),((int)(y) << 16) + (t->dir.m_y>>1) );       // shr was 1
                //grid_push(t, ((int)(x) << 16),((int)(y) << 16) );
            } else {
                t->new_amount += t->amount;
                t->new_dir.add( t->dir );
            }

            x++;
            t++;
        }
    }



    t = m_smap;
    t_target = m_smap+m_width*m_height;
    while (t!=t_target) {
        t->amount = t->new_amount;
        t->dir.set(t->new_dir);
#ifdef SANDGRID_DEBUG_ON
    m_totalAmount += t->amount;
#endif

        t++;
    }

    CT2DVector d;
    //run_pressure();
    for (int am = 0; am<4; am ++) diffuse_step( SELECTIVE_SOFTEN_EPSILON );

    if (bgLight<0) return;      // this is not the finald step

    selective_soften( SELECTIVE_SOFTEN_EPSILON/4 );


    // calculate normals
    for (y=0; y<m_height; y++) {
        t = m_smap + (y*m_width);
        t_target = t+m_width;
        int x=0;
        while (t!=t_target) {

            if (x>0 && y>0 && x<m_width-1 && y<m_height-1) {

                tx = -t[-m_width-1].amount + t[-m_width+1].amount +
                     t[-1].amount*-2 + t[1].amount*2 +
                     -t[m_width-1].amount + t[m_width+1].amount;

                ty = -t[-m_width-1].amount + -t[-m_width].amount*2 + -t[-m_width+1].amount +
                     t[m_width-1].amount + t[m_width].amount*2 + t[m_width+1].amount;

            } else {

                tx = -getAmount(x-1,y-1) + getAmount(x+1,y-1) +
                     getAmount(x-1,y)*-2 + getAmount(x+1,y)*2 +
                     -getAmount(x-1,y+1) + getAmount(x+1,y+1);

                ty = -getAmount(x-1,y-1) + getAmount(x,y-1)*-2 + -getAmount(x+1,y-1) +
                     getAmount(x-1, y+1) + getAmount(x, y+1)*2 + getAmount(x+1, y+1);

            };

            t->normal.set( -(tx>>3),-(ty>>3) );
            t->light = ((65536*11)>>4) + (((t->normal.m_x * light.m_x) + (t->normal.m_y * light.m_y))>>13);

            t->light = ((t->light*(FP_VAL-bgLight))>>FP_BITS);
            t->light += ((((86000-t->amount)>>1)*bgLight)>>14);          // >> FP_BITS


            //t->light = 16000;
            if (t->light<0) t->light =0;
            if (t->light>65535) t->light = 65535;

            x++;
            t++;
        }
    }



};



int CVSand_Grid::curveMass( int x, int y, int r, int strength ) {
    int harvested = 0;
    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int temp,d;

    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);

                if (d<r) {
                    m = (strength * (r-d))/r;
                    temp = ((m_smap[yy*m_width+xx].amount * m)>>FP_BITS);
                    harvested += temp;
                    m_smap[yy*m_width+xx].amount -= temp;
                }
            };
        };
    };

    return harvested;
};



#define MAX_SPEED 65536/4

void CVSand_Grid::scratch_to_target( int x, int y, int r, int power, TDWORD *target_buffer, int *initial_dir ) {
    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int d;
    int target_amount;
    CT2DVector dir;

    /*
    if (initial_dir) {
        dir.set(initial_dir[0], initial_dir[1]);
        d = dir.length();
        if (d>MAX_SPEED*8)
            dir.setLength( MAX_SPEED*8 );

    }
    */

    dir.set(initial_dir[0], initial_dir[1]);


    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);

                if (d<r) {
                    target_amount = (int)target_buffer[yy*m_width+xx];

                    m = ((target_amount-m_smap[yy*m_width+xx].amount) * (r-d))/r;
                    m = ((m*power)>>FP_BITS);
                    m_smap[yy*m_width+xx].amount += m;
                    //m_smap[yy*m_width+xx].amount = target_amount;


                    if (initial_dir) {
                        m_smap[yy*m_width+xx].dir.m_x += tmath_fpmul( dir.m_x, m); //((dir.m_x * m)>>14);
                        m_smap[yy*m_width+xx].dir.m_y += tmath_fpmul( dir.m_y, m); // ((dir.m_y * m)>>14);
                    };

                }
            };
        };
    };
};


#define ADD_MAX_LIMIT (65536*3/4)

int CVSand_Grid::addMass( int x, int y, int r, int amount, int *initial_dir ) {
    int added = 0;
    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int d;
    CT2DVector dir;

    if (initial_dir) {
        dir.set(initial_dir[0], initial_dir[1]);
        d = dir.length();
        if (d>MAX_SPEED*3)
            dir.setLength( MAX_SPEED*3 );

    }


    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);

                if (d<r) {
                    m = (amount * (r-d))/r;
                    d = (m_smap[yy*m_width+xx].amount);
                    if (d+m>ADD_MAX_LIMIT) m = ADD_MAX_LIMIT-d;

                    if (m<0) m = 0;
                    added += m;
                    m_smap[yy*m_width+xx].amount += m;


                    if (initial_dir) {

                        m_smap[yy*m_width+xx].dir.m_x += ((dir.m_x * (m>>3))>>7);
                        m_smap[yy*m_width+xx].dir.m_y += ((dir.m_y * (m>>3))>>7);
                    };

                }
            };
        };
    };

    return added;

};

void CVSand_Grid::push( int x, int y,int r, int power, int *add_dir ) {
    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int d;

    int take = 0;

    CT2DVector dir;
    if (add_dir) {
        dir.set(add_dir[0], add_dir[1]);
        d = dir.length();
        if (d>MAX_SPEED)
            dir.setLength( MAX_SPEED );

    }


    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);

                if (d<r) {

                    m = (power * (r-d))/r;
                    dx = tmath_fpmuldiv(dx, m*3, d+1);           // normalize
                    dy = tmath_fpmuldiv(dy, m*3, d+1);

                    m_smap[yy*m_width+xx].dir.m_x += dx;
                    m_smap[yy*m_width+xx].dir.m_y += dy;

                    // take mass away from the centre
                    //take = ((m_smap[yy*m_width+xx].amount*m)>>FP_BITS);
                    //m_smap[yy*m_width+xx].amount-=take;



                    if (add_dir) {
                        m_smap[yy*m_width+xx].dir.m_x += ((dir.m_x * m)>>11);
                        m_smap[yy*m_width+xx].dir.m_y += ((dir.m_y * m)>>11);
                    };
                }
            };
        };
    };

};


void CVSand_Grid::pull( int x, int y,int r, int power ) {
    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int d;

    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);

                if (d<r) {

                    m = (power * (r-d))/r;

                    m_smap[yy*m_width+xx].dir.m_x -= (dx*m)>>9;
                    m_smap[yy*m_width+xx].dir.m_y -= (dy*m)>>9;

                    m_smap[yy*m_width+xx].dir.m_x -= (dy*m)>>9;
                    m_smap[yy*m_width+xx].dir.m_y += (dx*m)>>9;
                }
            };
        };
    };

};



void CVSand_Grid::push_towards( int x, int y,int dirx, int diry, int r, int power ) {

    CT2DVector dir;
    dir.set(dirx*8, diry*8);
    dir.normalize();

    int scanx = x>>FP_BITS;
    int scany = y>>FP_BITS;
    int scana = (r>>FP_BITS) + 1;
    int m;
    int dx,dy;
    int d;

    r>>=6;

    for (int yy=scany-scana; yy<=scany+scana; yy++) {
        for (int xx=scanx-scana; xx<=scanx+scana; xx++) {
            if (xx>=0 && yy>=0 && xx<m_width && yy<m_height) {
                dx = (((xx<<FP_BITS) + FP_VAL/2) - x)>>6;
                dy = (((yy<<FP_BITS) + FP_VAL/2) - y)>>6;
                d = tmath_sqrt(dx*dx+dy*dy);
                if (d<r) {
                    m = (power * (r-d))/r;
                    m_smap[yy*m_width+xx].dir.m_x += (dir.m_x*m)>>13;
                    m_smap[yy*m_width+xx].dir.m_y += (dir.m_y*m)>>13;
                }
            };
        };
    };

};

