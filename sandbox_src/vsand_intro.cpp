/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include <stdlib.h>
#include "TMath.h"
#include "TBase.h"
#include "vsand_intro.h"
#include "TDrawHelpers.h"

#include "intro_digia.cpp"
#include "intro_flare.cpp"
#include "intro_sandpaint.cpp"					// 500,83

extern unsigned int sand_texture[];


CVSand_Intro::CVSand_Intro( bool grab_bg) {
    m_cancelled = false;
    m_flare.setAsReference( intro_flare, 128,128,128, eTSURFACEFORMAT_RGBA8888 );
    m_sand.setAsReference( sand_texture, 128,128,128, eTSURFACEFORMAT_RGBA8888 );
    m_screenCounter = 0;
    m_screenIndex = -1;
    m_bg = 0;
    m_grabBackground = grab_bg;
    for (int f=0; f<1024; f++) m_cosTable[f] = (int)( tmath_cos( (float)f/1024.0f * 3.141549f*2.0f ) * (float)FP_VAL );
};

CVSand_Intro::~CVSand_Intro() {
    if (m_bg) delete m_bg;
};

int CVSand_Intro::run( int ft ) {

    if (m_cancelled==true) return 0;

#ifdef WIN32
    m_screenCounter+=3000;
#else
    m_screenCounter+=6600;       // was 3000;
#endif
    if (m_screenIndex==-1) {
        m_currentLogo.setAsReference( intro_digia, 360,180,360, eTSURFACEFORMAT_RGBA8888 );
        m_screenIndex = 1;
        m_screenCounter = 0;
    };

    switch (m_screenIndex) {
    default:
        break;

       case 1:
        if (m_screenCounter>65536*15) {
            m_screenIndex = 3;
            m_screenCounter = 0;
            m_currentLogo.setAsReference( intro_sandpaint, 481,116,481, eTSURFACEFORMAT_RGBA8888 );
        };
        break;
       case 3:
        if (m_screenCounter>65536*15) return 0;
        break;

        break;

    };

    return 1;			// continue running
};



void vertical_light_line( TDWORD *t, TDWORD *s, int pitch, int h, int oy, int length, int height ) {

    t-=pitch*height;
    int a;
    register TDWORD temp, mask;
    for (int y=-height; y<=height; y++) {
        if (oy+y>=0 && oy+y<h)
        {
            TDWORD *t_target = t+length;
            int limit = (abs(y)<<8)/height;


            while (t!=t_target) {
                //a=((((s[0]>>16)&255)*(s[0]>>24))>>8)-limit;
                //a =((s[0]>>8)&255)-limit;
                a = (((((s[0])&255) + ((s[0]>>16)&255))*((s[0]>>24)&255)*18) >>13)-limit;

                if (a>0) {
                    //a = (a*a)>>8;
                    if (a>255) a = 255;
                    mask = (a|(a<<8)|(a<<16));
                    temp = ((*t&0xFEFEFEFE)>>1)+((mask&0xFEFEFEFE)>>1);
                    mask = (temp&0x80808080);
                    temp |= (mask-(mask>>7));
                    *t =  ((temp&0x7F7F7F7F)<<1);			// color here
                };
                //*t = 0xffff00ff;

                s++;
                t++;
            };

            s-=length;
            t-=length;
        }
        t+=pitch;
    }
};


