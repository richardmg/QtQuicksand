#include "TEngine.h"
#include <stdlib.h>
#include "vsandbox.h"


#include "TDrawHelpers.h"
#include "table_texture.cpp"
#include "reset_images.cpp"

extern unsigned int sand_texture[];




CVSandBox::CVSandBox( int simw, int simh ) {
    m_autoPaintStatus = -1;
    m_vGravity.set( 0, 0 );
    m_sandGrid = new CVSand_Grid( simw/SANDGRID_DIV+1, simh/SANDGRID_DIV+1 );
    m_litBackground = new TDWORD[ BG_SIZE * BG_SIZE ];
};


CVSandBox::~CVSandBox() {
    delete m_sandGrid;
    delete [] m_litBackground;
};


void CVSandBox::autoPaintToResetTarget( int index, int type ) {
    CTSurface_RGBA8888 freset, sub;
    freset.setAsReference( reset_images, 64, 512, 64, eTSURFACEFORMAT_RGBA8888 );
    sub.setReference( &freset, 0, index*64, 64, 64 );
    autoPaintTo( sub, type, 30 );
};


void CVSandBox::autoPaintTo( CTSurface_RGBA8888 &target, int type,  int power ) {
    if (target.getData()==0) return;    // cannot continue

    if (type==-1) m_autoPaintType = rand()%4; else m_autoPaintType= type;           // 4 = autopaint-  scratch

    m_autoPaintTarget.create( m_sandGrid->width(), m_sandGrid->height(), eTSURFACEFORMAT_RGBA8888 );
    CTDrawHelpers::linearScaleImage( &target, &m_autoPaintTarget, 0);
    m_autoPaintStatus = 0;


    TDWORD *t = (TDWORD*)m_autoPaintTarget.getData();

    int i;
    int r,g,b;
        // preprocess
    for (int y=0; y<m_sandGrid->height(); y++) {
        for (int x=0; x<m_sandGrid->width(); x++) {

            //*t = (((*t&255) | (((*t)>>8)&255) | (((*t)>>8)&255) * 65536) / (256*3));
            r = *t&255;
            g = (((*t)>>8)&255)&255;
            b = (((*t)>>16)&255)&255;
            r = 255-r;
            g = 255-g;
            b = 255-b;

            i = tmath_sqrt( r*r+g*g+b*b )*power;           // amount, ..was 70
            if (i<0) i =0 ;
            *t = (TDWORD)i;
            t++;

        }

    };

};


void CVSandBox::shake( int dirx, int diry ) {
    m_sandGrid->selective_soften(0);                // first, run the soften.
    m_sandGrid->selective_soften(0);                // last, run the soften.
};


void CVSandBox::markAllChanged() {
    // mark everything changed.
    SVSandNode *mark = m_sandGrid->smap();
    SVSandNode *mark_target = mark + m_sandGrid->width() * m_sandGrid->height();
    while (mark!=mark_target) {
        mark->checksum = 8787;
        mark++;
    };
};

void CVSandBox::relightBg(int mul) {

    TDWORD *s = (TDWORD*)table_texture;

    mul = (mul*3/4)+FP_VAL/4;
    for (int i=0; i<256*256; i++) m_litBackground[i] =
            (((s[i]&255)*mul)>>FP_BITS) |
            (((((s[i]>>8)&255)*mul)>>FP_BITS)<<8) |
            (((((s[i]>>16)&255)*mul)>>FP_BITS)<<16) |
            0xFF000000;


    markAllChanged();
};