void CVSand_Intro::effectBlit1( CTSurface_RGBA8888 &target, CTSurface_RGBA8888 &source, int x, int y, int linelen, int fadeaway ) {

    //target.blit(x,y, &source);
    //return;

    TDWORD *t = (TDWORD*)target.getData() + target.getPitch()*y +x;
    TDWORD *s = (TDWORD*)source.getData();

    int val = (m_screenCounter>>5);
    int ytarget = (((source.getHeight()+64) * val)>>FP_BITS);



    int delta;
    int a;
    int ia;
    int whitec;
    //int r,g,b;

    for (int yloop=0; yloop<source.getHeight(); yloop++) {

        TDWORD *t_horline_target = t+source.getWidth();
        delta = (ytarget-yloop);
        //a = abs(delta)*2;
        a = 255+delta*32;
        if (a>255) a = 255;
        a = ((a*fadeaway)>>8);
        ia = (32-abs(delta))*8;
        if (ia<0) ia = 0;
        a -= ia;




        //whitec = 700-abs(delta)*5;
        whitec = 340-(delta*delta)/36;
        if (whitec>255) whitec = 255;
        if (whitec<0) whitec = 0;


        if (a>0) {				// basic alpha horline
            while (t != t_horline_target) {

                ia = ((a*(*s>>24))>>8);

                //if (1) {
                if (ia>0) {

                    *t = (((((*s&255)*(255^whitec)>>8) + whitec)*ia + ((*t)&255)*(255^ia))>>8) |
                         (((((((*s>>8)&255)*(255^whitec)>>8) + whitec)*ia + (((*t)>>8)&255)*(255^ia) )>>8)<<8) |
                         (((((((*s>>16)&255)*(255^whitec)>>8) + whitec)*ia + (((*t)>>16)&255)*(255^ia) )>>8)<<16);



                }



                t++;
                s++;
            };
            t-=source.getWidth();s-=source.getWidth();
        }


        t+=target.getPitch();
        s+=source.getPitch();
    };

    if (m_grabBackground==false) reflect(target,  y+source.getHeight()-7, 100);

    if (ytarget<0) ytarget = 0;
    if (ytarget>source.getHeight()-1) ytarget = source.getHeight()-1;

    vertical_light_line( (TDWORD*)target.getData() + target.getPitch()*(y+ytarget)+x,
                         (TDWORD*)source.getData()+ytarget*source.getPitch(),
                         target.getPitch(), target.getHeight(), y+ytarget, source.getWidth(), linelen );

};


void CVSand_Intro::delightRed( CTSurface_RGBA8888 &target,  int lx, int amount ) {


    int temp, iamount;

    for (int x=0; x<target.getWidth(); x++) {
        TDWORD *t=(TDWORD*)target.getData() + x;
        TDWORD *t_target = t+target.getPitch()*target.getHeight();
        iamount = ((x-lx)*16);
        if (iamount<0) iamount = 0;
        iamount+=amount;
        if (iamount>256) iamount = 256;


        while (t!=t_target) {
            temp = ((int)(((*t)>>16)&255)+iamount) - (int)((((*t)>>8)&255) + ((*t)&255));

            //temp = ((temp*amount)>>8);
            temp = 255-temp;


            if (temp<1) temp = 0;
            //if ((unsigned int)temp>0) {
            if (temp<256) {
                *t = ((((*t)&255)*temp)>>8) |
                     ((((((*t)>>8)&255)*temp)>>8)<<8) |
                     ((((((*t)>>16)&255)*temp)>>8)<<16) |
                     ((*t)&0xFF000000);
            };
            t+=target.getPitch();
        };
    };
};


void CVSand_Intro::reflect( CTSurface_RGBA8888 &target, int y, int height ) {
    int val = 1;
    while (val<height) {
        if (y+val>=target.getHeight()) return;
        TDWORD *s = (TDWORD*)target.getData()+(y-val)*target.getPitch();
        TDWORD *t = (TDWORD*)target.getData()+(y+val)*target.getPitch();
        TDWORD *t_target = t+target.getWidth();
        int mul = (height-val)*64/height;
        while (t!=t_target) {
            *t =  ((((*s)&255)*mul)>>8) | ((((((*s)>>8)&255)*mul)>>8)<<8) | ((((((*s)>>16)&255)*mul)>>8)<<16);
            s++;
            t++;
        };

        val++;
    };
};