void CVSandBox::autoPaintStep() {

    switch (m_autoPaintType) {

    case 4:
        //m_ = eSANDTOOL_SCRATCH_TO_TARGET;
        if (m_autoPaintStatus<m_sandGrid->height()) {
            int rval;
            SVSandNode *g = m_sandGrid->smap()+m_autoPaintStatus*m_sandGrid->width();
            SVSandNode *g_target = g+m_sandGrid->width();
            while (g!=g_target) {
                g->amount = 16000+((rand()&255)<<6);
                g++;
            };
        }
        m_autoPaintStatus++;
        break;


    default:
    case 0:
        {
        TDWORD *t = (TDWORD*)m_autoPaintTarget.getData();
        SVSandNode *g = m_sandGrid->smap();
        SVSandNode *g_target = g+m_sandGrid->width()*m_sandGrid->height();

        while (g!=g_target) {
            if ((*t>>8) == (m_autoPaintStatus&127)) g->amount = *t;
            g++;
            t++;
        }

        m_autoPaintStatus++;
        if (m_autoPaintStatus>128*3) m_autoPaintStatus = -1;

        }
        break;

    case 1:
         {
            TDWORD *t = (TDWORD*)m_autoPaintTarget.getData();
            SVSandNode *g = m_sandGrid->smap();
            int v = m_autoPaintStatus%(m_sandGrid->height() +90);
            for (int y=0; y<m_sandGrid->height(); y++)
                for (int x=0; x<m_sandGrid->width(); x++) {
                if (y+((*t)>>8) == v) {
                    g->amount = *t;
                }
                t++;
                g++;
            };

            m_autoPaintStatus++;
            if (m_autoPaintStatus>(m_sandGrid->height()+90)*3) m_autoPaintStatus = -1;
            break;
        }

    case 2:
         {
            TDWORD *t = (TDWORD*)m_autoPaintTarget.getData();
            SVSandNode *g = m_sandGrid->smap();
            int v = m_autoPaintStatus%(m_sandGrid->width() +90);
            for (int y=0; y<m_sandGrid->height(); y++)
                for (int x=0; x<m_sandGrid->width(); x++) {
                if (x+((*t)>>8) == v) {
                    g->amount = *t;
                }
                t++;
                g++;
            };

            m_autoPaintStatus++;
            if (m_autoPaintStatus>(m_sandGrid->width()+90)*3) m_autoPaintStatus = -1;
            break;
        }
    }
};



int CVSandBox::run( int fixedFrameTime ) {
    for (int f=0; f<2; f++) if (m_autoPaintStatus>=0) autoPaintStep();

    m_sandGrid->run(  m_vGravity, m_vLight, m_bgLight );
    return 1;
};


void CVSandBox::setLight(int x, int y) {
    m_vLight.set(x,y*2);
    int l = m_vLight.length();

    if (l>FP_VAL*3) {
        //m_vLight.setLength( FP_VAL*3/2);
        m_vLight.m_x = tmath_fpmuldiv( m_vLight.m_x, FP_VAL*3, l);
        m_vLight.m_y = tmath_fpmuldiv( m_vLight.m_y, FP_VAL*3, l);
    }

    l/=3;
    int bgLight = tmath_fpdiv(FP_VAL*2, tmath_fpmul(l,l));
    if (bgLight>FP_VAL) bgLight = FP_VAL;

    m_bgLight = bgLight;
    relightBg( (FP_VAL+ bgLight)/2 );
};