void CVSand_Intro::sandBlit1( CTSurface_RGBA8888 &target, int x, int y, CTSurface_RGBA8888 &source, int phase ) {

    int targetx = -(source.getWidth()/2) + ((source.getWidth() * phase)>>16);

    TDWORD *sand = (TDWORD*)m_sand.getData();
    int spitch = source.getPitch();
    int tpitch = target.getPitch();

    TDWORD te, te2;
    int a,ia;
    int ofs;
    int tosand;
    int towhite;
    int delta, yloop;
    for (int xloop=0; xloop<source.getWidth(); xloop++) {
        TDWORD *s = (TDWORD*)source.getData() + xloop;
        TDWORD *s_target = s+source.getHeight()*spitch;
        TDWORD *t = (TDWORD*)target.getData() +  (xloop+x) + (y*tpitch);

        delta = (targetx-xloop);
        a = 255+delta;
        if (a>255) a = 255;
        tosand = 300-delta*10;
        if (tosand<0) tosand = 0;
        if (tosand > 255) tosand = 255;

        //towhite = 512 - delta*8;
        towhite = 300 - ((delta*delta)>>4);
        if (towhite > 255) towhite = 255;
        if (towhite<0) towhite = 0;
        //tosand = 0;

        yloop=0;

        if (a>0)
            while (s!=s_target) {
            te = sand[((yloop&127)<<7) + (xloop&127) ];			// sand color
            ia = ((a*(*s>>24))>>8);

            ofs = (-120-delta*3 + (((*s>>0)&255)>>1) + ((te>>24)>>1) ); // * (int)(255^a) )>>8;

            //towhite = 128+ofs;
            if (ofs<0) ofs = 0;

            //ofs = (ofs * (int)(te>>24)) >> 8;

            //int debofs = ofs;

            ofs = ((-m_cosTable[ (((ofs<<1)+256)&1023) ]*ofs)>>18) +((((-FP_VAL*3/2+m_cosTable[ (((ofs<<2)+256)&1023) ])*ofs)>>18))*tpitch;


            te2 = (((((*s)>>0)&255)*(255^towhite)>>8) + towhite)  |
                  ((((((*s)>>8)&255)*(255^towhite)>>8) + towhite)<<8) |
                  ((((((*s)>>16)&255)*(255^towhite)>>8) + towhite)<<16);


            t[ofs] = ((( (( ( (((te2)>>0)&255)*(255^tosand) + (((te>>0)&255)*tosand) ) * ia ) >> 8) + (((t[ofs])>>0)&255)*(255^ia) ) >> 8)<<0) |
                     ((( (( ( (((te2)>>8)&255)*(255^tosand) + (((te>>8)&255)*tosand) ) * ia ) >> 8) + (((t[ofs])>>8)&255)*(255^ia) ) >> 8)<<8) |
                     ((( (( ( (((te2)>>16)&255)*(255^tosand) + (((te>>16)&255)*tosand) ) * ia ) >> 8) + (((t[ofs])>>16)&255)*(255^ia) ) >> 8)<<16);




            //*t = *s;
            s+=spitch;
            t+=tpitch;
            yloop++;
            //xx+=1024;
        };


        //addline


        //yy+=1024;
    };

};



void CVSand_Intro::draw( CTSurface_RGBA8888 *target ) {


    // USE BG
    if (!m_bg) {
        if (m_grabBackground) {
            m_bg = new CTSurface_RGBA8888;
            m_bg->create( target->getWidth(), target->getHeight(), eTSURFACEFORMAT_RGBA8888 );
            m_bg->blit(0,0, target);
        } else target->clear();
    } else target->blit( 0,0, m_bg );


    if (m_screenIndex==3) {
	

        if (m_screenCounter<65536*14) {
            sandBlit1( *target, (target->getWidth()- m_currentLogo.getWidth())/2, (target->getHeight()- m_currentLogo.getHeight())/2, m_currentLogo, m_screenCounter>>3 );
        } else {
            int fadeout = 255-((m_screenCounter-65536*14)>>7);
            if (fadeout<0) fadeout = 0;
            target->blit( (target->getWidth()- m_currentLogo.getWidth())/2, (target->getHeight()- m_currentLogo.getHeight())/2, &m_currentLogo,eTBLITOPERATION_ALPHA, 0x00FFFFFF | (fadeout<<24) );
        }
        if (m_grabBackground == false) reflect(*target, target->getHeight()/2+47, 100);


    }




    if (m_screenIndex == 1) {			// ** DIGIA LOGO **, hackthon, logo


        int fadeaway = 255;
        // digia flare
        fadeaway = 255-((m_screenCounter-65536*13)>>9);
        if (fadeaway<0) fadeaway = 0;
        if (fadeaway>255) fadeaway = 255;

        effectBlit1(*target, m_currentLogo, (target->getWidth() - m_currentLogo.getWidth())/2,(target->getHeight() - m_currentLogo.getHeight())/2, 160, fadeaway);

        int FLARE_BEGIN = 65536*78/16;
        if (m_screenCounter>FLARE_BEGIN) {
            int p = (((m_screenCounter-FLARE_BEGIN)>>10)&127);
            int a = (((255-p*2) * fadeaway)>>8);

            if (a>255) a = 255;
            int vx = m_cosTable[ ((m_screenCounter>>8) & 1023) ]*(4+p*3/2);
            int vy = m_cosTable[ ((m_screenCounter>>9)+256) & 1023 ]*(4+p*3/2);

            CTDrawHelpers::rotateBlit(target, &m_flare, (target->getWidth()-34) << 13, (target->getHeight()+118)<<13, vx,vy, a | (a<<8) | (a<<16), eTBLITOPERATION_ADD );
        }
    }
};