void CVSandBox::drawGridItem( TDWORD *target,int pitch,  SVSandNode *node, int u, int v ) {

    int y;
    TDWORD *t_target;
    int s1 = (node[0].amount);
    int s2 = (node[1].amount);
    int s1inc = (node[m_sandGrid->width()].amount - s1);
    int s2inc = (node[m_sandGrid->width()+1].amount - s2);

    int l1 = (node[0].light);
    int l2 = (node[1].light);
    int l1inc = (node[m_sandGrid->width()].light - l1);
    int l2inc = (node[m_sandGrid->width()+1].light - l2);


    s1<<=SANDGRID_DIV_BITS;
    s2<<=SANDGRID_DIV_BITS;
    l1<<=SANDGRID_DIV_BITS;
    l2<<=SANDGRID_DIV_BITS;

    int a;
    TDWORD tecol;
    int inners, innersinc;
    int innerl, innerlinc;
    TDWORD *bg;

    for (y=0; y<SANDGRID_DIV; y++) {
        t_target = target+SANDGRID_DIV;
        bg = &m_litBackground[(v&255)<<8];

        inners = s1<<SANDGRID_DIV_BITS;
        innersinc = (s2-s1); //>>SANDGRID_DIV_BITS;

        innerl = l1<<SANDGRID_DIV_BITS;
        innerlinc = (l2-l1);//>>SANDGRID_DIV_BITS;

        while (target!=t_target) {
            tecol = sand_texture[ ((((v&127)<<7) | (u&127)) + ((inners>>SANDGRID_SAND_SHR)*3600))&16383];
            a = ((inners)>>11)-(tecol>>26);
            if (a>0) {
                if (a>=255) {
                    *target = (((tecol&255)*innerl) >> SANDGRID_LIGHT_SHR) |
                              (((((tecol>>8)&255)*innerl) >> SANDGRID_LIGHT_SHR)<<8) |
                              (((((tecol>>16)&255)*innerl) >> SANDGRID_LIGHT_SHR)<<16) |
                              0xFF000000;
                } else {
                    *target = ((((((tecol&255)*innerl) >> SANDGRID_LIGHT_SHR)*a) + ((bg[u&255]&255)*(255^a)))>>8) |
                              ((((((((tecol>>8)&255)*innerl) >> SANDGRID_LIGHT_SHR)*a) + (((bg[u&255]>>8)&255)*(255^a)))>>8)<<8) |
                              ((((((((tecol>>16)&255)*innerl) >> SANDGRID_LIGHT_SHR)*a) + (((bg[u&255]>>16)&255)*(255^a)))>>8)<<16) |
                              0xFF000000;
                };

            } else {
                *target = bg[u&255];
            };

            innerl+=innerlinc;
            inners+=innersinc;
            target++;
            u++;
        };


        u -= SANDGRID_DIV;
        v++;
        target-=SANDGRID_DIV;
        target+=pitch;

        s1+=s1inc;
        s2+=s2inc;
        l1+=l1inc;
        l2+=l2inc;
    };
};

int CVSandBox::render( CTSurface_RGBA8888 &target, int *bound_rect ) {

    int rval = 0;
    int tt;
    int sgw = m_sandGrid->width();

    SVSandNode *sdata = m_sandGrid->smap();
    for (tt=0; tt<m_sandGrid->width()*m_sandGrid->height(); tt++) {
        sdata->renderCheck= sdata->checksum;
        sdata->checksum = ((TSDWORD)sdata->light) ^ (((TSDWORD)(sdata->amount>>11))<<16); //^sdata->dir.m_x^sdata->dir.m_y;
        sdata++;
    };



    if (bound_rect) {
        bound_rect[0] = 1000;           // minx
        bound_rect[1] = 0;              // maxx
        bound_rect[2] = 1000;           // miny
        bound_rect[3] = 0;              // maxy
    };

    for (int y=0; y<m_sandGrid->height()-1; y++) {
        sdata = m_sandGrid->smap() + sgw * y;
        TDWORD *t = (TDWORD*)target.getData() + target.getPitch() * (y*SANDGRID_DIV);
        for (int x=0; x<sgw-1; x++) {

            // render check
            if ((sdata[0].checksum != sdata[0].renderCheck) ||
                (sdata[1].checksum != (sdata[1].renderCheck)) ||
                (sdata[sgw].checksum != (sdata[sgw].renderCheck)) ||
                (sdata[sgw+1].checksum != (sdata[sgw+1].renderCheck))
                ) {
                if (bound_rect) {
                    if (x<bound_rect[0]) bound_rect[0] =x;
                    if (x>bound_rect[1]) bound_rect[1] =x;
                    if (y<bound_rect[2]) bound_rect[2] =y;
                    if (y>bound_rect[3]) bound_rect[3] =y;

                };
                rval = 1;
                drawGridItem( t, target.getPitch(), sdata,x*SANDGRID_DIV,y*SANDGRID_DIV );
            }

            t+=SANDGRID_DIV;
            sdata++;
        }




    };

    if (bound_rect) {
        bound_rect[0] *= SANDGRID_DIV;
        bound_rect[1] *= SANDGRID_DIV;
        bound_rect[2] *= SANDGRID_DIV;
        bound_rect[3] *= SANDGRID_DIV;
    }
    return rval;
};
